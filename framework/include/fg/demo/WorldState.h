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
#include "fg/util/HexTile.h"
namespace fog
{
    class WorldState : public State
    {
    protected:
        // CellStateControl *cells;
        TilesState *tilesState;
        TilesTerrains *tts;
        std::vector<std::vector<tiles::Tile>> tiles;

    public:
        WorldState()
        {
        }
        void initTilesAndCostMap()
        {

            tiles::Terrains *terrains = Context<tiles::Terrains>::get();

            int tsWidth = terrains->tWidth;
            int tsHeight = terrains->tHeight;
            int terWidth = terrains->width;
            int terHeight = terrains->height;
            this->tts = new TilesTerrains(tsWidth, tsHeight, terWidth, terHeight);
            // std::vector<std::vector<tiles::Vertex>> vertexs(terWidth, std::vector<tiles::Vertex>(terHeight, tiles::Vertex()));

            tiles::TilesGenerator::generateTiles(tiles, tsWidth, tsHeight);
            // cost map

            CostMap *costMap = new CostMap(tsWidth, tsHeight);
            for (int x = 0; x < tsWidth; x++)
            {
                for (int y = 0; y < tsHeight; y++)
                {
                    int cost = CostMap::DEFAULT_COST;
                    switch (tiles[x][y].type)
                    {
                    case tiles::Type::OCEAN:
                    case tiles::Type::MOUNTAIN:
                    case tiles::Type::LAKE:

                        cost = CostMap::OBSTACLE;
                        break;
                    case tiles::Type::HILL:
                    case tiles::Type::FROZEN:
                    case tiles::Type::RIVER:
                        cost = 2;
                        break;
                    }

                    costMap->setCost(HexTile::Key(x, y), cost);
                }
            }

            Context<CostMap>::set(costMap);
        }


        HexTile::Key findCellToStandOn()
        {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<int> rPosX(0, Config::TILES_RANGE.getWidth() - 1);  //
            std::uniform_int_distribution<int> rPosY(0, Config::TILES_RANGE.getHeight() - 1); //

            for (int i = 0; i < 100; i++)
            {
                int x = rPosX(gen);
                int y = rPosX(gen);
                tiles::TileType type = tiles[x][y].type;
                if (type == tiles::Type::OCEAN || type == tiles::Type::MOUNTAIN)
                {
                    continue;
                }
                return HexTile::Key(x, y);
            }

            return HexTile::Key(0, 0);
        }

        virtual void init() override
        {
            CostMap *costMap = Context<CostMap>::get();
            CoreMod *core = Context<CoreMod>::get();
            Ogre::Root *root = core->getRoot();

            this->initTilesAndCostMap();

            // Create frame listener for main loop
            this->tilesState = new TilesState(this->tiles, this->tts);
            this->tilesState->init();

            this->addChild(this->tilesState);
            //

            MovableStateManager *movableStateMgr = Context<MovableStateManager>::get();
            movableStateMgr->init();
            this->addChild(movableStateMgr);
            movableStateMgr->setCellToStandOn(findCellToStandOn());
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

            EntryController *entryController = new EntryController();
            core->getAppContext()->addInputListener(entryController);
            root->addFrameListener(entryController);
        }
    };
};
