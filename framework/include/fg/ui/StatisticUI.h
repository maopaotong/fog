/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "imgui.h"
#include <string>
#include "fg/ogre.h"
#include "fg/core.h"
#include "fg/util.h"
#include <fmt/format.h>
#include "UIState.h"
namespace fog
{
    class StatisticTrayUI : public UIState
    {
        Actor *state = nullptr;
        CoreMod *core;

    public:
        INJECT(StatisticTrayUI(CoreMod *core)) : UIState("StatisticTray"), core(core)
        {
            // Context<ActorPropEC>::get()->addListener(this);
        }

        void doOpen() override
        {

            // RenderWindow *window =
            // window->getStatistics();
            const Ogre::RenderTarget::FrameStats &fs = core->getWindowStatistics();
            ImGui::Text(fmt::format("FPS:     {:.2f}", fs.lastFPS).c_str());
            ImGui::Text(fmt::format("Tris:    {}", fs.triangleCount).c_str());
            ImGui::Text(fmt::format("Batches: {}", fs.batchCount).c_str());
        }
    };

}; // end of namespace
