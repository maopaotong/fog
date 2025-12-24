/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "imgui.h"
#include <string>
#include "fg/ogre.h"

#include "fg/util.h"
#include <fmt/format.h>
#include "UIState.h"
namespace fog
{
    class BuildingTrayUI : /* public Listener<State *, std::string &>,*/ public UIState
    {
        Actor *state;
        EventBus * eventBus;

    public:
        INJECT(BuildingTrayUI(EventBus * eventBus)) : UIState("BuildingTray"), 
        eventBus(eventBus),
        state(nullptr)
        {
            
        }
        void init() override
        {
            // actorPosition = this->getProperty<Vector3>("actor1.position", false);
            UIState::init();
            eventBus-> //
                subscribe<BuildingEventType, Actor *>([this](BuildingEventType et, Actor *s)
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

                SceneNode *sNode = state->getSceneNode();
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
