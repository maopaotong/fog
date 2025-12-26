/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "CellInstanceStateManager.h"
#include "MovableStateManager.h"

#include "fg/cells.h"
namespace fog
{

    class MoveToCellTask : public Stairs
    {
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};

        float actorHighOffset;

    protected:
        CellKey::Offset cKey2; // target cell key.
        Actor *movingState;
        //
        PathFollow2MissionState *mission = nullptr;
        bool failed = false;

        //
        Vector3 prePosition;

        CostMap *costMap;
        EventBus *eventBus;
        CellsCost *cellsCost;
        Config *config;
        Transforms * tfs;

    public:
        MoveToCellTask(Actor *state, CellKey::Offset cKey2,
                       CostMap *costMap,
                       EventBus *eventBus,
                       CellsCost *cellsCost,

                       Config *config,
                       Transforms * tfs) : costMap(costMap),
                                                 tfs(tfs),
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
        CellKey::Offset getTargetCellKey()
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
            if (pos.distance(this->prePosition) > config->actorMovedEventDistance)
            {
                eventBus->emit(MovableEventType::StateMoved, this->movingState);
                this->prePosition = pos;
            }
            return ret;
        }

        std::tuple<CellKey::Offset, Point2<float>> resolveOwnerCell()
        {
            // Cell::Center *cells = Context<Cell::Center>::get();

            // check if this state's position on the target cell
            Point2<float> actorPosIn2D = this->movingState->getPosition(*tfs->d3d2);
            // Node2D *root2D = cells->getRoot2D();
            // Vector2 actorPosIn2D = root2D->to2D(aPos3);

            // Vector2 actorPosIn2D = Point2<float>::from(aPos3, Transform::D3_NORMAL_D2(config->D2_NORMAL_D3));

            // HexTile::Key cell;
            // // bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aHexTile::Key);
            // bool hitCell = Context<Cell::Center>::get()->findCellByPosition(actorPosIn2D, cell);
            CellKey::Offset cell = CellKey::transform<CellKey::CO>(actorPosIn2D);

            // todo: not hit?
            return {cell, actorPosIn2D};
        }

        PathFollow2 buildPath()
        {

            std::tuple<CellKey::Offset, Point2<float>> stateCellAndPosition = this->resolveOwnerCell();

            CellKey::Offset cKey1 = std::get<CellKey::Offset>(stateCellAndPosition);

            CellsCost &costFunc = *cellsCost;
            std::vector<CellKey::Offset> pathByCKey = costMap-> //
                                              findPath(cKey1,
                                                       cKey2, //
                                                       costFunc);

            if (pathByCKey.empty())
            {
                // find the nearest cell as the target.
                // TODO: if the targe is untouchable area, then find the nearest border cell of such area as the new target.
                std::vector<CellKey::Offset> linePath = costMap-> //
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

            //std::vector<Point2<float>> centres;
            //CellKey::transform<CellKey::OC>s<CellKey::Offset>(pathByCKey, centres);
            std::vector<Point2<float>> centres = CellKey::transformAll<CellKey::OCP>(pathByCKey);

            // float pathSpeed = this->Context<Var<float>::Bag>::get()->getVarVal(".pathSpeed", 1.0f);

            float pathSpeed = 1.0f; // Context<Var<float>::Bag>::get()->getVarVal(".pathSpeed", 1.0f);
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

            Ogre::AnimationStateSet *anisSet = movingState->getAllAnimationStates();
            
            float aniSpeed = 1.0f; // 

            // new child state.
            mission = new PathFollow2MissionState(this->movingState, path2D, anisSet, movingState->getAnimationNames(), aniSpeed, tfs, movingState->getActorHighOffset()); //
            
        }

    }; // end of class

}; // end of namespace
