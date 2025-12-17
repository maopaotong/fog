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
    class PolicyUI : public UIState
    {
        CoreMod *core;
        Actor *state = nullptr;
        CostMap *costMap;
        Property::Ref<Vector3> actorPosition;

    public:
        PolicyUI(CoreMod *core, CostMap *costMap) : UIState("ActiveActor")
        {
            this->core = core;
            this->costMap = costMap;            
        }
        void init() override
        {            
            UIState::init();
        }

        void doOpen() override
        {

            if (state)
            {
                
            }
            else
            {
                
            }
        }
    };

}; // end of namespace
