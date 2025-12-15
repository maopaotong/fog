/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <Ogre.h>
#include "fg/util.h"
#include "fg/State.h"
#include "PathState.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/core/EntityState.h"
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
        INJECT(Tower(CoreMod *core)) : EntityState("watchtower.mesh", ACTOR_SCALE, ACTOR_HEIGHT, {}, core)
        {
        }

        virtual ~Tower()
        {
        }

        virtual void init() override
        {
            EntityState::init();
        }
    };
}; // end of namespace
