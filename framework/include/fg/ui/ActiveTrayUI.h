/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "imgui.h"
#include <string>
#include "fg/core/CoreMod.h"
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
