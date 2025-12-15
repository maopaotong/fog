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

namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;

    class EntityState : public State
    {

    protected:
        Ogre::Entity *entity;

        std::vector<std::string> aniNames;
        float scale;
        float heigh;
        float heighOffset;

        std::string mesh;
        CoreMod *core;

    public:
        EntityState(std::string mesh, float scale, float height, std::vector<std::string> aniNames, CoreMod *core)
            : mesh(mesh),
              core(core),
              scale(scale),
              heigh(height),
              aniNames(aniNames)
        {
            this->heighOffset = this->heigh / 2.0f * scale;
        }

        virtual ~EntityState()
        {
        }

        virtual void init() override
        {

            if (DEBUG_CHECK_IMPOSSIBLE_ERROR)
            {
                if (this->entity)
                {
                    throw std::runtime_error("entity already exists,entity state cannot init twice.");
                }
            }
            // Context<CoreMod>::get()
            SceneManager *sMgr =
                core->getSceneManager();

            entity = sMgr->createEntity(mesh);
            entity->setQueryFlags(0x00000001);

            sceNode = sMgr->getRootSceneNode()->createChildSceneNode();
            sceNode->setScale(scale, scale, scale);
            sceNode->attachObject(entity);

            this->setSceneNode(sceNode);

            float height = 0.0f; // Context<Terrains>::get()->getHeightWithNormalAtWorldPosition(Vector3(0, 0, 0), nullptr);

            sceNode->translate(Vector3(0, height + this->heighOffset, 0));
            // init task owner.
        }

        Ogre::Entity *getEntity()
        {
            return this->entity;
        }
        AnimationStateSet *getAllAnimationStates()
        {
            return entity->getAllAnimationStates();
        }
        std::vector<std::string> getAnimationNames()
        {
            return this->aniNames;
        }
        float getActorHighOffset()
        {
            return this->heighOffset;
        }
    };
}; // end of namespace
