/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
#pragma once
#include <vector>
#include <random>
#include <cmath>
#include <cassert>
#include <iostream>
#include <OgreImage.h>
#include <OgreColourValue.h>
namespace fog
{

    class DiamondSquare
    {

    public:
        static void generateImg(Ogre::Image &img)
        {
            int size = img.getWidth();
            size = 513;
            std::vector<std::vector<float>> heightmap(size, std::vector<float>(size, 0.0f));
            generateAndNormalise(heightmap, size, 0.6, 8151245);
            eraseDetailWithinTerrainTypes(heightmap, size);
            for (int x = 0; x < size; x++)
            {
                for (int y = 0; y < size; y++)
                {
                    Ogre::ColourValue cv;
                    cv.r = cv.g = cv.b = heightmap[x][y];
                    img.setColourAt(cv, x, y, 0);
                }
            }
        }

        //
        // hide the detail within each type of terrain.
        static void eraseDetailWithinTerrainTypes(std::vector<std::vector<float>> &heightmap, int size)
        {
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    float h = heightmap[i][j];

                    // h = std::floor(h * 10) / 10.0f;
                    if (h <= .35)
                    {
                        h = 0.35;
                    }
                    else if (h <= .50)
                    {
                        h = .50;
                    }
                    else if (h <= .85)
                    {
                        h = .85;
                    }
                    else if (h <= 100)
                    {
                        h = 1.0;
                    }

                    heightmap[i][j] = h;
                    // heightmap[i][j] = 0;
                }
            }
        }

        //

        static void generateAndNormalise(std::vector<std::vector<float>> &heightmap, int size, float roughness, unsigned int seed)
        {

            generate(heightmap, size, roughness, seed);
            // determine the max and min val;
            float min_val = heightmap[0][0], max_val = heightmap[0][0];
            float total = 0.0f;
            for (const auto &row : heightmap)
            {
                for (float h : row)
                {
                    if (h < min_val)
                    {
                        min_val = h;
                    }
                    if (h > max_val)
                    {

                        max_val = h;
                    }
                    total += 1.0f;
                }
            }

            float rangeFloat = max_val - min_val + 1;
            // counting the samples scatter on [0..1000]
            int length = 1000;
            float *percentage = new float[length]{0.0f};

            for (int i = 0; i < size; ++i)
            {
                for (int j = 0; j < size; ++j)
                {
                    int idx = static_cast<int>(std::floor((heightmap[i][j] - min_val) / rangeFloat * length));
                    percentage[idx]++;
                }
            }

            // incremental percentage scatter on [0..1000]
            float count = 0.0f;
            for (int i = 0; i < length; i++)
            {
                count += percentage[i];
                percentage[i] = count / total; // percentage,
            }

            // incremental percentage as the height of position.
            for (int i = 0; i < size; ++i)
            {
                for (int j = 0; j < size; ++j)
                {
                    // height as index of incremental histogram.
                    int hAsIdx = static_cast<int>(std::floor((heightmap[i][j] - min_val) / rangeFloat * length));
                    heightmap[i][j] = percentage[hAsIdx];
                }
            }
            delete[] percentage;
        }

        static void generate(std::vector<std::vector<float>> &heightmap, int size, float roughness = 0.5f, unsigned int seed = 0)
        {
            // 检查 size 是否为 2^n + 1
            int n = static_cast<int>(log2(size - 1));
            std::cout << "size:" << size << std::endl;
            assert((1 << n) + 1 == size && "Size must be 2^n + 1 (e.g., 65, 129, 257)");

            // 初始化随机数生成器
            std::mt19937 rng(seed);
            std::uniform_real_distribution<float> rand(0.0f, 1.0f);

            // 初始化四个角（可设为 0 或小随机值）
            heightmap[0][0] = rand(rng);
            heightmap[0][size - 1] = rand(rng);
            heightmap[size - 1][0] = rand(rng);
            heightmap[size - 1][size - 1] = rand(rng);

            int step = size - 1; // 当前正方形边长
            float scale = 1.0f;  // 随机扰动幅度

            while (step > 1)
            {
                int half = step / 2;

                // ---------- Diamond Step ----------
                for (int y = 0; y < size - 1; y += step)
                {
                    for (int x = 0; x < size - 1; x += step)
                    {
                        float avg = (heightmap[y][x] +
                                     heightmap[y][x + step] +
                                     heightmap[y + step][x] +
                                     heightmap[y + step][x + step]) /
                                    4.0f;

                        heightmap[y + half][x + half] = avg + rand(rng) * scale;
                    }
                }

                // ---------- Square Step ----------
                for (int y = 0; y < size; y += half)
                {
                    for (int x = (y + half) % step; x < size; x += step)
                    {
                        float total = 0.0f;
                        int count = 0;

                        // 上
                        if (y - half >= 0)
                        {
                            total += heightmap[y - half][x];
                            ++count;
                        }
                        // 下
                        if (y + half < size)
                        {
                            total += heightmap[y + half][x];
                            ++count;
                        }
                        // 左
                        if (x - half >= 0)
                        {
                            total += heightmap[y][x - half];
                            ++count;
                        }
                        // 右
                        if (x + half < size)
                        {
                            total += heightmap[y][x + half];
                            ++count;
                        }

                        if (count > 0)
                        {
                            heightmap[y][x] = total / count + rand(rng) * scale;
                        }
                    }
                }

                step /= 2;
                scale *= roughness; // 衰减扰动
            }
        }
    };
};
