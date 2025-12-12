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
#include "imgui.h"
#include <string>
#include "fg/CoreMod.h"
#include "fg/State.h"
#include "fg/util.h"
#include <fmt/format.h>
#include "UIState.h"
namespace fog
{
    class ActiveTrayUI : /* public Listener<State *, std::string &>,*/ public UIState
    {
        State *state;

    public:
        ActiveTrayUI() : UIState("ActiveActor"), state(nullptr)
        {
        }
        void init() override
        {
            // actorPosition = this->getProperty<Vector3>("actor1.position", false);
            UIState::init();
            Context<Event::Bus>::get()-> //
                subscribe<MovableEventType, State *>([this](MovableEventType et, State *s)
                                                     {
                    if (et == MovableEventType::StatePicked)
                    {
                        this->state = s;
                        this->setActive(true);
                    }
                    else if (et == MovableEventType::StateUnpicked)
                    {
                        this->state = nullptr;
                        this->setActive(false);
                    } });
        }

        void doOpen() override
        {
            int counter = 0;
            if (state)
            {

                SceneNode *sNode = state->findSceneNode();
                ImGui::Text(("Active State: " + std::to_string((uintptr_t)state)).c_str());
                ImGui::SameLine();
                ImGuiUtil::Text(sNode->getPosition());
                counter++;
                // Time Cost is the key: the building speed depends on the worker number and building type
                //
                if (ImGui::Button("Tower Building(1*120-Person-Month)"))
                {
                    Context<BuildingStateManager>::get()->planToBuild(BuildingType::Tower);
                }

                if (ImGui::Button("Create Sinbad(1*10-Persen-Month)"))
                {

                    Context<MovableStateManager>::get()->createSinbad();
                }

                if (ImGui::Button("Build Farm(1*10-Persen-Month)"))
                {

                    Context<BuildingStateManager>::get()->planToBuild(BuildingType::Farm);
                }
                if (ImGui::Button("Build H0085"))
                {

                    Context<BuildingStateManager>::get()->planToBuild(BuildingType::H0085);
                }
            }

            if (!counter)
            {
                ImGui::Text("No Active State");
                if (ImGui::Button("Active actor"))
                {
                }
            }
        }
    };

}; // end of namespace
