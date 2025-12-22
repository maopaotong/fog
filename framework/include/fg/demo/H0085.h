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
    class H0085 : public EntityState
    {

        constexpr static float ACTOR_SCALE = 30.0f;
        constexpr static float ACTOR_HEIGHT = 10.0f;

    protected:
    public:
        H0085(CoreMod * core,SceneNode* sNode) : EntityState("hc28c.mesh", ACTOR_SCALE, ACTOR_HEIGHT, {},core,sNode)
        {
            entity->getSubEntity(0)->setMaterialName("hc28c");
            this->sceNode->setOrientation(Ogre::Vector3(0, 0, 1).getRotationTo(Vector3(0, -1, 0)));
        }

        virtual ~H0085()
        {
        }

    };
}; // end of namespace
