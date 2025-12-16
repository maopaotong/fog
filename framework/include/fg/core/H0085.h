/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <Ogre.h>
#include "fg/util.h"
#include "fg/Actor.h"
#include "PathState.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/core/EntityState.h"
namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;

    class H0085 : public EntityState
    {

        constexpr static float ACTOR_SCALE = 30.0f;
        constexpr static float ACTOR_HEIGHT = 10.0f;

    protected:
    public:
        H0085(CoreMod * core) : EntityState("hc28c.mesh", ACTOR_SCALE, ACTOR_HEIGHT, {},core)
        {
        }

        virtual ~H0085()
        {
        }

        virtual void init() override
        {
            EntityState::init();
            // this->sceNode->setOrientation();
            entity->getSubEntity(0)->setMaterialName("hc28c");
            this->sceNode->setOrientation(Ogre::Vector3(0, 0, 1).getRotationTo(Vector3(0, -1, 0)));
        }
    };
}; // end of namespace
