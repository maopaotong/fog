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
    using namespace Ogre;

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
        Actor *parent = nullptr;
        FrameListener *frameListener = nullptr;
        SceneNode *sceNode = nullptr;
        //
        List<UniquePtr<Actor>> children;
        bool active = false;
        Options options;
        std::string name;
        CellKey cKey;

        // template <typename T>
        // Property::Ref<T> createProperty(std::string name, T defaultValue)
        // {
        //     return Context<Property::Bag>::get()->createProperty<T>(name, defaultValue);
        // }

        // template <typename T>
        // Property::Ref<T> getProperty(std::string name)
        // {
        //     return Context<Property::Bag>::get()->getProperty<T>(name);
        // }

    public:
        Actor(){

        }
        Actor(SceneNode * sceNode):sceNode(sceNode)
        {
            // std::cout << "new State()" << this << "" << std::endl;
            Actor::set(sceNode, this);            
        }
        virtual ~Actor()
        {
            // children no need to delete , unique ptr will help.
            // 
            if(this->sceNode){
                Actor::set(this->sceNode, nullptr);
                Node * pNode = this->sceNode->getParent();
                pNode->removeChild(this->sceNode);
                this->sceNode = nullptr;
            }            
        }

        Point2<float> getPosition(Transform::D3_NORMAL_D2 & d3_normal_d2){
            Vector3 positionIn3D = this->getSceneNode()->getPosition();
            return Point2<float>::from(positionIn3D, d3_normal_d2);
        }

        void setPosition(CellKey cKey,Transform::D2H2D3 &d2h2d3){
            Vector3 v3 = cKey.transform3(d2h2d3);
            this->getSceneNode()->setPosition(v3);
        }
        
        Actor * getParent(){
            return this->parent;
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
        
        // virtual void init() {

        // };

        virtual void collectProperties()
        {
        }

        // Vector3 to3D(Vector2 point, float scale)
        // {
        //     return Context<Node2D>::get()->to3D(point, scale);
            
        // }

        // bool findCell(Vector2 aPos3, HexTile::Key &cell)
        // {
        //     return Context<Cell::Center>::get()->findCellByPosition(aPos3, cell);
        // }

        SceneNode *getSceneNode()
        {
            return this->sceNode;
        }

        SceneNode *findSceneNode()
        {
            Actor *s = this;
            while (s)
            {
                if (s->sceNode)
                {
                    return s->sceNode;
                }
                s = s->parent;
            }
            return nullptr;
        }
        void setSceneNode(SceneNode *sNode)
        {
            Actor::set(sNode, this);
            this->sceNode = sNode;
        }

        // void addChild(Actor *s)
        // {
        //     if (s->parent)
        //     {
        //         throw std::runtime_error("Already has a parent state.");
        //     }
        //     children.push_back(UniquePtr<Actor>(s));
        //     s->parent = this;
        // }

        // void removeChild(Actor *cs)
        // {
        //     children.erase(
        //         std::remove_if(children.begin(), children.end(), [cs](const UniquePtr<Actor> &state)
        //                        { return cs == state.get(); }),
        //         children.end());
        // }

        FrameListener *getFrameListener()
        {
            return this->frameListener;
        }

        void setFrameListener(FrameListener *listener)
        {
            this->frameListener = listener;
        }

        void setActive(bool active)
        {
            // bool changed = (this->active != active);
            this->active = active;
            // if (changed)
            // {

            //     // Context<ActorPropEC>::get()->emit(this, std::string("active"));
            //     eventBus->emit<State *, std::string &>(this, std::string("active"));
            // }
        }

        bool isActive()
        {
            return this->active;
        }
        template <typename F>
        bool forEach(F &&func)
        {
            bool goOn = func(this);
            if (goOn)
            {
                return forEachChild(func);
            }
            return false;
        }

        // template <typename... Args>
        // void forEachChild(void (*func)(State *, Args...), Args... args)
        // {
        //     forEachChild<Args...>(true, func, args...);
        // }

        template <typename F>
        bool forEachChild(F &&func, bool recursive = true)
        {

            bool goOn = true;
            for (auto &it = children.begin(); it != children.end(); ++it)
            {
                Actor *s = it->get();
                goOn = func(s);
                if (goOn && recursive)
                {
                    goOn = s->forEachChild(func);
                }
                if (!goOn)
                {
                    break;
                }
            }
            return goOn;
        }

        template <typename... Args>
        void forEachChild(bool recursive, void (*func)(Actor *, Args...), Args... args)
        {
            std::vector<Actor *> *tmp = this->children;
            for (auto it = tmp->begin(); it != tmp->end(); ++it)
            {
                Actor *s = *it;
                func(s, args...);
                if (recursive)
                {
                    s->forEachChild<Args...>(true, func, args...);
                }
            }
        }
        void setName(std::string name)
        {
            this->name = name;
        }
        Actor *getChildByName(std::string name)
        {
            Actor *ret = nullptr;
            forEachChild([&name, &ret](Actor *s)
                         {
                if(s->name == name){
                    ret = s;
                    return false;
                }
                return true; }, false);
            return ret;
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
