/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
#pragma once

#include <Ogre.h>
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
#include "fg/State.h"
#include "PathState.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/util/CollectionUtil.h"

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

    public:
        EntityState(std::string mesh, float scale, float height, std::vector<std::string> aniNames) : mesh(mesh),
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
            SceneManager *sMgr = Context<CoreMod>::get()->getSceneManager();

            entity = sMgr->createEntity(mesh);
            entity->setQueryFlags(0x00000001);

            sceNode = sMgr->getRootSceneNode()->createChildSceneNode();
            sceNode->setScale(scale, scale, scale);
            sceNode->attachObject(entity);

            this->setSceneNode(sceNode);

            float height = 0.0f;// Context<Terrains>::get()->getHeightWithNormalAtWorldPosition(Vector3(0, 0, 0), nullptr);

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
