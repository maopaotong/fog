/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Common.h"

#include <typeindex>

#include <memory>
#include <type_traits>
#include <utility>
#include <Ogre.h>
#include "fg/util.h"
#include "OgreSceneNode.h"
#include "fg/util/CostMap.h"
#include "PathState.h"
#include "PathFollow2MissionState.h"
#include "fg/util.h"
#include "fg/cells.h"
#include "CellInstanceStateManager.h"

namespace fog
{
    /**
     * PathingStateManager find a path from current state position to mouse position.
     */
    class PathingStateManager : public Stairs
    {
        //
        Actor *sourceState;
        CellInstanceState *targetCis;
        PathState *path;

    protected:
        CellKey cKey2;
        CellKey cKey1;
        CostMap *costMap;
        Viewport *viewport;
        Camera *camera;
        CellInstanceStateManager *cellInstMgrState;
        MovableStateManager *movableStateMgr;
        Event::Bus *eventBus;
        CellsCost *cellsCost;
        Config *config;

    public:
        INJECT(PathingStateManager(CostMap *costMap, Viewport *viewport,
                                   Event::Bus *eventBus,
                                   Config *config,
                                   CellInstanceStateManager *cellInstMgrState,
                                   MovableStateManager *movableStateMgr,
                                   Camera *camera,
                                   CellsCost *cellsCost)) : viewport(viewport),
                                                            camera(camera),
                                                            config(config),
                                                            eventBus(eventBus),
                                                            costMap(costMap), sourceState(nullptr), targetCis(nullptr),
                                                            cellInstMgrState(cellInstMgrState),
                                                            movableStateMgr(movableStateMgr),
                                                            cellsCost(cellsCost),
                                                            path(nullptr)

        {
            std::cout << "PathingStateManager created." << std::endl;
            eventBus-> //
                subscribe<MovableEventType, Actor *>([this](MovableEventType evtType, Actor *state)
                                                     {
                                                         if (evtType == MovableEventType::StateUnpicked)
                                                         {
                                                             this->setSource(nullptr);
                                                         }
                                                         else if (evtType == MovableEventType::StatePicked)
                                                         {
                                                             this->setSource(state);
                                                         }
                                                         else if (evtType == MovableEventType::StateStartMoving)
                                                         {
                                                             this->setSource(nullptr);
                                                         }
                                                         return true; //
                                                     });
        }
        virtual ~PathingStateManager()
        {
            std::cout << "~PathingStateManager destroyed." << std::endl;
        }

        void setPath(PathState *path2)
        {
            // if (this->path)
            // {
            //     this->removeChild(this->path);
            // }
            this->path = path2;
            // if (this->path)
            // {
            //     this->addChild(path);
            // }
        }

        void setSource(Actor *state)
        {
            if (this->sourceState)
            {
                this->setPath(nullptr);
                this->setTargetCis(nullptr);
                this->sourceState = nullptr;
            }
            this->sourceState = state;
        }
        void setTargetCis(CellInstanceState *cis)
        {
            if (this->targetCis)
            {
                this->targetCis->popColour();
            }
            this->targetCis = cis;
            if (this->targetCis)
            {
                this->targetCis->pushColour(ColourValue::White);
            }
        }

        // TODO move this function to MouseStateManager
        CONSUMED onMouseMoved(int x, int y)
        {
            if (!this->sourceState)
            {
                return false;
            }

            // Viewport *viewport = Context<CoreMod>::get()->getViewport();
            // Camera *camera = Context<CoreMod>::get()->getCamera();

            float ndcX = x / (float)viewport->getActualWidth();
            float ndcY = y / (float)viewport->getActualHeight();

            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0

            auto hitGrd = ray.intersects(ground);

            Ogre::Vector3 pos2;

            if (!hitGrd.first)
            {
                return false;
            }

            pos2 = ray.getPoint(hitGrd.second);

            // Point2<float> p2 = Point2<float>::from(pos2, Transform::D3_NORMAL_D2(config->D2H2D3));
            Point2<float> p2 = Point2<float>::from(pos2, *config->transformD3NormalToD2);

            CellInstanceState *cis = cellInstMgrState->getCellInstanceStateByPosition(p2);
            if (!cis)
            {
                return false;
            }
            if (cis == targetCis)
            {
                return false;
            }
            if (targetCis)
            {
                targetCis->popColour();
                eventBus-> //
                    emit<MouseCellEventType, CellKey>(MouseCellEventType::MouseLeaveCell, targetCis->getCellKey());
            }
            cis->pushColour(ColourValue::White);
            targetCis = cis;
            eventBus-> //
                emit<MouseCellEventType, CellKey>(MouseCellEventType::MouseEnterCell, targetCis->getCellKey());

            return false;
        }

        GOON step(float time) override
        {
            if (!sourceState)
            {
                return true;
            }
            if (!targetCis)
            {
                return true;
            }

            CellInstanceState *sourceCis = nullptr;
            // find source cell
            // MovableStateManager *movableStateMgr = Context<MovableStateManager>::get();
            movableStateMgr->forEach([&sourceCis, this](Actor *state)
                                     {
                                         if (state->isActive())
                                         {
                                             //Context<CellInstanceManager>::get()
                                             sourceCis = this->cellInstMgrState->getCellInstanceStateByPosition(state->getPosition(*config->transformD3NormalToD2));
                                             if (sourceCis)
                                             {
                                                 return false; // break
                                             }
                                         } //
                                         return true; });
            if (!sourceCis)
            {
                return true;
            }
            //
            CellKey cKey2 = targetCis->getCellKey();
            CellKey cKey1 = sourceCis->getCellKey();
            if (this->cKey2 == cKey2 && this->cKey1 == cKey1)
            {                // do nothing.
                return true; // GOON
            }
            // update path
            this->cKey1 = cKey1;
            this->cKey2 = cKey2;
            auto costFunc = *cellsCost;
            std::vector<CellKey> pathByCellKey =
                costMap->findPath(cKey1, cKey2, costFunc);

            PathState *pathState2 = new PathState(cellInstMgrState);
            //pathState2->init();
            pathState2->setPath(pathByCellKey);
            this->setPath(pathState2);
            return true;
        }

    }; //
};
