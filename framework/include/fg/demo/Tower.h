/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "Common.h"
#include "fg/util.h"

#include "fg/cells.h"
#include "PathState.h"
#include "PathFollow2MissionState.h"
namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;

    class Tower : public EntityState
    {

        constexpr static float ACTOR_SCALE = 5.0f;
        constexpr static float ACTOR_HEIGHT = 10.0f;

    protected:
    public:
        INJECT(Tower(CoreMod *core,SceneNode* sNode)) : EntityState("watchtower.mesh", ACTOR_SCALE, ACTOR_HEIGHT, {}, core,sNode)
        {
        }

        virtual ~Tower()
        {
        }

    };
}; // end of namespace
