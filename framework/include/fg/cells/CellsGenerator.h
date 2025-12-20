/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/ogre.h"

#include "fg/ogre.h"
#include "fg/util.h"
#include "Cells.h"
namespace fog
{

    using CellType = uint8;
    /**
     * Generator of tiles with types.
     */
    class CellsGenerator
    {

    public:
        static void changeType(std::vector<std::vector<CellData>> &tiles, int w, int h, CellRegion &region, CellType toType)
        {
            std::unordered_map<CellType, int> plot;
            Iteration::forEach<CellData>(tiles, w, h, [&tiles, &w, &h, &plot, &region, &toType](int x, int y, CellData &tl)
                                         { bool isRegion =  CellRegion::forEachTileInSameRegion(tiles, w, h, CellKey(x, y), tl, region); });
        }
        static void generateCells(std::vector<std::vector<CellData>> &tiles, int w, int h, Config *config)
        {
            assert(w == h && "cannot generate tiles because w<>h.");
            Iteration::forEachAsTable<std::vector<std::vector<CellData>> &>(w, h, //
                                                                            [](int row, std::vector<std::vector<CellData>> &tiles)
                                                                            { tiles.push_back(std::vector<CellData>()); }, //
                                                                            [](int row, int col, std::vector<std::vector<CellData>> &tiles)
                                                                            { tiles[row].emplace_back(); }, //
                                                                            tiles                           //
            );

            std::vector<std::vector<float>> heightmap(w, std::vector<float>(w, 0.0f));

            DiamondSquare::generateAndNormalise(heightmap, w, config->generatorRoughness1, config->seedOfGenerator1);

            // statistic
            std::unordered_map<CellType, int> plot;
            Iteration::forEach<float>(heightmap, w, w, [&tiles, &plot,config](int x, int y, float h)
                                      {
                                              CellType type = CellTypes::UNKNOW;
                                              if (h < config->GENERATOR1_OCEAN_RATIO) // TODo calculate the actual ratio instead of the height.
                                              {
                                                  type = CellTypes::OCEAN;
                                              }
                                              else if (h < config->GENERATOR1_SHORE_RATIO)
                                              {
                                                  type = CellTypes::SHORE;
                                              }
                                              else if (h < config->GENERATOR1_PLAIN_RATIO)
                                              {
                                                  type = CellTypes::PLAIN;
                                              }
                                              else if (h < config->GENERATOR1_HILL_RATIO)
                                              {
                                                  type = CellTypes::HILL;
                                              }
                                              else if (h < config->GENERATOR1_MOUNTAIN_RATIO)
                                              {
                                                  type = CellTypes::MOUNTAIN;
                                              }
                                              else
                                              {
                                                  type = CellTypes::FROZEN;
                                              }
                                              tiles[x][y].type = type; //

                                              auto &it = plot.find(type);
                                              if (it == plot.end())
                                              {
                                                  plot[type] = 1;
                                              }
                                              else
                                              {
                                                  it->second++;
                                              } });

            // make lakes
            if (config->GENERATOR1_MAKE_LAKE)
            {
                makeLake(tiles, w, h);
            }

        } // end of generate tiles
        /**
         *
         */
        struct ChangeSingleTileTypeOp
        {
            std::vector<std::vector<CellData>> &tiles;
            int w;
            int h;
            CellType type;
            std::function<CellType(std::unordered_set<CellType>)> typeFunc;

            ChangeSingleTileTypeOp(std::vector<std::vector<CellData>> &tiles, int w, int h, CellType type1, std::function<CellType(std::unordered_set<CellType>)> newType) : tiles(tiles), w(w), h(h), type(type1), typeFunc(newType)
            {
            }

            void operator()(int x, int y, CellData &tl)
            {
                CellType type = tl.type;

                std::unordered_set<CellType> borderTypes;
                std::unordered_set<CellData *> inners;

                CellRegion region([&borderTypes, this](CellKey cKey, CellData &tile, CellRegion &rg)
                                  { return tile.type == this->type; },
                                  [&borderTypes](CellKey cKey, CellData &tile, CellRegion &rg)
                                  {
                                      borderTypes.insert(tile.type);
                                      return true; //
                                  },
                                  [&borderTypes, &inners](CellKey cKey, CellData &tile, CellRegion &rg)
                                  {
                                      inners.insert(&tile);
                                      if (inners.size() > 1) // do not change it ; 1. in case it link to deep ocean. 2. large ocean near land or shore is ok?
                                      {
                                          return false;
                                      }
                                      return true; //
                                  });

                bool isRegion =  CellRegion::forEachTileInSameRegion(tiles, w, h, CellKey(x, y), tl, region); //
                if (isRegion)
                {
                    CellType type2 = this->typeFunc(borderTypes); // it will become: 1. plain, 2. shore.
                    for (auto it = inners.begin(); it != inners.end(); it++)
                    {
                        (*it)->type = type2;
                    }
                } //
            }
        };

        struct MakeLakeByOceanShore
        {
            std::vector<std::vector<CellData>> &tiles;
            int w;
            int h;

            int totalLakes;
            MakeLakeByOceanShore(std::vector<std::vector<CellData>> &tiles, int w, int h) : tiles(tiles), w(w), h(h), totalLakes(0)
            {
            }

            void operator()(int x, int y, CellData &tl)
            {
                CellType type = tl.type;

                std::unordered_set<CellType> innerTypes;
                std::unordered_set<CellType> borderTypes;
                std::unordered_set<CellData *> inners;

                CellRegion region([&innerTypes, &borderTypes](CellKey cKey, CellData &tile, CellRegion &rg)
                                  {
                                      if (tile.type == CellTypes::SHORE || tile.type == CellTypes::OCEAN)
                                      {
                                          innerTypes.insert(tile.type);
                                          return true; //
                                      }
                                      return false; //
                                  },
                                  [&borderTypes, &innerTypes](CellKey cKey, CellData &tile, CellRegion &rg)
                                  {
                                      borderTypes.insert(tile.type);
                                      return true; //
                                  },
                                  [&borderTypes, &innerTypes, &inners](CellKey cKey, CellData &tile, CellRegion &rg)
                                  {
                                      inners.insert(&tile);
                                      if (inners.size() > 1)
                                      {
                                          return false;
                                      }
                                      if (borderTypes.count(CellTypes::FROZEN) == 0)
                                      {
                                          return false;
                                      }
                                      return true; //
                                  });

                bool isLake =  CellRegion::forEachTileInSameRegion(tiles, w, h, CellKey(x, y), tl, region); //
                if (isLake)
                {
                    tl.type = CellTypes::LAKE;
                    this->totalLakes++;
                } //
            }
        };

        struct RemoveOceanShoreInLand
        {
            std::vector<std::vector<CellData>> &tiles;
            int w;
            int h;

            RemoveOceanShoreInLand(std::vector<std::vector<CellData>> &tiles, int w, int h) : tiles(tiles), w(w), h(h)
            {
            }
            void operator()(int x, int y, CellData &tl)
            {
                CellType type = tl.type;

                std::unordered_set<CellType> innerTypes;
                std::unordered_set<CellType> borderTypes;
                std::unordered_set<CellData *> inners;

                CellRegion region([&innerTypes, &borderTypes](CellKey cKey, CellData &tile, CellRegion &rg)
                                  {
                                      if (tile.type == CellTypes::SHORE || tile.type == CellTypes::OCEAN)
                                      {
                                          innerTypes.insert(tile.type);
                                          return true; //
                                      }
                                      return false; //
                                  },
                                  [&borderTypes, &innerTypes](CellKey cKey, CellData &tile, CellRegion &rg)
                                  {
                                      borderTypes.insert(tile.type);
                                      return true; //
                                  },
                                  [&borderTypes, &innerTypes, &inners](CellKey cKey, CellData &tile, CellRegion &rg)
                                  {
                                      inners.insert(&tile);
                                      if (inners.size() > 10) // is real ocean.
                                      {
                                          return false;
                                      }
                                      return true; //
                                  });

                bool isRegion = CellRegion::forEachTileInSameRegion(tiles, w, h, CellKey(x, y), tl, region); //
                if (isRegion)
                {
                    CellType newType = determineNewTypeForInnerMiddleOcean(borderTypes);
                    for (auto it = inners.begin(); it != inners.end(); it++)
                    {
                        (*it)->type = newType;
                    }
                } //
            }
        };

        static void makeLake(std::vector<std::vector<CellData>> &tiles, int w, int h)
        {

            // mark inner plain ocean as lake:
            // 1. must have a mountain on border.
            // 2. single tile lake.
            //
            MakeLakeByOceanShore makeLakeOp1(tiles, w, h);
            Iteration::forEach<CellData>(tiles, w, h, makeLakeOp1);

            if (makeLakeOp1.totalLakes == 0)
            {
                Iteration::forEach<CellData>(tiles, w, h, ChangeSingleTileTypeOp(tiles, w, h, CellTypes::PLAIN, determineTileTypeForMakeLakeByPlain));
            }

            // remove all other inner ocean/shore
            Iteration::forEach<CellData>(tiles, w, h, RemoveOceanShoreInLand(tiles, w, h));

            // remove single ocean.
            Iteration::forEach<CellData>(tiles, w, h, ChangeSingleTileTypeOp(tiles, w, h, CellTypes::OCEAN, determineNewTypeForSingleOcean));

            // remove single shore
            Iteration::forEach<CellData>(tiles, w, h, ChangeSingleTileTypeOp(tiles, w, h, CellTypes::SHORE, determineNewTypeForSingleShore));

            // resolve single plain
            Iteration::forEach<CellData>(tiles, w, h, ChangeSingleTileTypeOp(tiles, w, h, CellTypes::PLAIN, determineNewTypeForSingleShore));
            /*
             */

        } // end of method makeLake tiles.

        static CellType determineTileTypeForMakeLakeByPlain(std::unordered_set<CellType> borderTypes)
        {
            // TODO find a random type from border
            if (borderTypes.count(CellTypes::FROZEN) && borderTypes.count(CellTypes::MOUNTAIN) && borderTypes.count(CellTypes::HILL))
            { // near fronzen and mountain and hill.
                return CellTypes::LAKE;
            }
            return CellTypes::PLAIN;
        }

        static CellType determineNewTypeForInnerMiddleOcean(std::unordered_set<CellType> borderTypes)
        {
            // TODO find a random type from border
            return *borderTypes.begin();
        }

        static CellType determineNewTypeForSingleShore(std::unordered_set<CellType> borderTypes)
        {
            // TODO find a random type from border
            return *borderTypes.begin();
        }

        static CellType determineNewTypeForSingleOcean(std::unordered_set<CellType> borderTypes)
        {
            // TODO find a random type from border
            return *borderTypes.begin();
        }

    }; // end of class

}; // end of namespace
