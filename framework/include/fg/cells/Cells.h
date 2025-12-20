/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/util/DiamondSquare.h"
#include "Common.h"
#include "fg/util/Iteration.h"
#include "fg/util/Rect.h"

namespace fog
{

    using CellType = uint8;

    class CellTypes
    {
    public:
        static constexpr CellType SKY = 100; //
        static constexpr CellType RIVER = 9;
        static constexpr CellType LAKE = 8;
        static constexpr CellType DESERT = 7;
        static constexpr CellType FROZEN = 6;
        static constexpr CellType MOUNTAIN = 5;
        static constexpr CellType HILL = 4;
        static constexpr CellType PLAIN = 3;
        static constexpr CellType SHORE = 2;
        static constexpr CellType OCEAN = 1;
        static constexpr CellType UNKNOW = 0;
        //
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
    };    

}; // end of namespace
