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
#include "CommandUI.h"
#include "MainUI.h"
#include "SceneNodeUI.h"
#include "EntryUI.h"
namespace fog
{
    class OnFrameUI : public ImGuiApp::FrameListener
    {
        CoreMod *core;
        bool breakRenderRequested = false;
        // RenderWindow *window;
        ActiveTrayUI *activeTrayUI = nullptr;
        MainUI *mainUI = nullptr;
        SceneNodeUI *sceneNodeUI = nullptr;
        CommandUI *cmdUI;
        EntryUI *entryUI;

    public:
        INJECT(OnFrameUI(EntryUI* entry, CoreMod * core)):
        core(core),
        entryUI(entry)
        {
            //this->core = Context<CoreMod>::get();

            // this->window = core->getWindow();

            //
            //this->activeTrayUI = new ActiveTrayUI(core, costMap);

            // this->mainUI = new MainUI(core);
            // this->sceneNodeUI = new Ogre::SceneNodeUI(core);
            // this->cmdUI = new CommandUI(core);
            
//            this->entryUI = new EntryUI();
            this->entryUI->init();
        }

        void onFrame(const Ogre::FrameEvent &evt)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
            // this->mainUI->Open();
            // this->activeTrayUI->Open();
            // this->sceneNodeUI->Open();
            // this->cmdUI->Open();

            this->entryUI->open();

            ImGui::PopStyleColor();
        }
    };
}; // end of namespace
