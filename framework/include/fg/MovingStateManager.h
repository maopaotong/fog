/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/Common.h"
#include "fg/Actor.h"
#include "fg/core/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/CellInstanceStateManager.h"
#include "fg/core/MoveToCellTask.h"
#include "fg/MovableStateManager.h"
#include "fg/Config.h"
#include "fg/util.h"

#include "fg/cells.h"
namespace fog
{

    class MoveToCellTask : public Stairs
    {
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};

        float actorHighOffset;

    protected:
        CellKey cKey2; // target cell key.
        Actor *movingState;
        //
        PathFollow2MissionState *mission = nullptr;
        bool failed = false;

        //
        Vector3 prePosition;

        CostMap *costMap;
        Event::Bus *eventBus;
        CellsCost *cellsCost;
        Config *config;

    public:
        MoveToCellTask(Actor *state, CellKey cKey2,
                       CostMap *costMap,
                       Event::Bus *eventBus,
                       CellsCost *cellsCost,
                       Config *config) : costMap(costMap),
                                         eventBus(eventBus),
                                         cellsCost(cellsCost),
                                         movingState(state), cKey2(cKey2), config(config)
        {
        }
        virtual ~MoveToCellTask()
        {
        }

        Actor *getState()
        {
            return this->movingState;
        }
        CellKey getTargetCellKey()
        {
            return this->cKey2;
        }

        bool step(float time) override
        {
            if (!this->mission)
            {
                buildMission(nullptr);
            }
            bool ret = mission->step(time);
            Vector3 pos = this->movingState->getSceneNode()->getPosition();
            if (pos.distance(this->prePosition) > config->STATE_MOVED_EVENT_DISTNACE)
            {
                eventBus->emit(MovableEventType::StateMoved, this->movingState);
                this->prePosition = pos;
            }
            return ret;
        }

        std::tuple<CellKey, Point2<float>> resolveOwnerCell()
        {
            // Cell::Center *cells = Context<Cell::Center>::get();

            // check if this state's position on the target cell
            Point2<float> actorPosIn2D = this->movingState->getPosition();
            // Node2D *root2D = cells->getRoot2D();
            // Vector2 actorPosIn2D = root2D->to2D(aPos3);

            // Vector2 actorPosIn2D = Point2<float>::from(aPos3, Transform::D3_NORMAL_D2(config->D2_NORMAL_D3));

            // HexTile::Key cell;
            // // bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aHexTile::Key);
            // bool hitCell = Context<Cell::Center>::get()->findCellByPosition(actorPosIn2D, cell);
            CellKey cell = CellKey::from(actorPosIn2D);

            // todo: not hit?
            return {cell, actorPosIn2D};
        }

        PathFollow2 buildPath()
        {

            std::tuple<CellKey, Point2<float>> stateCellAndPosition = this->resolveOwnerCell();

            CellKey cKey1 = std::get<CellKey>(stateCellAndPosition);

            CellsCost &costFunc = *cellsCost;
            std::vector<CellKey> pathByCKey = costMap-> //
                                              findPath(cKey1,
                                                       cKey2, //
                                                       costFunc);

            if (pathByCKey.empty())
            {
                // find the nearest cell as the target.
                // TODO: if the targe is untouchable area, then find the nearest border cell of such area as the new target.
                std::vector<CellKey> linePath = costMap-> //
                                                findPath(
                                                    cKey2, // reverse simple line path
                                                    cKey1, //
                                                    [](Point2<int> cKey) -> int
                                                    {
                                                        return 1;
                                                    });

                for (auto it = linePath.begin(); it != linePath.end(); it++)
                { // try
                    cKey2 = *it;
                    pathByCKey = costMap->findPath(cKey1, cKey2, costFunc);

                    if (!pathByCKey.empty())
                    {
                        break;
                    }
                }
                //
            }

            // CellUtil::translatePathToCellCenter(pathByKey, pathByPosition, CellUtil::offset(costMap));

            // Context<Node2D>::get()->

            std::vector<Point2<float>> centres;
            CellKey::getCentres(pathByCKey, centres);
            // float pathSpeed = this->Context<Var<float>::Bag>::get()->getVarVal(".pathSpeed", 1.0f);

            float pathSpeed = Context<Var<float>::Bag>::get()->getVarVal(".pathSpeed", 1.0f);
            PathFollow2 path(std::get<Point2<float>>(stateCellAndPosition), centres, pathSpeed);
            // PathState *pathState = new PathState();
            // pathState->init();
            // pathState->setPath(pathByPoint2DNom, std::get<HexTile::Key>(keyAndPosition), cKey2);

            // TODO add pathState to another common state.
            // this->addChild(pathState); //

            return path;
        }

        void buildMission(MoveToCellTask *preTask)
        {

            PathFollow2 path2D = buildPath();

            AnimationStateSet *anisSet = movingState->getAllAnimationStates();
            if (!anisSet)
            {
                throw std::runtime_error("no animation set, not supported for now.");
            }
            // float aniSpeed = this->Context<Var<float>::Bag>::get()->getVarVal(".aniSpeed", 1.0f);
            float aniSpeed = Context<Var<float>::Bag>::get()->getVarVal(".aniSpeed", 1.0f);

            // new child state.
            mission = new PathFollow2MissionState(this->movingState, path2D, anisSet, movingState->getAnimationNames(), aniSpeed, config, movingState->getActorHighOffset()); //
            mission->init();
            // delete missionState;
            // this->addChild(missionState);
            // this->addChild(mission);
        }

    }; // end of class

    class MovingStateManager : public Manager<Actor>, public Stairs
    {
        std::vector<std::unique_ptr<MoveToCellTask>> tasks;
        Actor *state;
        CostMap *costMap;
        Viewport *viewport;
        Camera *camera;
        Event::Bus *eventBus;
        CellsCost *cellsCost;
        Config *config;

    public:
        INJECT(MovingStateManager(CostMap *cm, Viewport *viewport,
                                  Event::Bus *eventBus,
                                  Camera *camera,
                                  Config *config,
                                  CellsCost *cellsCost)) : viewport(viewport),
                                                           config(config),
                                                           eventBus(eventBus),
                                                           camera(camera),
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

            float ndcX = mx / (float)viewport->getActualWidth();
            float ndcY = my / (float)viewport->getActualHeight();

            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0
            auto hitGrd = ray.intersects(ground);
            std::cout << "ndc:(" << ndcX << "," << ndcY << ")" << "hit:" << hitGrd.first << std::endl;
            if (!hitGrd.first)
            {
                return false;
            }
            Ogre::Vector3 pos3D = ray.getPoint(hitGrd.second);

            Point2<float> pos = Point2<float>::from(pos3D, *Context<Transform::D3_NORMAL_D2>::get());

            // bool hitCell = CellUtil::findCellByPoint(costMap, Vector2(pos.x, pos.z), cKey);
            // bool hitCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(pos), cKey);
            // HexTile::Key cell2;
            // bool hitCell = Context<Cell::Center>::get()->findCellByPosition(pos, cell2);
            // if (!hitCell)
            // {
            //     return false;
            // }
            CellKey cKey2 = CellKey::from(pos);

            eventBus->emit<CellEventType, CellKey>(CellEventType::CellAsTarget, cKey2);

            // state
            this->movingActiveStateToCell(cKey2);
            // cout << "worldPoint(" << pickX << ",0," << pickZ << "),cellIdx:[" << cx << "," << cy << "]" << endl;
            return true;
        }

        void movingActiveStateToCell(CellKey cKey2)
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
            MoveToCellTask *task = new MoveToCellTask(state, cKey2, costMap, eventBus, cellsCost, config);
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
