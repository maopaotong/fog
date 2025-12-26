/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/ogre.h"

#include "fg/util.h"
#include "fg/cells.h"
// include the files under same folder
#include "CellInstanceStateManager.h"
#include "MoveToCellTask.h"
#include "MovableStateManager.h"
namespace fog
{

    class MovingStateManager : public Manager<Actor>, public Stairs
    {
        std::vector<std::unique_ptr<MoveToCellTask>> tasks;
        Actor *state;
        CostMap *costMap;
        EventBus *eventBus;
        CellsCost *cellsCost;
        Config *config;
        CoreMod *core;
        Transforms *tfs;
        Geometry *geo;

    public:
        INJECT(MovingStateManager(CostMap *cm,
                                  EventBus *eventBus,
                                  CoreMod *core,
                                  Config *config,
                                  Transforms *tfs,
                                  Geometry *geo,
                                  CellsCost *cellsCost)) : core(core),
                                                           geo(geo),
                                                           tfs(tfs),
                                                           config(config),
                                                           eventBus(eventBus),
                                                           cellsCost(cellsCost),
                                                           costMap(cm), state(nullptr)
        {
            eventBus-> //
                subscribe<MovableEventType, Actor *>([this](MovableEventType evtType, Actor *state)
                                                     {
                                                         if (evtType == MovableEventType::StatePicked)
                                                         {
                                                             this->state = state;
                                                         }
                                                         else if (evtType == MovableEventType::StateUnpicked)
                                                         {
                                                             this->state = nullptr;
                                                         }
                                                         return true; //
                                                     });
        }
        virtual ~MovingStateManager()
        {
        }
        // 禁止拷贝
        MovingStateManager(const MovingStateManager &) = delete;
        MovingStateManager &operator=(const MovingStateManager &) = delete;

        // TODO move this function to MouseStateManager
        bool movingActiveStateToCellByMousePosition(int mx, int my)
        {
            // // normalized (0,1)
            // Viewport *viewport = Context<CoreMod>::get()->getViewport();
            // Camera *camera = Context<CoreMod>::get()->getCamera();
            Box2<float> vp = core->getActualViewportBox();
            float ndcX = mx / (float)vp.getWidth();
            float ndcY = my / (float)vp.getHeight();

            Ogre::Ray ray = core->getCameraToViewportRay(ndcX, ndcY);

            auto hitGrd = ray.intersects(geo->ground);
            std::cout << "ndc:(" << ndcX << "," << ndcY << ")" << "hit:" << hitGrd.first << std::endl;
            if (!hitGrd.first)
            {
                return false;
            }
            Vector3 pos3D = ray.getPoint(hitGrd.second);

            Point2<float> pos = Point2<float>::from(pos3D, *tfs->d3d2);

            // bool hitCell = CellUtil::findCellByPoint(costMap, Vector2(pos.x, pos.z), cKey);
            // bool hitCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(pos), cKey);
            // HexTile::Key cell2;
            // bool hitCell = Context<Cell::Center>::get()->findCellByPosition(pos, cell2);
            // if (!hitCell)
            // {
            //     return false;
            // }
            CellKey::Offset cKey2 = CellKey::transform<CellKey::CO>(pos);

            eventBus->emit<CellEventType, CellKey::Offset>(CellEventType::CellAsTarget, cKey2);

            // state
            this->movingActiveStateToCell(cKey2);
            // cout << "worldPoint(" << pickX << ",0," << pickZ << "),cellIdx:[" << cx << "," << cy << "]" << endl;
            return true;
        }

        void movingActiveStateToCell(CellKey::Offset cKey2)
        {
            if (this->state == nullptr)
            {
                return;
            }

            // clear old task on the same state
            for (auto &it = this->tasks.begin(); it != tasks.end();)
            {
                MoveToCellTask *task = it->get();
                if (task->getState() == state)
                {
                    it = tasks.erase(it);
                }
                else
                {
                    it++;
                }
            }

            //
            MoveToCellTask *task = new MoveToCellTask(state, cKey2, costMap, eventBus, cellsCost, config, tfs);
            this->tasks.push_back(std::unique_ptr<MoveToCellTask>(task));
            eventBus->emit<MovableEventType, Actor *>(MovableEventType::StateStartMoving, state);
        }
        template <typename F>
        void forEachTask(F &&f)
        {
            for (auto &task : this->tasks)
            {
                if (!f(task.get()))
                {
                    break;
                }
            }
        }

        GOON step(float time) override
        {

            for (auto it = this->tasks.begin(); it != this->tasks.end();)
            {
                MoveToCellTask *task = it->get();
                if (!task->step(time))
                {
                    eventBus->emit<MovableEventType, Actor *>(MovableEventType::StateStopMoving, task->getState());
                    it = this->tasks.erase(it);
                    continue;
                }

                it++;
            }
            return true;
        }

    }; // end of class
}; // end of namespace
