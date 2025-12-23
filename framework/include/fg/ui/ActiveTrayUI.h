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
    class ActiveTrayUI : /* public Listener<State *, std::string &>,*/ public UIState
    {
        Actor *state;
        Event::Bus *eventBus;
        Actor *rootState;
        MovableStateManager * msm;
        BuildingStateManager * bsm;
    public:
        INJECT(ActiveTrayUI(Event::Bus *eventBus, MovableStateManager *msm, BuildingStateManager *bsm)) : UIState("ActiveActor"), msm(msm), bsm(bsm), state(nullptr), eventBus(eventBus)
        {
        }
        void init() override
        {
            // actorPosition = this->getProperty<Vector3>("actor1.position", false);
            UIState::init();
            eventBus-> //
                subscribe<MovableEventType, Actor *>([this](MovableEventType et, Actor *s)
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

                SceneNode *sNode = state->getSceneNode();
                ImGui::Text(("Active State: " + std::to_string((uintptr_t)state)).c_str());
                ImGui::SameLine();
                ImGuiUtil::Text(sNode->getPosition());
                counter++;
                // Time Cost is the key: the building speed depends on the worker number and building type
                //
                if (ImGui::Button("Building Center"))
                {
                    bsm->startLocator(BuildingType::Center);
                }
            }

            if (!counter)
            {
                ImGui::Text("No Active State");
                if (ImGui::Button("Active actor"))
                {
                    msm->forEach([this](Actor *state)
                                       {
                        Sphere *sb = dynamic_cast<Sphere*>(state);
                        if(sb){
                            msm->pick(sb);
                            return false;
                        }
                        return true; 
                    });
                }
            }
        }
    };

}; // end of namespace
