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
    private:
        static float DEF_HEIGHT_SCALE;
        static Box2<int> DEF_TILES_RANGE;
        static int DEF_TILE_TERRAIN_QUALITY;
        static int DEF_TILE_MESH_QUALITY;
        static float DEF_CELL_SCALE;
        static float DEF_WORLD_WIDTH;     // = CELL_SCALE * 2.0 * TILES_WIDTH;
        static float DEF_WORLD_HEIGHT;    // = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
                                          //
        static float DEF_HEIGHT_OCEAN;    // = 0.49f * 0.9f;
        static float DEF_HEIGHT_SHORE;    // = 0.50f * 0.9f;
        static float DEF_HEIGHT_PLAIN;    // = 0.51f * 1.1f;
        static float DEF_HEIGHT_HILL;     // = 0.52f * 1.2f;
        static float DEF_HEIGHT_MOUNTAIN; // = 0.53f * 1.f;
        static float DEF_HEIGHT_FROZEN;   // = 0.54f * 1.f;
        // generator
        static int DEF_GENERATOR1_SEED;
        static int DEF_GENERATOR2_SEED;
        static float DEF_GENERATOR1_ROUGHNESS;
        static float DEF_GENERATOR2_ROUGHNESS;
        static float DEF_GENERATOR1_OCEAN_RATIO;
        static float DEF_GENERATOR1_SHORE_RATIO;
        static float DEF_GENERATOR1_PLAIN_RATIO;
        static float DEF_GENERATOR1_HILL_RATIO;
        static float DEF_GENERATOR1_MOUNTAIN_RATIO;
        static float DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN;
        static bool DEF_GENERATOR1_MAKE_LAKE;
        // fragment
        static bool DEF_FRAG_SHOW_EDGE;
        // fog of war
        static Box2<int> DEF_FOG_OF_WAR_TEX_RANGE;
        static std::string DEF_FOG_OF_WAR_TEX_NAME;
        static Box2<int> DEF_FOG_OF_WAR_EREASE_RANGE;
        static float DEF_STATE_MOVED_EVENT_DISTNACE;
        // debug
        static Box2<int> DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE;
        static bool DEF_DEBUG_COUT;
        static bool DEF_DEBUG_MOVING_POSITION;
        static bool DEF_DEBUG_FOG_OF_WAR;

    private:
        static Box2<int> Config::parseValueOfRange2Int(std::string string);

    public:
        struct Args
        {
            std::vector<std::string> files;
            Args(std::vector<std::string> files) : files(files)
            {
            }
        };

        template <typename T>
        static T getConfigByOption(std::string key, Options &opts, T def)
        {
            return Options::get<T>(opts, key, def);
        }

        static void load(std::string file, Options &opts, bool strict);

        INJECT(Config(Args arg))
        {

            Options opts;

            for (auto it = arg.files.begin(); it != arg.files.end(); it++)
            {
                load(*it, opts, false);
            }

            HEIGHT_SCALE = Options::get<float>(opts, "HEIGHT_SCALE", DEF_HEIGHT_SCALE);
            TILES_RANGE = Options::get<Box2<int>>(opts, "TILES_RANGE", DEF_TILES_RANGE);

            TILE_TERRAIN_QUALITY = Options::get<int>(opts, "TILE_TERRAIN_QUALITY", DEF_TILE_TERRAIN_QUALITY);
            TILE_MESH_QUALITY = Options::get<int>(opts, "TILE_MESH_QUALITY", DEF_TILE_MESH_QUALITY);
            if (TILE_MESH_QUALITY > TILE_TERRAIN_QUALITY)
            {
                TILE_MESH_QUALITY = TILE_TERRAIN_QUALITY;
            }
            CELL_SCALE = Options::get<float>(opts, "CELL_SCALE", DEF_CELL_SCALE);

            WORLD_WIDTH = CELL_SCALE * 2.0 * TILES_RANGE.getWidth();
            WORLD_HEIGHT = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
            //

            HEIGHT_OCEAN = Options::get<float>(opts, "HEIGHT_OCEAN", DEF_HEIGHT_OCEAN);
            HEIGHT_SHORE = Options::get<float>(opts, "HEIGHT_SHORE", DEF_HEIGHT_SHORE);
            HEIGHT_PLAIN = Options::get<float>(opts, "HEIGHT_PLAIN", DEF_HEIGHT_PLAIN);
            HEIGHT_HILL = Options::get<float>(opts, "HEIGHT_HILL", DEF_HEIGHT_HILL);
            HEIGHT_MOUNTAIN = Options::get<float>(opts, "HEIGHT_MOUNTAIN", DEF_HEIGHT_MOUNTAIN);
            HEIGHT_FROZEN = Options::get<float>(opts, "HEIGHT_FROZEN", DEF_HEIGHT_FROZEN);
            //
            GENERATOR1_SEED = getConfigByOption<int>("GENERATOR1_SEED", opts, DEF_GENERATOR1_SEED);
            GENERATOR2_SEED = getConfigByOption<int>("GENERATOR2_SEED", opts, DEF_GENERATOR2_SEED);
            GENERATOR1_OCEAN_RATIO = getConfigByOption<float>("GENERATOR1_OCEAN_RATIO", opts, DEF_GENERATOR1_OCEAN_RATIO);
            GENERATOR1_SHORE_RATIO = getConfigByOption<float>("GENERATOR1_SHORE_RATIO", opts, DEF_GENERATOR1_SHORE_RATIO);
            GENERATOR1_PLAIN_RATIO = getConfigByOption<float>("GENERATOR1_PLAIN_RATIO", opts, DEF_GENERATOR1_PLAIN_RATIO);
            GENERATOR1_HILL_RATIO = getConfigByOption<float>("GENERATOR1_HILL_RATIO", opts, DEF_GENERATOR1_HILL_RATIO);
            GENERATOR1_MOUNTAIN_RATIO = getConfigByOption<float>("GENERATOR1_MOUNTAIN_RATIO", opts, DEF_GENERATOR1_MOUNTAIN_RATIO);
            GENERATOR1_RATIO_LAKE_OF_PLAIN = getConfigByOption<float>("GENERATOR1_RATIO_LAKE_OF_PLAIN", opts, DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN);
            GENERATOR1_MAKE_LAKE = getConfigByOption<bool>("GENERATOR1_MAKE_LAKE", opts, DEF_GENERATOR1_MAKE_LAKE);
            DEBUG_PRINT_TERRAINS_TEX_RANGE = getConfigByOption<Box2<int>>("DEBUG_PRINT_TERRAINS_TEX_RANGE", opts, DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE);
            FRAG_SHOW_EDGE = getConfigByOption<bool>("FRAG_SHOW_EDGE", opts, DEF_FRAG_SHOW_EDGE);
            FOG_OF_WAR_TEX_RANGE = getConfigByOption<Box2<int>>("FOG_OF_WAR_TEX_RANGE", opts, DEF_FOG_OF_WAR_TEX_RANGE);
            FOG_OF_WAR_EREASE_RANGE = getConfigByOption<Box2<int>>("FOG_OF_WAR_EREASE_RANGE", opts, DEF_FOG_OF_WAR_EREASE_RANGE);
            STATE_MOVED_EVENT_DISTNACE = getConfigByOption<float>("STATE_MOVED_EVENT_DISTNACE", opts, DEF_STATE_MOVED_EVENT_DISTNACE);
            DEBUG_COUT = getConfigByOption<bool>("DEBUG_COUT", opts, DEF_DEBUG_COUT);
            DEBUG_MOVING_POSITION = getConfigByOption<bool>("DEBUG_MOVING_POSITION", opts, DEF_DEBUG_MOVING_POSITION);
            DEBUG_FOG_OF_WAR = getConfigByOption<bool>("DEBUG_FOG_OF_WAR", opts, DEF_DEBUG_FOG_OF_WAR);

            // // transform
            // TF_CELL_SCALE = {CELL_SCALE};
            // CELLKEY_2_UV = {TILES_RANGE.getWidth(), TILES_RANGE.getHeight()};

            Context<Transform::D2H2D3>::set(new Transform::D2H2D3(CELL_SCALE, [](float x, float y) -> float
                                                                  {
                                                                      // see TilesTerrains.h
                                                                      return 0.0f; // TODO provide the right height function, instead of one for init and update it in another place.
                                                                  }));

            Context<Transform::D3_NORMAL_D2>::set(new Transform::D3_NORMAL_D2(1.0f / CELL_SCALE));

            // Config::D2H2D3 = {};
        }

        float HEIGHT_SCALE = DEF_HEIGHT_SCALE;
        Box2<int> TILES_RANGE = DEF_TILES_RANGE;
        int TILE_TERRAIN_QUALITY = DEF_TILE_TERRAIN_QUALITY;
        int TILE_MESH_QUALITY= DEF_TILE_MESH_QUALITY;
        float CELL_SCALE= DEF_CELL_SCALE;
        float WORLD_WIDTH= DEF_WORLD_WIDTH;     // = CELL_SCALE * 2.0 * TILES_WIDTH;
        float WORLD_HEIGHT = DEF_WORLD_HEIGHT;    // = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
                               //
        float HEIGHT_OCEAN = DEF_HEIGHT_OCEAN;    // = 0.49f * 0.9f;
        float HEIGHT_SHORE = DEF_HEIGHT_SHORE;    // = 0.50f * 0.9f;
        float HEIGHT_PLAIN= DEF_HEIGHT_PLAIN;    // = 0.51f * 1.1f;
        float HEIGHT_HILL = DEF_HEIGHT_HILL;     // = 0.52f * 1.2f;
        float HEIGHT_MOUNTAIN = DEF_HEIGHT_MOUNTAIN; // = 0.53f * 1.f;
        float HEIGHT_FROZEN = DEF_HEIGHT_FROZEN;   // = 0.54f * 1.f;
                               // generator
        int GENERATOR1_SEED = DEF_GENERATOR1_SEED;
        int GENERATOR2_SEED = DEF_GENERATOR2_SEED;
        float GENERATOR1_ROUGHNESS = DEF_GENERATOR1_ROUGHNESS;
        float GENERATOR2_ROUGHNESS = DEF_GENERATOR2_ROUGHNESS;
        float GENERATOR1_OCEAN_RATIO = DEF_GENERATOR1_OCEAN_RATIO;
        float GENERATOR1_SHORE_RATIO = DEF_GENERATOR1_SHORE_RATIO;
        float GENERATOR1_PLAIN_RATIO = DEF_GENERATOR1_PLAIN_RATIO;
        float GENERATOR1_HILL_RATIO = DEF_GENERATOR1_HILL_RATIO;
        float GENERATOR1_MOUNTAIN_RATIO = DEF_GENERATOR1_MOUNTAIN_RATIO;
        float GENERATOR1_RATIO_LAKE_OF_PLAIN = DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN;
        bool GENERATOR1_MAKE_LAKE = DEF_GENERATOR1_MAKE_LAKE;
        // fragment
        bool FRAG_SHOW_EDGE = DEF_FRAG_SHOW_EDGE;
        // fog of war
        Box2<int> FOG_OF_WAR_TEX_RANGE = DEF_FOG_OF_WAR_TEX_RANGE;
        std::string FOG_OF_WAR_TEX_NAME = DEF_FOG_OF_WAR_TEX_NAME;
        Box2<int> FOG_OF_WAR_EREASE_RANGE = DEF_FOG_OF_WAR_EREASE_RANGE;
        float STATE_MOVED_EVENT_DISTNACE = DEF_STATE_MOVED_EVENT_DISTNACE;
        // debug
        Box2<int> DEBUG_PRINT_TERRAINS_TEX_RANGE = DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE;
        bool DEBUG_COUT = DEF_DEBUG_COUT;
        bool DEBUG_MOVING_POSITION = DEF_DEBUG_MOVING_POSITION;
        bool DEBUG_FOG_OF_WAR = DEF_DEBUG_FOG_OF_WAR;

    }; // end of class
};
