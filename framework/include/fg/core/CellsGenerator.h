/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/State.h"
#include "fg/core/CoreMod.h"
#include "fg/util.h"
#include "fg/MeshBuild.h"

#include "fg/core/ManualState.h"
#include "fg/util/DiamondSquare.h"
#include "fg/Common.h"
#include "fg/util/Iteration.h"
#include "fg/util/Rect.h"
#include "fg/Config.h"
#include "fg/core/Cells.h"
namespace fog::cells
{

    using TileType = uint8;
    /**
     * Generator of tiles with types.
     */
    class CellsGenerator
    {

    public:
        static void changeType(std::vector<std::vector<Tile>> &tiles, int w, int h, Region &region, TileType toType)
        {
            std::unordered_map<TileType, int> plot;
            Iteration::forEach<Tile>(tiles, w, h, [&tiles, &w, &h, &plot, &region, &toType](int x, int y, Tile &tl)
                                     { bool isRegion = forEachTileInSameRegion(tiles, w, h, Cell::Key(x, y), tl, region); });
        }
        static void generateCells(std::vector<std::vector<Tile>> &tiles, int w, int h)
        {
            assert(w == h && "cannot generate tiles because w<>h.");
            Iteration::forEachAsTable<std::vector<std::vector<Tile>> &>(w, h, //
                                                                        [](int row, std::vector<std::vector<Tile>> &tiles)
                                                                        { tiles.push_back(std::vector<Tile>()); }, //
                                                                        [](int row, int col, std::vector<std::vector<Tile>> &tiles)
                                                                        { tiles[row].emplace_back(); }, //
                                                                        tiles                           //
            );

            std::vector<std::vector<float>> heightmap(w, std::vector<float>(w, 0.0f));
            // DiamondSquare::generateAndNormalise(heightmap, w, Config::GENERATOR_ROUGHNESS1, Config::GENERATOR_SEED1);
            DiamondSquare::generateAndNormalise(heightmap, w, Config::GENERATOR2_ROUGHNESS, Config::GENERATOR2_SEED);

            // statistic
            std::unordered_map<TileType, int> plot;
            Iteration::forEach<float>(heightmap, w, w, [&tiles, &plot](int x, int y, float h)
                                      {
                                              TileType type = Type::UNKNOW;
                                              if (h < Config::GENERATOR1_OCEAN_RATIO) // TODo calculate the actual ratio instead of the height.
                                              {
                                                  type = Type::OCEAN;
                                              }
                                              else if (h < Config::GENERATOR1_SHORE_RATIO)
                                              {
                                                  type = Type::SHORE;
                                              }
                                              else if (h < Config::GENERATOR1_PLAIN_RATIO)
                                              {
                                                  type = Type::PLAIN;
                                              }
                                              else if (h < Config::GENERATOR1_HILL_RATIO)
                                              {
                                                  type = Type::HILL;
                                              }
                                              else if (h < Config::GENERATOR1_MOUNTAIN_RATIO)
                                              {
                                                  type = Type::MOUNTAIN;
                                              }
                                              else
                                              {
                                                  type = Type::FROZEN;
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
            if (Config::GENERATOR1_MAKE_LAKE)
            {
                makeLake(tiles, w, h);
            }

        } // end of generate tiles
        /**
         *
         */
        struct ChangeSingleTileTypeOp
        {
            std::vector<std::vector<Tile>> &tiles;
            int w;
            int h;
            TileType type;
            std::function<TileType(std::unordered_set<TileType>)> typeFunc;

            ChangeSingleTileTypeOp(std::vector<std::vector<Tile>> &tiles, int w, int h, TileType type1, std::function<TileType(std::unordered_set<TileType>)> newType) : tiles(tiles), w(w), h(h), type(type1), typeFunc(newType)
            {
            }

            void operator()(int x, int y, Tile &tl)
            {
                TileType type = tl.type;

                std::unordered_set<TileType> borderTypes;
                std::unordered_set<Tile *> inners;

                Region region([&borderTypes, this](Cell::Key cKey, Tile &tile, Region &rg)
                              { return tile.type == this->type; },
                              [&borderTypes](Cell::Key cKey, Tile &tile, Region &rg)
                              {
                                  borderTypes.insert(tile.type);
                                  return true; //
                              },
                              [&borderTypes, &inners](Cell::Key cKey, Tile &tile, Region &rg)
                              {
                                  inners.insert(&tile);
                                  if (inners.size() > 1) // do not change it ; 1. in case it link to deep ocean. 2. large ocean near land or shore is ok?
                                  {
                                      return false;
                                  }
                                  return true; //
                              });

                bool isRegion = forEachTileInSameRegion(tiles, w, h, Cell::Key(x, y), tl, region); //
                if (isRegion)
                {
                    TileType type2 = this->typeFunc(borderTypes); // it will become: 1. plain, 2. shore.
                    for (auto it = inners.begin(); it != inners.end(); it++)
                    {
                        (*it)->type = type2;
                    }
                } //
            }
        };

        struct MakeLakeByOceanShore
        {
            std::vector<std::vector<Tile>> &tiles;
            int w;
            int h;

            int totalLakes;
            MakeLakeByOceanShore(std::vector<std::vector<Tile>> &tiles, int w, int h) : tiles(tiles), w(w), h(h), totalLakes(0)
            {
            }

            void operator()(int x, int y, Tile &tl)
            {
                TileType type = tl.type;

                std::unordered_set<TileType> innerTypes;
                std::unordered_set<TileType> borderTypes;
                std::unordered_set<Tile *> inners;

                Region region([&innerTypes, &borderTypes](Cell::Key cKey, Tile &tile, Region &rg)
                              {
                                  if (tile.type == Type::SHORE || tile.type == Type::OCEAN)
                                  {
                                      innerTypes.insert(tile.type);
                                      return true; //
                                  }
                                  return false; //
                              },
                              [&borderTypes, &innerTypes](Cell::Key cKey, Tile &tile, Region &rg)
                              {
                                  borderTypes.insert(tile.type);
                                  return true; //
                              },
                              [&borderTypes, &innerTypes, &inners](Cell::Key cKey, Tile &tile, Region &rg)
                              {
                                  inners.insert(&tile);
                                  if (inners.size() > 1)
                                  {
                                      return false;
                                  }
                                  if (borderTypes.count(Type::FROZEN) == 0)
                                  {
                                      return false;
                                  }
                                  return true; //
                              });

                bool isLake = forEachTileInSameRegion(tiles, w, h, Cell::Key(x, y), tl, region); //
                if (isLake)
                {
                    tl.type = Type::LAKE;
                    this->totalLakes++;
                } //
            }
        };

        struct RemoveOceanShoreInLand
        {
            std::vector<std::vector<Tile>> &tiles;
            int w;
            int h;

            RemoveOceanShoreInLand(std::vector<std::vector<Tile>> &tiles, int w, int h) : tiles(tiles), w(w), h(h)
            {
            }
            void operator()(int x, int y, Tile &tl)
            {
                TileType type = tl.type;

                std::unordered_set<TileType> innerTypes;
                std::unordered_set<TileType> borderTypes;
                std::unordered_set<Tile *> inners;

                Region region([&innerTypes, &borderTypes](Cell::Key cKey, Tile &tile, Region &rg)
                              {
                                  if (tile.type == Type::SHORE || tile.type == Type::OCEAN)
                                  {
                                      innerTypes.insert(tile.type);
                                      return true; //
                                  }
                                  return false; //
                              },
                              [&borderTypes, &innerTypes](Cell::Key cKey, Tile &tile, Region &rg)
                              {
                                  borderTypes.insert(tile.type);
                                  return true; //
                              },
                              [&borderTypes, &innerTypes, &inners](Cell::Key cKey, Tile &tile, Region &rg)
                              {
                                  inners.insert(&tile);
                                  if (inners.size() > 10) // is real ocean.
                                  {
                                      return false;
                                  }
                                  return true; //
                              });

                bool isRegion = forEachTileInSameRegion(tiles, w, h, Cell::Key(x, y), tl, region); //
                if (isRegion)
                {
                    TileType newType = determineNewTypeForInnerMiddleOcean(borderTypes);
                    for (auto it = inners.begin(); it != inners.end(); it++)
                    {
                        (*it)->type = newType;
                    }
                } //
            }
        };

        static void makeLake(std::vector<std::vector<Tile>> &tiles, int w, int h)
        {

            // mark inner plain ocean as lake:
            // 1. must have a mountain on border.
            // 2. single tile lake.
            //
            MakeLakeByOceanShore makeLakeOp1(tiles, w, h);
            Iteration::forEach<Tile>(tiles, w, h, makeLakeOp1);

            if (makeLakeOp1.totalLakes == 0)
            {
                Iteration::forEach<Tile>(tiles, w, h, ChangeSingleTileTypeOp(tiles, w, h, Type::PLAIN, determineTileTypeForMakeLakeByPlain));
            }

            // remove all other inner ocean/shore
            Iteration::forEach<Tile>(tiles, w, h, RemoveOceanShoreInLand(tiles, w, h));

            // remove single ocean.
            Iteration::forEach<Tile>(tiles, w, h, ChangeSingleTileTypeOp(tiles, w, h, Type::OCEAN, determineNewTypeForSingleOcean));

            // remove single shore
            Iteration::forEach<Tile>(tiles, w, h, ChangeSingleTileTypeOp(tiles, w, h, Type::SHORE, determineNewTypeForSingleShore));

            // resolve single plain
            Iteration::forEach<Tile>(tiles, w, h, ChangeSingleTileTypeOp(tiles, w, h, Type::PLAIN, determineNewTypeForSingleShore));
            /*
             */

        } // end of method makeLake tiles.

        static TileType determineTileTypeForMakeLakeByPlain(std::unordered_set<TileType> borderTypes)
        {
            // TODO find a random type from border
            if (borderTypes.count(Type::FROZEN) && borderTypes.count(Type::MOUNTAIN) && borderTypes.count(Type::HILL))
            { // near fronzen and mountain and hill.
                return Type::LAKE;
            }
            return Type::PLAIN;
        }

        static TileType determineNewTypeForInnerMiddleOcean(std::unordered_set<TileType> borderTypes)
        {
            // TODO find a random type from border
            return *borderTypes.begin();
        }

        static TileType determineNewTypeForSingleShore(std::unordered_set<TileType> borderTypes)
        {
            // TODO find a random type from border
            return *borderTypes.begin();
        }

        static TileType determineNewTypeForSingleOcean(std::unordered_set<TileType> borderTypes)
        {
            // TODO find a random type from border
            return *borderTypes.begin();
        }

    private:
        struct VisitCtx
        {
            std::vector<std::vector<Tile>> &tiles;
            int w;
            int h;
            Region &region;
            std::unordered_set<Tile *> processed;
            std::unordered_set<Tile *> border;
            bool goOn;
            VisitCtx(std::vector<std::vector<Tile>> &tiles,
                     int w,
                     int h,
                     Region &region) : tiles(tiles), w(w), h(h),
                                       region(region),
                                       processed(std::unordered_set<Tile *>()),
                                       border(std::unordered_set<Tile *>()),
                                       goOn(true)
            {
            }
        };

    public:
        static bool forEachTileInSameRegion(std::vector<std::vector<Tile>> &tiles, int w, int h, Cell::Key cKey, Tile &tile, Region &region)
        {
            VisitCtx ctx(tiles, w, h, region);
            return doForEachTileInSameRegion(0, ctx, cKey, tile);
        }

    private:
        static bool doForEachTileInSameRegion(int depth, VisitCtx &ctx, Cell::Key cKey, Tile &tile0)
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
            Cell::Key neibers[6];
            Cell::getNeibers(cKey.x, cKey.y, neibers);
            // we remember all inner neibers for next recursive calling.
            std::unordered_map<int, Tile *> inners;
            for (int i = 0; i < 6; i++)
            {

                if (!isValid(neibers[i].x, neibers[i].y, ctx.w, ctx.h))
                {
                    continue;
                }

                Tile &tileI = ctx.tiles[neibers[i].x][neibers[i].y];

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
                Tile *tileI = it->second;
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
