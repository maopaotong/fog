/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
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
    class BuildingTrayUI : /* public Listener<State *, std::string &>,*/ public UIState
    {
        State *state;

    public:
        BuildingTrayUI() : UIState("BuildingTray"), state(nullptr)
        {
            
        }
        void init() override
        {
            // actorPosition = this->getProperty<Vector3>("actor1.position", false);
            UIState::init();
            Context<Event::Bus>::get()-> //
                subscribe<BuildingEventType, State *>([this](BuildingEventType et, State *s)
                                              {
                    if (et == BuildingEventType::StatePicked)
                    {
                        this->state = s;
                        this->setActive(true);
                    }
                    else if (et == BuildingEventType::StateUnpicked)
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
                ImGui::Text(("Building State: " + std::to_string((uintptr_t)state)).c_str());
                ImGui::SameLine();
                ImGuiUtil::Text(sNode->getPosition());
                counter++;
            }

            if (!counter)
            {
                ImGui::Text("No Building State");                
            }
        }
    };

}; // end of namespace
