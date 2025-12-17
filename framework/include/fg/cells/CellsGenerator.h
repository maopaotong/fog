/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/ogre.h"
#include "fg/core.h"
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
                                         { bool isRegion = forEachTileInSameRegion(tiles, w, h, CellKey(x, y), tl, region); });
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
            // DiamondSquare::generateAndNormalise(heightmap, w, config->GENERATOR_ROUGHNESS1, config->GENERATOR_SEED1);
            DiamondSquare::generateAndNormalise(heightmap, w, config->GENERATOR2_ROUGHNESS, config->seedOfGenerator2);

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

                bool isRegion = forEachTileInSameRegion(tiles, w, h, CellKey(x, y), tl, region); //
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

                bool isLake = forEachTileInSameRegion(tiles, w, h, CellKey(x, y), tl, region); //
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

                bool isRegion = forEachTileInSameRegion(tiles, w, h, CellKey(x, y), tl, region); //
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

    private:
        struct VisitCtx
        {
            std::vector<std::vector<CellData>> &tiles;
            int w;
            int h;
            CellRegion &region;
            std::unordered_set<CellData *> processed;
            std::unordered_set<CellData *> border;
            bool goOn;
            VisitCtx(std::vector<std::vector<CellData>> &tiles,
                     int w,
                     int h,
                     CellRegion &region) : tiles(tiles), w(w), h(h),
                                           region(region),
                                           processed(std::unordered_set<CellData *>()),
                                           border(std::unordered_set<CellData *>()),
                                           goOn(true)
            {
            }
        };

    public:
        static bool forEachTileInSameRegion(std::vector<std::vector<CellData>> &tiles, int w, int h, CellKey cKey, CellData &tile, CellRegion &region)
        {
            VisitCtx ctx(tiles, w, h, region);
            return doForEachTileInSameRegion(0, ctx, cKey, tile);
        }

    private:
        static bool doForEachTileInSameRegion(int depth, VisitCtx &ctx, CellKey cKey, CellData &tile0)
        {
            if (ctx.processed.count(&tile0))
            {
                return true;
            }

            ctx.processed.insert(&tile0);

            // start point must be inner.
            // no need to check again if depth > 0.
            if (depth == 0 && !ctx.region.inner(cKey, tile0, ctx.region))
            {
                //! isInner and isBorder, so recursive terminal condition meet.
                return false;
            }

            // is Inner, so check the neiber if valid thus recursive calling.
            // is Inner and not border.
            CellKey neibers[6];
            Cell::getNeibers(cKey.x, cKey.y, neibers);
            // we remember all inner neibers for next recursive calling.
            std::unordered_map<int, CellData *> inners;
            for (int i = 0; i < 6; i++)
            {

                if (!isValid(neibers[i].x, neibers[i].y, ctx.w, ctx.h))
                {
                    continue;
                }

                CellData &tileI = ctx.tiles[neibers[i].x][neibers[i].y];

                if (ctx.region.inner(cKey, tileI, ctx.region))
                {
                    // remeber this inner , before enter recursive calling, first thing to do is calling border and check user func, this is more important.
                    inners.emplace(i, &tileI);
                }
                else
                {
                    if (!ctx.region.border(cKey, tileI, ctx.region))
                    {                 // this tile not inner, not border, so the border is broken, all tiles under checking is gaving up.
                        return false; // break, all the tiles during the calling is not the desired region
                    }
                }
                // is inner or border
            } // end of for.

            // before enter next depth, call user check function first.

            if (!ctx.region.check(cKey, tile0, ctx.region))
            {
                return false; // user func break the processing, so we give up.
            }

            // next depth:
            for (auto it = inners.begin(); it != inners.end(); it++)
            {
                int i = it->first;
                CellData *tileI = it->second;
                if (!doForEachTileInSameRegion(depth + 1, ctx, neibers[i], *tileI))
                {
                    return false;
                }
            }
            // current tile is ok for vist.
            // every thing is done.
            return true;
        }

        static bool isValid(int x, int y, int w, int h)
        {
            return x >= 0 && y >= 0 && x < w && y < h;
        }
    }; // end of class

}; // end of namespace
