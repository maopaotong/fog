/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "MovingStateManager.h"
#include "CellInstanceStateManager.h"
#include "EntryController.h"
#include "MovableStateManager.h"
#include "BuildingStateManager.h"
#include "InventoryStateManager.h"
#include "fg/cells.h"
#include "fg/util.h"
#include "FogOfWar.h"
#include "HomeCellKey.h"
namespace fog
{

    class WorldManager
    {
    protected:
        // CellStateControl *cells;
        CellsState *cellsState;
        TransformD2TD3 *tts;
        // std::vector<std::vector<CellData>> tiles;
        CellsDatas *cellsDatas;
        FogOfWar *fogOfWar;
        EntryController *entryController;
        CoreMod *core;
        MovableStateManager *movableStateMgr;
        BuildingStateManager *buildingStateMgr;
        InventoryManager *inventoryStateMgr;
        HomeCellKey * homeCell;

    public:
        INJECT(WorldManager(FogOfWar *fogOfWar,
                          EntryController *entryController,
                          BuildingStateManager *buildingStateMgr,
                          CoreMod *core,
                          MovableStateManager *movableStateMgr,
                          InventoryManager *inventoryStateMgr,
                          TransformD2TD3 *tts,
                          HomeCellKey * homeCell,
                          CellsState * cellsState,
                          CellsDatas *cDatas)) : fogOfWar(fogOfWar),
                                                 core(core),
                                                 cellsState(cellsState),
                                                 movableStateMgr(movableStateMgr),
                                                 inventoryStateMgr(inventoryStateMgr),
                                                 tts(tts),
                                                 buildingStateMgr(buildingStateMgr),
                                                 entryController(entryController),
                                                 homeCell(homeCell),
                                                 cellsDatas(cDatas)
        {
                   


            //this->cellsState->rebuildMesh();

            
        }
    };
};
