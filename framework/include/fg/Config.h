/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util/Options.h"
#include "fg/util/Box2.h"
#include "fg/util.h"
namespace fog
{
    class Config
    {
    public:
        static float HEIGHT_SCALE;
        static Box2<int> TILES_RANGE;
        static int TILE_TERRAIN_QUALITY;
        static int TILE_MESH_QUALITY;
        static float CELL_SCALE;
        static float WORLD_WIDTH;     // = CELL_SCALE * 2.0 * TILES_WIDTH;
        static float WORLD_HEIGHT;    // = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
                                      //
        static float HEIGHT_OCEAN;    // = 0.49f * 0.9f;
        static float HEIGHT_SHORE;    // = 0.50f * 0.9f;
        static float HEIGHT_PLAIN;    // = 0.51f * 1.1f;
        static float HEIGHT_HILL;     // = 0.52f * 1.2f;
        static float HEIGHT_MOUNTAIN; // = 0.53f * 1.f;
        static float HEIGHT_FROZEN;   // = 0.54f * 1.f;
        // generator
        static int GENERATOR1_SEED;
        static int GENERATOR2_SEED;
        static float GENERATOR1_ROUGHNESS;
        static float GENERATOR2_ROUGHNESS;
        static float GENERATOR1_OCEAN_RATIO;
        static float GENERATOR1_SHORE_RATIO;
        static float GENERATOR1_PLAIN_RATIO;
        static float GENERATOR1_HILL_RATIO;
        static float GENERATOR1_MOUNTAIN_RATIO;
        static float GENERATOR1_RATIO_LAKE_OF_PLAIN;
        static bool GENERATOR1_MAKE_LAKE;
        // fragment
        static bool FRAG_SHOW_EDGE;
        // fog of war
        static Box2<int> FOG_OF_WAR_TEX_RANGE;
        static Box2<int> FOG_OF_WAR_EREASE_RANGE;
        static float STATE_MOVED_EVENT_DISTNACE;
        // debug
        static Box2<int> DEBUG_PRINT_TERRAINS_TEX_RANGE;
        static bool DEBUG_COUT;
        static bool DEBUG_MOVING_POSITION;
        static bool DEBUG_FOG_OF_WAR;
                

    private:
        static Box2<int> Config::parseValueOfRange2Int(std::string string);

    public:
        template <typename T>
        static void getConfigByOption(std::string key, Options &opts, T &ref)
        {
            ref = Options::get<T>(opts, key, ref);
        }
        static void init(std::string file);

        static void load(std::string file, Options &opts);

    }; // end of class
};
