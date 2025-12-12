/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/State.h"
#include "CellStateControl.h"
#include "fg/CameraState.h"
#include "fg/core/EntityState.h"
#include "fg/MovingStateManager.h"
#include "fg/CellInstanceManager.h"
#include "fg/demo/EntryController.h"
#include "fg/MovableStateManager.h"
#include "fg/BuildingStateManager.h"
#include "fg/InventoryStateManager.h"   
#include "fg/core/TilesState.h"
namespace fog
{
    class WorldState : public State
    {
    protected:
        //CellStateControl *cells;
        TilesState* tiles;


    public:
        WorldState()
        {
            CostMap *costMap = Context<CostMap>::get();
            CoreMod *core = Context<CoreMod>::get();
            Ogre::Root *root = core->getRoot();

            // Create frame listener for main loop
            this->tiles = new TilesState();
            this->tiles->init();
            
            this->addChild(this->tiles);
            //
            MovableStateManager *movableStateMgr = Context<MovableStateManager >::get();
            movableStateMgr->init();
            this->addChild(movableStateMgr);
            //Context<MovableStateManager >::set(movableStateMgr);
            //           
            BuildingStateManager *buildingStateMgr = Context<BuildingStateManager >::get();
            buildingStateMgr->init();
            this->addChild(buildingStateMgr);
            //

            InventoryStateManager *inventoryStateMgr = Context<InventoryStateManager >::get();
            inventoryStateMgr->init();
            this->addChild(inventoryStateMgr);
            //

            EntryController *entryController = new EntryController();
            core->getAppContext()->addInputListener(entryController);
            root->addFrameListener(entryController);

        }


        virtual void init() override
        {
        }
    };
};
