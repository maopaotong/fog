/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <Ogre.h>
#include <imgui.h>
#include <vector>

namespace fog{
class ImGuiFPSCounter
{
public:
    ImGuiFPSCounter(Ogre::Timer *timer)
        : mTimer(timer), mLastTime(0.0f), mFps(0.0f), mFrameTimes(100, 0.0f) // 保留最近100帧
          ,
          mFrameIndex(0)
    {
    }

    void update()
    {
        float currentTime = mTimer->getMilliseconds();
        float deltaTimeMs = currentTime - mLastTime;    

        mLastTime = currentTime;

        if (deltaTimeMs > 0.0f)
        {
            mFps = 1000.0f / deltaTimeMs; // 毫秒 → FPS

            // 记录帧时间（用于绘图）
            mFrameTimes[mFrameIndex] = deltaTimeMs;
            mFrameIndex = (mFrameIndex + 1) % mFrameTimes.size();
        }
    }

    void showOverlay(bool *p_open = nullptr)
    {
        if (!ImGui::Begin("Performance", p_open))
        {
            ImGui::End();
            return;
        }

        // 显示当前 FPS
        ImGui::Text("FPS: %.1f", mFps);
        ImGui::SameLine();
        ImGui::Text(" | Frame Time: %.2f ms", 1000.0f / mFps);

        // 绘制帧时间曲线图
        ImGui::PlotLines("Frame Time (ms)",
                         mFrameTimes.data(),
                         static_cast<int>(mFrameTimes.size()),
                         mFrameIndex,
                         nullptr,
                         0.0f,
                         33.3f, // 假设目标 30 FPS（33.3ms）
                         ImVec2(0, 80));

        ImGui::End();
    }

private:
    Ogre::Timer *mTimer;
    float mLastTime = 0.0f;
    float mFps = 0.0f;
    std::vector<float> mFrameTimes;
    size_t mFrameIndex = 0;
    float rate = 1.0;
};
};//end of namespace
