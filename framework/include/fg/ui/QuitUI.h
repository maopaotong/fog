/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/ogre.h"

#include "fg/util.h"
#include "fg/demo.h"

#include "imgui.h"
#include <fmt/format.h>
#include <random>
#include <OgreRenderWindow.h>
#include "ActiveTrayUI.h"

namespace fog
{
    class QuitUI : public UIState
    {
        bool breakRenderRequested = false;
        // RenderWindow *window;
        ActiveTrayUI *activeTrayUI = nullptr;
        CoreMod * core;
    public:
        INJECT(QuitUI(CoreMod * core)) : UIState("Quit"),core(core)
        {            
            // this->window = core->getWindow();

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
                core->getImGuiApp()->breakRender();
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
