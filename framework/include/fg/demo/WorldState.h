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
namespace fog
{
    class WorldState : public State
    {
    protected:
        // CellStateControl *cells;
        CellsState *tilesState;
        TheTerrains *tts;
        std::vector<std::vector<CellData>> tiles;
        FogOfWar *fogOfWar;
        EntryController *entryController;
        CellsTerrains *terrains;

    public:
        INJECT(WorldState(FogOfWar *fogOfWar,
                          EntryController *entryController,
                          CellsTerrains *terrains)) : fogOfWar(fogOfWar),
                                                      entryController(entryController),
                                                      terrains(terrains)
        {
        }
        void initCellsAndCostMap()
        {

            // CellsTerrains *terrains = Context<CellsTerrains>::get();

            int tsWidth = terrains->tWidth;
            int tsHeight = terrains->tHeight;
            int terWidth = terrains->width;
            int terHeight = terrains->height;
            this->tts = new TheTerrains(tsWidth, tsHeight, terWidth, terHeight);
            // std::vector<std::vector<tiles::Vertex>> vertexs(terWidth, std::vector<tiles::Vertex>(terHeight, tiles::Vertex()));

            CellsGenerator::generateCells(tiles, tsWidth, tsHeight);
            // cost map

            CostMap *costMap = new CostMap(tsWidth, tsHeight);
            Context<CellsCost>::set(new CellsCost(&tiles));

            Context<CostMap>::set(costMap);
        }

        CellKey findCellToStandOn()
        {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<int> rPosX(0, Config::TILES_RANGE.getWidth() - 1);  //
            std::uniform_int_distribution<int> rPosY(0, Config::TILES_RANGE.getHeight() - 1); //

            for (int i = 0; i < 100; i++)
            {
                int x = rPosX(gen);
                int y = rPosX(gen);
                CellType type = tiles[x][y].type;
                if (type == CellTypes::OCEAN || type == CellTypes::MOUNTAIN)
                {
                    continue;
                }
                return CellKey(x, y);
            }

            return CellKey(0, 0);
        }

        virtual void init() override
        {
            CostMap *costMap = Context<CostMap>::get();
            CoreMod *core = Context<CoreMod>::get();
            Ogre::Root *root = core->getRoot();

            this->initCellsAndCostMap();
            CellKey cKey = findCellToStandOn(); //

            // Context<FogOfWar>::get()->setHomeCell(cKey);
            // Context<FogOfWar>::get()->init();
            fogOfWar->init();
            // Create frame listener for main loop
            this->tilesState = new CellsState(this->tiles, this->tts, this->fogOfWar, this->terrains);

            this->tilesState->init();

            this->addChild(this->tilesState);
            //

            MovableStateManager *movableStateMgr = Context<MovableStateManager>::get();
            movableStateMgr->init();
            this->addChild(movableStateMgr);
            movableStateMgr->setCellToStandOn(cKey);

            //

            // Context<MovableStateManager >::set(movableStateMgr);
            //
            BuildingStateManager *buildingStateMgr = Context<BuildingStateManager>::get();
            buildingStateMgr->init();
            this->addChild(buildingStateMgr);
            //

            InventoryStateManager *inventoryStateMgr = Context<InventoryStateManager>::get();
            inventoryStateMgr->init();
            this->addChild(inventoryStateMgr);
            //

            core->getAppContext()->addInputListener(entryController);
            root->addFrameListener(entryController);
        }
    };
};
