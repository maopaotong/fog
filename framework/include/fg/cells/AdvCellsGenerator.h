/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/ogre.h"

#include "fg/ogre.h"
#include "fg/util.h"
#include "Common.h"
#include "TemperatureGenerator.h"
#include "CellsGenerator.h"

namespace fog
{

    using CellType =  uint8_t;
    /**
     * Generator of tiles with types.
     */
    struct AdvCellsGenerator : public CellsGenerator
    {

        Args opts;
        INJECT(AdvCellsGenerator(Args opts)) : opts(opts), CellsGenerator(opts)
        {
        }
        void generateCellsTypes(GenerateOpCtx &goc) override
        {
            CellsGenerator::generateCellsTypes(goc);
            generateLakeTypes(goc);
            generateBySingleCells(goc);
            generateFrozenTypes(goc);
        }
        void generateLakeTypes(GenerateOpCtx &goc)
        {

            // make lakes
            if (!opts.GENERATOR1_MAKE_LAKE)
            {
                return;
            }
            // mark inner plain ocean as lake:
            // 1. must have a mountain on border.
            // 2. single tile lake.
            //
            MakeLakeByOceanShore makeLakeOp1(goc.tiles, goc.w, goc.h);
            Iteration::forEach<CellData>(goc.tiles, goc.w, goc.h, makeLakeOp1);

            if (makeLakeOp1.totalLakes == 0)
            {
                Iteration::forEach<CellData>(goc.tiles, goc.w, goc.h, ChangeSingleTileTypeOp(goc.tiles, goc.w, goc.h, CellTypes::PLAIN, determineTileTypeForMakeLakeByPlain));
            }
        }

        void generateFrozenTypes(GenerateOpCtx &goc) 
        {
            // make sub type by temperature.
            Iteration::forEach<float>(goc.heightmap, goc.w, goc.w, [this, &goc](int x, int y, float h)
                                      {
                                          CellType &type = goc.tiles[x][y].type;
                                          float tp = goc.tpMap[x][y];
                                          if (tp < this->opts.frozenDistribution) // temperature
                                          {
                                              // TODO frozen as extra attributes of ocean/shore/lake/plain.
                                              // if (type == CellTypes::MOUNTAIN || type == CellTypes::HILL || type == CellTypes::PLAIN)
                                              //{

                                              //}
                                              if (type == CellTypes::OCEAN || type == CellTypes::SHORE || type == CellTypes::LAKE)
                                              {
                                                  type = CellTypes::FRZ_SHORE;
                                              }
                                              else if (type == CellTypes::PLAIN)
                                              {
                                                  type = CellTypes::FRZ_PLAIN;
                                              }
                                              else if (type == CellTypes::MOUNTAIN)
                                              {
                                                  type = CellTypes::FRZ_MOUNTAIN;
                                              }
                                          } //
                                      });

        } // end of generate tiles

        void generateBySingleCells(GenerateOpCtx &goc)
        {

            // remove all other inner ocean/shore
            Iteration::forEach<CellData>(goc.tiles, goc.w, goc.h, RemoveOceanShoreInLand(goc.tiles, goc.w, goc.h));

            // remove single ocean.
            Iteration::forEach<CellData>(goc.tiles, goc.w, goc.h, ChangeSingleTileTypeOp(goc.tiles, goc.w, goc.h, CellTypes::OCEAN, determineNewTypeForSingleOcean));

            // remove single shore
            Iteration::forEach<CellData>(goc.tiles, goc.w, goc.h, ChangeSingleTileTypeOp(goc.tiles, goc.w, goc.h, CellTypes::SHORE, determineNewTypeForSingleShore));

            // resolve single plain
            Iteration::forEach<CellData>(goc.tiles, goc.w, goc.h, ChangeSingleTileTypeOp(goc.tiles, goc.w, goc.h, CellTypes::PLAIN, determineNewTypeForSingleShore));
            /*
             */
        }

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

                CellRegion region([&borderTypes, this](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  { return tile.type == this->type; },
                                  [&borderTypes](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  {
                                      borderTypes.insert(tile.type);
                                      return true; //
                                  },
                                  [&borderTypes, &inners](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  {
                                      inners.insert(&tile);
                                      if (inners.size() > 1) // do not change it ; 1. in case it link to deep ocean. 2. large ocean near land or shore is ok?
                                      {
                                          return false;
                                      }
                                      return true; //
                                  });

                bool isRegion = CellRegion::forEachCellInSameRegion(tiles, w, h, CellKey::Offset(x, y), tl, region); //
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
            CellType mustNeiber = CellTypes::MOUNTAIN;
            int mustNeibersLow = 4;

            MakeLakeByOceanShore(std::vector<std::vector<CellData>> &tiles, int w, int h) : tiles(tiles), w(w), h(h), totalLakes(0)
            {
            }

            void operator()(int x, int y, CellData &tl)
            {
                CellType type = tl.type;

                std::unordered_map<CellType, int> innerTypes;
                std::unordered_map<CellType, int> borderTypes;
                std::unordered_set<CellData *> inners;

                CellRegion region([&innerTypes, &borderTypes](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  {
                                      if (tile.type == CellTypes::SHORE || tile.type == CellTypes::OCEAN)
                                      {
                                          if (auto it = innerTypes.find(tile.type); it == innerTypes.end())
                                          {
                                              innerTypes.emplace(tile.type, 1);
                                          }
                                          else
                                          {
                                              innerTypes[tile.type]++;
                                          }
                                          return true; //
                                      }
                                      return false; //
                                  },
                                  [&borderTypes, &innerTypes](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  {
                                      if (auto it = borderTypes.find(tile.type); it == borderTypes.end())
                                      {
                                          borderTypes.emplace(tile.type, 1);
                                      }
                                      else
                                      {
                                          borderTypes[tile.type]++;
                                      }
                                      return true; //
                                  },
                                  [this, &borderTypes, &innerTypes, &inners](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  {
                                      inners.insert(&tile);
                                      if (inners.size() > 1)
                                      {
                                          return false;
                                      }
                                      if (auto it = borderTypes.find(mustNeiber); it != borderTypes.end())
                                      {
                                          if (borderTypes[mustNeiber] < mustNeibersLow)
                                          {
                                              return false;
                                          }
                                      }
                                      else
                                      {
                                          return false;
                                      }
                                      return true; //
                                  });

                bool isLake = CellRegion::forEachCellInSameRegion(tiles, w, h, CellKey::Offset(x, y), tl, region); //
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

                CellRegion region([&innerTypes, &borderTypes](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  {
                                      if (tile.type == CellTypes::SHORE || tile.type == CellTypes::OCEAN)
                                      {
                                          innerTypes.insert(tile.type);
                                          return true; //
                                      }
                                      return false; //
                                  },
                                  [&borderTypes, &innerTypes](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  {
                                      borderTypes.insert(tile.type);
                                      return true; //
                                  },
                                  [&borderTypes, &innerTypes, &inners](CellKey::Offset cKey, CellData &tile, CellRegion &rg)
                                  {
                                      inners.insert(&tile);
                                      if (inners.size() > 10) // is real ocean.
                                      {
                                          return false;
                                      }
                                      return true; //
                                  });

                bool isRegion = CellRegion::forEachCellInSameRegion(tiles, w, h, CellKey::Offset(x, y), tl, region); //
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

        static CellType determineTileTypeForMakeLakeByPlain(std::unordered_set<CellType> borderTypes)
        {
            // TODO find a random type from border
            if (borderTypes.count(CellTypes::MOUNTAIN) && borderTypes.count(CellTypes::HILL))
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
