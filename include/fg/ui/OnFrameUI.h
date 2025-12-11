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
#include "fg/CoreMod.h"
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
        RenderWindow *window;
        Viewport *vp;
        SceneManager *sceMgr;
        ActiveTrayUI *activeTrayUI = nullptr;
        MainUI *mainUI = nullptr;
        SceneNodeUI *sceneNodeUI = nullptr;
        CommandUI *cmdUI;
        EntryUI *entryUI;

    public:
        OnFrameUI()
        {
            this->core = Context<CoreMod>::get();

            this->window = core->getWindow();
            this->vp = core->getViewport();
            this->sceMgr = core->getSceneManager();

            //
            //this->activeTrayUI = new ActiveTrayUI(core, costMap);

            // this->mainUI = new MainUI(core);
            // this->sceneNodeUI = new SceneNodeUI(core);
            // this->cmdUI = new CommandUI(core);
            
            this->entryUI = new EntryUI();
            this->entryUI->init();
        }

        void onFrame(const FrameEvent &evt)
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
