/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/util/CostMap.h"
#include "fg/Cell.h"
#include "fg/util/CostMap.h"
#include "fg/MeshBuild.h"

#include "fg/util/Property.h"
#include "fg/core/ManualState.h"
#include "fg/util/DiamondSquare.h"
#include "fg/defines.h"
#include "fg/util/Iteration.h"
#include "fg/util/Rect.h"
#include "fg/Config.h"
namespace fog::tiles
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

        using RegionFunc = std::function<bool(HexTile::Key, Tile &, Region &rg)>;

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

            RegionFunc inner = [this, &rg](HexTile::Key cKey, Tile &type, Region &rg3)
            {
                return this->inner(cKey, type, rg3) || rg.inner(cKey, type, rg3);
            };
            this->inner = inner;

            RegionFunc border = [this, &rg](HexTile::Key cKey, Tile &type, Region &rg3)
            {
                return this->border(cKey, type, rg3) || rg.border(cKey, type, rg3);
            };
            this->border = border;
        }

        void intersects(Region &rg)
        {

            RegionFunc inner = [this, &rg](HexTile::Key cKey, Tile &type, Region &rg3)
            {
                return this->inner(cKey, type, rg3) && rg.inner(cKey, type, rg3);
            };
            this->inner = inner;

            RegionFunc border = [this, &rg](HexTile::Key cKey, Tile &type, Region &rg3)
            {
                return this->border(cKey, type, rg3) && rg.border(cKey, type, rg3);
            };
            this->border = border;
        }
    };    

}; // end of namespace
