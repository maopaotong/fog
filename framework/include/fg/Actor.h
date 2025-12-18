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
        static Actor *get(Node *node)
        {
            const Any &any = node->getUserAny();
            if (any.isEmpty())
            {
                return nullptr;
            }

            Actor *state = Ogre::any_cast<Actor *>(any);
            return state;
        }
        static void set(SceneNode *node, Actor *state)
        {
            node->setUserAny(state);
        }

        static Actor *get(MovableObject *mo)
        {
            const Any &any = mo->getUserAny();
            if (any.isEmpty())
            {
                return nullptr;
            }

            Actor *state = Ogre::any_cast<Actor *>(any);
            return state;
        }

    protected:
        SceneNode *sceNode;
        //
        bool active = false;
       
        CellKey cKey;

    public:
        Actor(SceneNode *sceNode) : sceNode(sceNode)
        {
            // std::cout << "new State()" << this << "" << std::endl;
            Actor::set(sceNode, this);
        }
        virtual ~Actor()
        {
            // children no need to delete , unique ptr will help.
            //

            Actor::set(this->sceNode, nullptr);
            Node *pNode = this->sceNode->getParent();
            pNode->removeChild(this->sceNode);
            this->sceNode = nullptr;
        }

        Point2<float> getPosition(Transform::D3_NORMAL_D2 &d3_normal_d2)
        {
            Vector3 positionIn3D = this->getSceneNode()->getPosition();
            return Point2<float>::from(positionIn3D, d3_normal_d2);
        }
        //TODO remove dependence of CellKey
        void setPosition(CellKey cKey, Transform::D2H2D3 &d2h2d3)
        {
            Vector3 v3 = cKey.transform3(d2h2d3);
            this->getSceneNode()->setPosition(v3);
        }

        CellKey getCellKey()
        {
            return this->cKey;
        }
        void setCellKey(CellKey ckey)
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

        SceneNode *getSceneNode()
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
        
        virtual AnimationStateSet *getAllAnimationStates()
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
