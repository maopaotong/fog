/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"

//
#include "Common.h"
#include "PathState.h"
#include "PathFollow2MissionState.h"

namespace fog
{
    class EntityState : public Actor
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
        EntityState(std::string mesh, float scale, float height, std::vector<std::string> aniNames, CoreMod *core, SceneNode *sceNode)
            :Actor(sceNode), mesh(mesh),
              core(core),
              scale(scale),
              heigh(height),
              aniNames(aniNames)
        {
            this->heighOffset = this->heigh / 2.0f * scale;            
            // Context<CoreMod>::get()
          
            entity = core->createEntity(mesh);
            entity->setQueryFlags(0x00000001);

            //sceNode = sMgr->getRootSceneNode()->createChildSceneNode();
            sceNode->setScale(scale, scale, scale);
            sceNode->attachObject(entity);
            // this->setSceneNode(sceNode);
            sceNode->translate(Vector3(0, height + this->heighOffset, 0));
        }

        virtual ~EntityState()
        {
        }

        virtual void init() 
        {
           
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
