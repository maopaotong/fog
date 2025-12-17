/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/core.h"
#include "CellStateControl.h"
#include "CameraState.h"
#include "EntityState.h"
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
        TheTerrains *tts;
        // std::vector<std::vector<CellData>> tiles;
        CellsDatas *cellsDatas;
        FogOfWar *fogOfWar;
        EntryController *entryController;
        CellsTerrains *terrains;
        CoreMod *core;
        MovableStateManager *movableStateMgr;
        BuildingStateManager *buildingStateMgr;
        InventoryManager *inventoryStateMgr;
        HomeCellKey * homeCell;

    public:
        INJECT(WorldManager(FogOfWar *fogOfWar,
                          EntryController *entryController,
                          BuildingStateManager *buildingStateMgr,
                          CellsTerrains *terrains,
                          CoreMod *core,
                          MovableStateManager *movableStateMgr,
                          InventoryManager *inventoryStateMgr,
                          TheTerrains *tts,
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
                                                 terrains(terrains),
                                                 homeCell(homeCell),
                                                 cellsDatas(cDatas)
        {
                   
            Ogre::Root *root = core->getRoot();


            //this->cellsState->rebuildMesh();

            core->getAppContext()->addInputListener(entryController);
            root->addFrameListener(entryController);
        }
    };
};
