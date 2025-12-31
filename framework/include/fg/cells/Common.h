/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/util.h"
namespace fog
{

    using CellType = uint8_t;

    class CellTypes
    {
    public:
        //
        //
        static constexpr CellType SKY = 100; //

        static constexpr CellType FRZ_MOUNTAIN = 12;
        static constexpr CellType FRZ_PLAIN = 11;
        static constexpr CellType FRZ_SHORE = 10;
        static constexpr CellType RIVER = 9;
        static constexpr CellType LAKE = 8;
        static constexpr CellType DESERT = 7;
        static constexpr CellType MOUNTAIN = 5;
        static constexpr CellType HILL = 4;
        static constexpr CellType PLAIN = 3;
        static constexpr CellType SHORE = 2;
        static constexpr CellType OCEAN = 1;
        static constexpr CellType UNKNOW = 0;
    };
    
    //
    struct CellData
    {
        CellType type;
        std::unordered_map<CellType, int> regions; // region , additional types

        CellData() : type(CellTypes::UNKNOW) // unkonwn
        {
        }

        void removeRegion(CellType rType)
        {
            regions.erase(rType);
        }

        void addRegion(CellType rType, int size)
        {
            regions.emplace(rType, size);
        }
        int getRegion(CellType rType)
        {
            auto it = regions.find(rType);
            if (it == regions.end())
            {
                return 0;
            }
            return it->second;
        }
    };

    /**
     *
     * Define a region by declaring the inner function, the border function and additional check function.
     * This is used as argument to iterate on the cells collection.
     *
     */
    struct CellRegion
    {
        using RegionFunc = std::function<bool(CellKey, CellData &, CellRegion &rg)>;

        RegionFunc inner; // inner judge

        RegionFunc border; // border judge

        RegionFunc check; // user additional checker.

        CellRegion() = delete;

        CellRegion(RegionFunc inner, RegionFunc border, RegionFunc check) : inner(inner), border(border), check(check)
        {
        }

        // Region(Region &) = delete;
        void merge(CellRegion &rg)
        {

            RegionFunc inner = [this, &rg](CellKey cKey, CellData &type, CellRegion &rg3)
            {
                return this->inner(cKey, type, rg3) || rg.inner(cKey, type, rg3);
            };
            this->inner = inner;

            RegionFunc border = [this, &rg](CellKey cKey, CellData &type, CellRegion &rg3)
            {
                return this->border(cKey, type, rg3) || rg.border(cKey, type, rg3);
            };
            this->border = border;
        }

        void intersects(CellRegion &rg)
        {

            RegionFunc inner = [this, &rg](CellKey cKey, CellData &type, CellRegion &rg3)
            {
                return this->inner(cKey, type, rg3) && rg.inner(cKey, type, rg3);
            };
            this->inner = inner;

            RegionFunc border = [this, &rg](CellKey cKey, CellData &type, CellRegion &rg3)
            {
                return this->border(cKey, type, rg3) && rg.border(cKey, type, rg3);
            };
            this->border = border;
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
        static bool forEachCellInSameRegion(std::vector<std::vector<CellData>> &tiles, int w, int h, CellKey cKey, CellData &tile, CellRegion &region)
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
            CellsLayout::getNeibers<CellLayout>(cKey, neibers);
            // we remember all inner neibers for next recursive calling.
            std::unordered_map<int, CellData *> inners;
            for (int i = 0; i < 6; i++)
            {

                if (!isValid(neibers[i].col, neibers[i].row, ctx.w, ctx.h))
                {
                    continue;
                }

                CellData &tileI = ctx.tiles[neibers[i].col][neibers[i].row];

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
    };

}; // end of namespace
