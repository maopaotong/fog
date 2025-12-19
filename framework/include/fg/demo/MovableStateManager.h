/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/core.h"
#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/cells.h"
#include "Sinbad.h"
#include "Tower.h"
#include "HomeCellKey.h"

namespace fog
{
    class MovingState : public Stairs
    {
    public:
        Actor *state;
        CellInstanceState *cis;
        CellInstanceStateManager *cisManager;
        Event::Bus *eventBus;
        Config *config;
        MovingState(CellInstanceStateManager *cisManager,
                    Event::Bus *eventBus,
                    Config *config) : state(nullptr), cis(nullptr),
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

                CellInstanceState *cis2 = cisManager->getCellInstanceStateByPosition(state->getPosition(*config->transformD3NormalToD2Ptr));
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
                this->cis->popColour();
            }
            this->cis = cis2;
            if (this->cis)
            {
                this->cis->pushColour(ColourValue::White);
            }
        }
    };

    class MovableStateManager : public Manager<Actor>, public Stairs
    {
        MovingState movingState;
        EntityState *actor2;
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};
        CoreMod *core;
        Event::Bus *eventBus;
        HomeCellKey *home;
        Config *config;

    public:
        INJECT(MovableStateManager(CoreMod *core, CellInstanceStateManager *cisManager,
                                   Event::Bus *eventBus,
                                   HomeCellKey *homeCellKey,
                                   Config *config,
                                   Component::Injector * injector
                                   ))
            : core(core),
              config(config),
              eventBus(eventBus),
              home(homeCellKey),
              movingState{cisManager, eventBus, config}
        {
            eventBus-> //
                subscribe<MovableEventType, Actor *>([this](MovableEventType evtType, Actor *state)
                                                     {
                                                         if (evtType == MovableEventType::StateStartMoving)
                                                         {
                                                             this->movingState.setState(nullptr);
                                                         }
                                                         return true; //
                                                     });
        this->actor2= injector->getPtr<Sinbad>();                                                     
            this->createSinbad();
        }
        virtual ~MovableStateManager()
        {
        }

        void createSinbad()
        {
            actor2->init();
            this->add(actor2);
            // actor2->setPosition(home->cKey );
            //  find a position.
            actor2->setPosition(home->cKey, *config->transformFromD2HToD3Ptr);
        }

        void setCellToStandOn(CellKey cKey)
        {
            // actor2->setPosition(cKey);
        }

        bool step(float time) override
        {
            movingState.step(time);
            return true;
        }

        bool pick(Ray &ray)
        {

            // 创建射线查询对象
            Ogre::RaySceneQuery *rayQuery = core->createRayQuery(ray);
            rayQuery->setSortByDistance(true);  // 按距离排序（最近的优先）
            rayQuery->setQueryMask(0x00000001); // 与 Entity 的查询掩码匹配

            // 执行查询
            Ogre::RaySceneQueryResult &result = rayQuery->execute();

            Actor *picked = nullptr;
            // 遍历结果
            for (auto &it : result)
            {
                Node *node = it.movable->getParentNode();
                Actor *s = Actor::get(node);
                // if (s && s->pickable() && s->getParent() == this)
                if (s && s->pickable() && s == actor2)
                {

                    picked = s;
                    break;
                }
            }
            // Context<CoreMod>::get()
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
