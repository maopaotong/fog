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
#include "fg/cells.h"
#include "fg/util.h"
#include "fg/core/FogOfWar.h"
#include "fg/demo/CellsDatas.h"
#include "fg/demo/HomeCellKey.h"
namespace fog
{

    class WorldState : public State
    {
    protected:
        // CellStateControl *cells;
        CellsState *tilesState;
        TheTerrains *tts;
        // std::vector<std::vector<CellData>> tiles;
        CellsDatas *cellsDatas;
        FogOfWar *fogOfWar;
        EntryController *entryController;
        CellsTerrains *terrains;
        CoreMod *core;
        MovableStateManager *movableStateMgr;
        BuildingStateManager *buildingStateMgr;
        InventoryStateManager *inventoryStateMgr;
        HomeCellKey * homeCell;

    public:
        INJECT(WorldState(FogOfWar *fogOfWar,
                          EntryController *entryController,
                          BuildingStateManager *buildingStateMgr,
                          CellsTerrains *terrains,
                          CoreMod *core,
                          MovableStateManager *movableStateMgr,
                          InventoryStateManager *inventoryStateMgr,
                          TheTerrains *tts,
                          HomeCellKey * homeCell,
                          CellsDatas *cDatas)) : fogOfWar(fogOfWar),
                                                 core(core),
                                                 movableStateMgr(movableStateMgr),
                                                 inventoryStateMgr(inventoryStateMgr),
                                                 tts(tts),
                                                 buildingStateMgr(buildingStateMgr),
                                                 entryController(entryController),
                                                 terrains(terrains),
                                                 homeCell(homeCell),
                                                 cellsDatas(cDatas)
        {
        }

        virtual void init() override
        {
            // CostMap *costMap = Context<CostMap>::get();
            // CoreMod *core = Context<CoreMod>::get();
            Ogre::Root *root = core->getRoot();

            // Context<FogOfWar>::get()->init();
            // SetHomeCellOfFogWar{cellsDatas}(*fogOfWar);
            //fogOfWar->init();
            // Create frame listener for main loop
            this->tilesState = new CellsState(cellsDatas->tiles, this->tts, this->fogOfWar, this->terrains, core);

            this->tilesState->init();

            this->addChild(this->tilesState);
            //

            //            MovableStateManager *movableStateMgr = Context<MovableStateManager>::get();
            movableStateMgr->init();
            this->addChild(movableStateMgr);

            movableStateMgr->setCellToStandOn(homeCell->cKey);

            //

            // Context<MovableStateManager >::set(movableStateMgr);
            //
            // BuildingStateManager *buildingStateMgr = Context<BuildingStateManager>::get();
            buildingStateMgr->init();
            this->addChild(buildingStateMgr);
            //

            inventoryStateMgr->init();
            this->addChild(inventoryStateMgr);
            //

            core->getAppContext()->addInputListener(entryController);
            root->addFrameListener(entryController);
        }
    };
};
