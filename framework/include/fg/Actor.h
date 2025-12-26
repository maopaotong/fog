/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "Common.h"
#include "fg/util.h"
#include "Pickable.h"

#define WATCH_PROPERTY(monitor, obj_ptr, member_name) \
    (monitor).add((obj_ptr), &std::remove_pointer_t<decltype(obj_ptr)>::member_name, #member_name)

namespace fog
{

    class Actor
    {
    public:
        static Actor *get(Ogre::Node *node)
        {
            const Ogre::Any &any = node->getUserAny();
            if (any.isEmpty())
            {
                return nullptr;
            }

            Actor *state = Ogre::any_cast<Actor *>(any);
            return state;
        }
        static void set(Ogre::SceneNode *node, Actor *state)
        {
            node->setUserAny(state);
        }

        static Actor *get(Ogre::MovableObject *mo)
        {
            const Ogre::Any &any = mo->getUserAny();
            if (any.isEmpty())
            {
                return nullptr;
            }

            Actor *state = Ogre::any_cast<Actor *>(any);
            return state;
        }

    protected:
        bool active = false;
        float height = 0.0f;

    private:
        Ogre::SceneNode *sceNode;
        //

        CellKey::OffsetPointy cKey;

    public:
        Actor(Ogre::SceneNode *sceNode) : sceNode(sceNode)
        {
            Actor::set(sceNode, this);
        }
        virtual ~Actor()
        {
            Ogre::SceneNode *pSNode = sceNode->getParentSceneNode();
            pSNode->removeAndDestroyChild(sceNode);
        }

        template <typename F>
        Point2<float> getPosition(F &&d3_normal_d2)
        {
            Vector3 positionIn3D = this->getSceneNode()->getPosition();
            return Point2<float>::from(positionIn3D, d3_normal_d2);
        }
        // TODO remove dependence of CellKey
        // template <typename F>
        // void setPosition(CellKey::Offset cKey, F &&d2d3)
        // {
        //     Vector3 v3 = CellKey::transform3(cKey, d2d3);
        //     Vector3 v3 = CellKey::transform3(cKey, d2d3);
        //     this->getSceneNode()->setPosition(v3);
        // }
        void setPosition(Vector3 pos3){
            this->getSceneNode()->setPosition(pos3);
        }

        CellKey::OffsetPointy getCellKey()
        {
            return this->cKey;
        }
        void setCellKey(CellKey::OffsetPointy ckey)
        {
            this->cKey = ckey;
        }
        bool pickable()
        {
            if (this->active)
            {
                return false;
            }
            return true;
        }

        Ogre::SceneNode *getSceneNode()
        {
            return this->sceNode;
        }

        void setActive(bool active)
        {
            this->active = active;
        }

        bool isActive()
        {
            return this->active;
        }

        virtual Ogre::AnimationStateSet *getAllAnimationStates()
        {
            return nullptr;
        }
        virtual std::vector<std::string> getAnimationNames()
        {
            return std::vector<std::string>();
        }
        virtual float getActorHighOffset()
        {
            return 0.0f;
        }

    public:
    };
};
