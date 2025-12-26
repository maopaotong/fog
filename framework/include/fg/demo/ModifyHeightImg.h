/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Common.h"
#include "OgreColourValue.h"

//using namespace Ogre;
class ModifyHeightImg
{
    std::vector<float> height;
    Ogre::Image &img;
    int W;

public:
    ModifyHeightImg(Ogre::Image &img) : height(img.getWidth() * img.getWidth()), img(img)
    {
        W = img.getWidth();
        for (int i = 0; i < W; ++i)
        {
            for (int j = 0; j < W; ++j)
            {
                Ogre::ColourValue cv = img.getColourAt(i, j, 0);

                int value = static_cast<int>(2.0 * cv.r);
                height[i * W + j] = static_cast<float>(value) / 2.0f;
            }
        }
    }
    void modifyHeight()
    {
        std::vector<float> smoothHeight(W * W);
        modifyHeightInternal(smoothHeight);
        for (int i = 0; i < W; ++i)
        {
            for (int j = 0; j < W; ++j)
            {
                float h = smoothHeight[i * W + j];
                Ogre::ColourValue cv;
                cv.r = cv.g = cv.b = h;
                img.setColourAt(cv, i, j, 0);
            }
        }
    }

private:
    bool floatEquals(float f1, float f2)
    {
        const float EPSILON = 0.0001f;
        return std::abs(f1 - f2) < EPSILON;
    }

    bool floatZero(float f)
    {
        return std::abs(f) < 0.0001f;
    }

    void modifyHeightInternal(std::vector<float> &smoothHeight)
    {
        const int TRANSITION_WIDTH = 5; // 过渡带半宽（总宽约 2*W+1）

        const int LOOPS = 10;

        const float STEP_SIZE = 1.0f / static_cast<float>(LOOPS);

        // 1. 读取原始二值高度图（0.0f 或 1.0f）

        // ... fill height ...

        // 2. 创建两个距离图：dist0（到最近0的距离），dist1（到最近1的距离）
        std::vector<float> dist0(W * W, 1.0f);
        std::vector<float> dist1(W * W, 1.0f);
        // 初始化
        for (int i = 0; i < W * W; ++i)
        {
            if (floatEquals(height[i], 0.0f))
                dist0[i] = 0.0f;
            if (floatEquals(height[i], 1.0f))
                dist1[i] = 0.0f;
        }

        // 3. 使用多轮传播（类似 BFS）计算欧氏距离（简化：用 Manhattan 或 Chamfer）
        // 这里用 2-pass 动态规划近似（仅支持 4-邻域 Manhattan 距离）

        auto updateDist = [&](std::vector<float> &dist)
        {
            // forward pass
            for (int y = 0; y < W; ++y)
                for (int x = 0; x < W; ++x)
                {
                    float d = dist[y * W + x];
                    if (x > 0)
                        d = std::min(d, dist[y * W + x - 1] + STEP_SIZE);
                    if (y > 0)
                        d = std::min(d, dist[(y - 1) * W + x] + STEP_SIZE);
                    dist[y * W + x] = d;
                }
            // backward pass
            for (int y = W - 1; y >= 0; --y)
                for (int x = W - 1; x >= 0; --x)
                {
                    float d = dist[y * W + x];
                    if (x < W - 1)
                        d = std::min(d, dist[y * W + x + 1] + STEP_SIZE);
                    if (y < W - 1)
                        d = std::min(d, dist[(y + 1) * W + x] + STEP_SIZE);
                    dist[y * W + x] = d;
                }
        };

        for (int i = 0; i < LOOPS; i++)
        {

            updateDist(dist0);
            updateDist(dist1);
        }

        // 4. 生成平滑高度图

        for (int i = 0; i < W * W; ++i)
        {
            float d0 = dist0[i]; // 到最近0的距离
            float d1 = dist1[i]; // 到最近1的距离

            // 如果在纯0区或纯1区，保持原值
            if (floatEquals(d1, 0.0f))
            {
                smoothHeight[i] = 1.0f;
            }
            else if (floatEquals(d0, 0.0f))
            {
                smoothHeight[i] = 0.0f;
            }
            else
            {
                // 在过渡带：根据距离比例插值
                // float total = d0 + d1;
                // float t = d0 / total; // 越靠近1，t 越大 → 高度越高
                // float h = t;          // 线性插值：0~1
                //
                // float t = d0 / (d0 + d1);
                // float h = t * t * (3 - 2 * t); // smoothstep
                float total = d0 + d1; //
                float t = d0 / total;
                float h = t;
                //h = t * t * (3 - 2 * t); // smoothstep


                // 可选：限制过渡带宽度（只在 width 范围内平滑）
                if (d0 > TRANSITION_WIDTH && d1 > TRANSITION_WIDTH)
                {
                    // 超出过渡带，保持原值（但理论上不会发生）
                    smoothHeight[i] = height[i];
                    smoothHeight[i] = h;
                }
                else
                {
                    smoothHeight[i] = h;
                }
            }
        }
    }
};
