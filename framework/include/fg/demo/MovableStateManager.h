/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/cells.h"
#include "fg/actors.h"
#include "HomeCellKey.h"

namespace fog
{
    class MovingState : public Stairs
    {
    public:
        Actor *state;
        CellInstanceState *cis;
        CellInstanceStateManager *cisManager;
        EventBus *eventBus;
        Config *config;
        Transforms *tfs;
        MovingState(CellInstanceStateManager *cisManager,
                    EventBus *eventBus,
                    Config *config,
                    Transforms *tfs) : state(nullptr), cis(nullptr),
                                       tfs(tfs),
                                       eventBus(eventBus),
                                       config(config),
                                       cisManager(cisManager)

        {
        }

        ~MovingState()
        {
            setState(nullptr);
        }

        GOON step(float time) override
        {
            tryUpdateCis();
            return true;
        }
        void setState(Actor *state2)
        {
            if (this->state)
            {
                trySetCis(nullptr);
            }
            this->state = state2;
            tryUpdateCis();
            eventBus-> //
                emit<MovableEventType, Actor *>(state2 ? MovableEventType::StatePicked : MovableEventType::StateUnpicked, state2);
        }
        void tryUpdateCis()
        {
            if (this->state)
            {

                CellInstanceState *cis2 = cisManager->getCellInstanceStateByPosition(state->getPosition(*tfs->d3d2));
                this->trySetCis(cis2);
            }
            else
            {
                trySetCis(nullptr);
            }
        }
        void trySetCis(CellInstanceState *cis2)
        {
            if (this->cis == cis2)
            {
                return;
            }
            if (this->cis)
            {
                // this->cis->popColour();
            }
            this->cis = cis2;
            if (this->cis)
            {
                // this->cis->pushColour(ColourValue::White);
            }
        }
    };

    class MovableStateManager : public Manager<Actor>, public Stairs
    {
        MovingState movingState;
        Sphere *actor2;
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};
        CoreMod *core;
        EventBus *eventBus;
        HomeCellKey *home;
        Config *config;
        Transforms *tfs;

    public:
        INJECT(MovableStateManager(CoreMod *core, CellInstanceStateManager *cisManager,
                                   EventBus *eventBus,
                                   HomeCellKey *homeCellKey,
                                   Config *config,
                                   Transforms *tfs,
                                   Injector *injector))
            : core(core),
              tfs(tfs),
              config(config),
              eventBus(eventBus),
              home(homeCellKey),
              movingState{cisManager, eventBus, config, tfs}
        {
            eventBus-> //
                subscribe<MovableEventType, Actor *>([this](MovableEventType evtType, Actor *state)
                                                     {
                                                         if (evtType == MovableEventType::StateStartMoving)
                                                         {
                                                             // this->movingState.setState(nullptr);
                                                         }
                                                         return true; //
                                                     });
            this->actor2 = new Sphere(core, core->getRootSceneNode()->createChildSceneNode());
            this->add(actor2);
            Vector3 pos3 = tfs->transform3(home->cKey);
            actor2->setPosition(pos3);
            //actor2->setPosition(home->cKey, *tfs->d2td3);
        
        }
        virtual ~MovableStateManager()
        {
        }

        bool step(float time) override
        {
            movingState.step(time);
            return true;
        }

        bool pick(Ogre::Ray &ray)
        {

            // 
            Ogre::RaySceneQuery *rayQuery = core->createRayQuery(ray);
            rayQuery->setSortByDistance(true);  // 
            rayQuery->setQueryMask(0x00000001); // 

            // 
            Ogre::RaySceneQueryResult &result = rayQuery->execute();

            Actor *picked = nullptr;
            // 
            for (auto &it : result)
            {
                Ogre::Node *node = it.movable->getParentNode();
                Actor *s = Actor::get(node);
                if (s && s->pickable() && s == actor2)
                {

                    picked = s;
                    break;
                }
            }
            //
            core->destroyQuery(rayQuery);
            this->movingState.setState(picked);
            return picked != nullptr;
        }

        void pick(Actor *picked)
        {
            this->movingState.setState(picked);
        }

    }; // end of class
}; // end of namespace
