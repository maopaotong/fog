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
        CellKey::Offset cKey2;
        CellKey::Offset cKey1;
        CostMap *costMap;
        CellInstanceStateManager *cellInstMgrState;
        MovableStateManager *movableStateMgr;
        EventBus *eventBus;
        CellsCost *cellsCost;
        Config *config;
        CoreMod *core;
        Transforms * tfs;

    public:
        INJECT(PathingStateManager(CostMap *costMap, 
                                   EventBus *eventBus,
                                   Config *config,
                                   CoreMod *core,
                                   CellInstanceStateManager *cellInstMgrState,
                                   MovableStateManager *movableStateMgr,
                                   Transforms * tfs,
                                   CellsCost *cellsCost)) : core(core),
                                                            config(config),
                                                            eventBus(eventBus),
                                                            costMap(costMap), sourceState(nullptr), targetCis(nullptr),
                                                            cellInstMgrState(cellInstMgrState),
                                                            movableStateMgr(movableStateMgr),
                                                            cellsCost(cellsCost),
                                                            path(nullptr),
                                                            tfs(tfs)

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
                //this->targetCis->popColour();
            }
            this->targetCis = cis;
            if (this->targetCis)
            {
                //this->targetCis->pushColour(ColourValue::White);
            }
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
                                             sourceCis = this->cellInstMgrState->getCellInstanceStateByPosition(state->getPosition(*tfs->d3d2));
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
            CellKey::Offset cKey2 = targetCis->getCellKey();
            CellKey::Offset cKey1 = sourceCis->getCellKey();
            if (this->cKey2 == cKey2 && this->cKey1 == cKey1)
            {                // do nothing.
                return true; // GOON
            }
            // update path
            this->cKey1 = cKey1;
            this->cKey2 = cKey2;
            auto costFunc = *cellsCost;
            std::vector<CellKey::Offset> pathByCellKey =
                costMap->findPath(cKey1, cKey2, costFunc);

            PathState *pathState2 = new PathState(cellInstMgrState);
            // pathState2->init();
            pathState2->setPath(pathByCellKey);
            this->setPath(pathState2);
            return true;
        }

    }; //
};
