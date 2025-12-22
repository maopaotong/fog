/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <Ogre.h>

#include "fg/util.h"
#include "fg/cells.h"
#include "PathFollow2MissionState.h"
namespace fog
{
    class Building : public ManualState
    {

    protected:
        BuildingType type;
        Transforms * tfs;

    public:
        INJECT(Building(BuildingType type, Transforms * tfs, CoreMod *core)) : ManualState(core),
                                                           tfs(tfs),
                                                                                            type(type)
        {
            this->material = MaterialNames::materialNameBuilding;
        }

        virtual ~Building()
        {
        }

    };
}; // end of namespace
