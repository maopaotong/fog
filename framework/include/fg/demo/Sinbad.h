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

    class Sinbad : public EntityState
    {

        constexpr static float ACTOR_SCALE = 2.5f;
        constexpr static float ACTOR_HEIGHT = 10.0f;

    protected:

    public:
        INJECT(Sinbad(CoreMod * core,SceneNode * sceNode)) : EntityState("Sinbad.mesh", ACTOR_SCALE, ACTOR_HEIGHT, {"RunBase", "RunTop"},core,sceNode)
        {
        }

        ~Sinbad()
        {
        }

    };
}; // end of namespace
