/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/core.h"
#include "fg/Mod.h"
#include "fg/util/CostMap.h"
#include "fg/core/MaterialFactory.h"
#include "fg/demo/WorldState.h"

#include "imgui.h"
#include <fmt/format.h>
#include <random>
#include <OgreRenderWindow.h>
#include "fg/demo/GameTerrain.h"
#include "ActiveTrayUI.h"

namespace fog
{
    class QuitUI : public UIState
    {
        bool breakRenderRequested = false;
        RenderWindow *window;
        Viewport *vp;
        SceneManager *sceMgr;
        ActiveTrayUI *activeTrayUI = nullptr;

    public:
        QuitUI() : UIState("Quit")
        {
            CoreMod *core = Context<CoreMod>::get();
            this->window = core->getWindow();
            this->vp = core->getViewport();
            this->sceMgr = core->getSceneManager();

            //
        }
        virtual bool open() override
        {
            if (!this->active)
            {
                return false;
            }

            std::string fName = this->getFullName();
            ImGui::OpenPopup(fName.c_str());

            return ImGuiUtil::BeginPopupModalIfEnd(fName,[this](){
                this->doOpen();
            });        
        };
        void doOpen()
        {
            ImGui::Text("Quit confirm?");
            ImGui::Separator();

            if (ImGui::Button("Yes"))
            {
                // 处理“确定”逻辑
                Context<CoreMod>::get()->getImGuiApp()->breakRender();
                ImGui::CloseCurrentPopup(); // 关闭弹窗
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                this->active = false;
            }
        }
    };
}; // end of namespace
