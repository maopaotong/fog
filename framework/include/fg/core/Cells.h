/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/util/DiamondSquare.h"
#include "fg/Common.h"
#include "fg/util/Iteration.h"
#include "fg/util/Rect.h"
#include "fg/Config.h"
namespace fog::cells
{

    using TileType = uint8;

    class Type
    {
    public:
        static constexpr TileType SKY = 100; //
        static constexpr TileType RIVER = 9;
        static constexpr TileType LAKE = 8;
        static constexpr TileType DESERT = 7;
        static constexpr TileType FROZEN = 6;
        static constexpr TileType MOUNTAIN = 5;
        static constexpr TileType HILL = 4;
        static constexpr TileType PLAIN = 3;
        static constexpr TileType SHORE = 2;
        static constexpr TileType OCEAN = 1;
        static constexpr TileType UNKNOW = 0;
        //
    };
    //
    struct Tile
    {
        TileType type;
        std::unordered_map<TileType, int> regions; // region , additional types

        Tile() : type(Type::UNKNOW) // unkonwn
        {
        }

        void removeRegion(TileType rType)
        {
            regions.erase(rType);
        }

        void addRegion(TileType rType, int size)
        {
            regions.emplace(rType, size);
        }
        int getRegion(TileType rType)
        {
            auto it = regions.find(rType);
            if (it == regions.end())
            {
                return 0;
            }
            return it->second;
        }
    };

    struct Region
    {

        using RegionFunc = std::function<bool(CellKey, Tile &, Region &rg)>;

        RegionFunc inner; // inner judge

        RegionFunc border; // border judge

        RegionFunc check; // user additional checker.

        Region() = delete;

        Region(RegionFunc inner, RegionFunc border, RegionFunc check) : inner(inner), border(border), check(check)
        {
        }

        // Region(Region &) = delete;
        void merge(Region &rg)
        {

            RegionFunc inner = [this, &rg](CellKey cKey, Tile &type, Region &rg3)
            {
                return this->inner(cKey, type, rg3) || rg.inner(cKey, type, rg3);
            };
            this->inner = inner;

            RegionFunc border = [this, &rg](CellKey cKey, Tile &type, Region &rg3)
            {
                return this->border(cKey, type, rg3) || rg.border(cKey, type, rg3);
            };
            this->border = border;
        }

        void intersects(Region &rg)
        {

            RegionFunc inner = [this, &rg](CellKey cKey, Tile &type, Region &rg3)
            {
                return this->inner(cKey, type, rg3) && rg.inner(cKey, type, rg3);
            };
            this->inner = inner;

            RegionFunc border = [this, &rg](CellKey cKey, Tile &type, Region &rg3)
            {
                return this->border(cKey, type, rg3) && rg.border(cKey, type, rg3);
            };
            this->border = border;
        }
    };    

}; // end of namespace
