/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
#include "fg/core/Sinbad.h"
#include "fg/core/Tower.h"

namespace fog
{
    class MovingState : public State, public Stairs
    {
    public:
        State *state;
        CellInstanceState *cis;
        MovingState() : state(nullptr), cis(nullptr)
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
        void setState(State *state2)
        {
            if (this->state)
            {
                trySetCis(nullptr);
            }
            this->state = state2;
            tryUpdateCis();
            Context<Event::Bus>::get()-> //
                emit<MovableEventType, State *>(state2 ? MovableEventType::StatePicked : MovableEventType::StateUnpicked, state2);
        }
        void tryUpdateCis()
        {
            if (this->state)
            {
                CellInstanceManager *cisManager = Context<CellInstanceManager>::get();

                CellInstanceState *cis2 = cisManager->getCellInstanceStateByPosition(state->getPosition());
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

    class MovableStateManager : public State, public Stairs
    {
        MovingState movingState;
        EntityState *actor2;
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};

    public:
        MovableStateManager() : actor2(new Sinbad())
        {
            Context<Event::Bus>::get()-> //
                subscribe<MovableEventType, State *>([this](MovableEventType evtType, State *state)
                                                     {
                                                         if (evtType == MovableEventType::StateStartMoving)
                                                         {
                                                             this->movingState.setState(nullptr);
                                                         }
                                                         return true; //
                                                     });
        }
        virtual ~MovableStateManager()
        {
        }
        void init() override
        {
            this->createSinbad();
        }

        void createSinbad()
        {

            actor2->init();
            this->addChild(actor2);

            // find a position.
        }

        void setCellToStandOn(HexTile::Key cKey)
        {
            actor2->setPosition(cKey);
        }

        bool step(float time) override
        {
            movingState.step(time);
            return true;
        }

        bool pick(Ray &ray)
        {

            // 创建射线查询对象
            Ogre::RaySceneQuery *rayQuery = Context<CoreMod>::get()->getSceneManager()->createRayQuery(ray);
            rayQuery->setSortByDistance(true);  // 按距离排序（最近的优先）
            rayQuery->setQueryMask(0x00000001); // 与 Entity 的查询掩码匹配

            // 执行查询
            Ogre::RaySceneQueryResult &result = rayQuery->execute();

            State *picked = nullptr;
            // 遍历结果
            for (auto &it : result)
            {
                Node *node = it.movable->getParentNode();
                State *s = State::get(node);
                if (s && s->pickable() && s->getParent() == this)
                {

                    picked = s;
                    break;
                }
            }
            Context<CoreMod>::get()->getSceneManager()->destroyQuery(rayQuery);
            this->movingState.setState(picked);
            return picked != nullptr;
        }

    }; // end of class
}; // end of namespace
