/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "util.h"
namespace fog
{
    class Config
    {
    private:
        static float DEF_HEIGHT_SCALE;
        static Box2<int> DEF_TILES_RANGE;
        static inline int DEF_CELLS_TERRAIN_AMP = 1;
        static inline int DEF_TILE_MESH_QUALITY = 8;
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
        inline static float DEF_GENERATOR1_ROUGHNESS = 0.45;
        ;
        static float DEF_GENERATOR2_ROUGHNESS;
        static float DEF_GENERATOR1_OCEAN_RATIO;
        static float DEF_GENERATOR1_SHORE_RATIO;
        static float DEF_GENERATOR1_PLAIN_RATIO;
        static float DEF_GENERATOR1_HILL_RATIO;
        static float DEF_GENERATOR1_MOUNTAIN_RATIO;
        static float DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN;
        static bool DEF_GENERATOR1_MAKE_LAKE;
        // fragment
        static int DEF_SHADER_SHOW_CELL_EDGE;
        static int DEF_SHADER_SHOW_REGION_EDGE;
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
        static inline float DEF_HILL_PEAK_DISTRIBUTION = 0.01f;
        static inline float DEF_MOUNTAIN_PEAK_DISTRIBUTION = 0.01f;
        static inline float DEF_frozenDistribution = 0.1f;
        static inline float DEF_normalDistribution = 0.75;
        static inline float DEF_hotDistribution = 0.95;
        static inline float DEF_temperatureLatitudeWeightPower = 3.0f;
        static inline bool DEF_makeMountainRange = true;
        static inline bool DEF_DEBUG_polygonMode_wireFrame = false;

    private:
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

        INJECT(Config(Args arg))
        {

            OptionsLoader loader;
            std::unordered_map<std::string, Options> optsMap;
            for (auto it = arg.files.begin(); it != arg.files.end(); it++)
            {
                loader.load(*it, optsMap, false);
            }

            Options &opts = optsMap["config"];

            heightScale = Options::get<float>(opts, "HEIGHT_SCALE", DEF_HEIGHT_SCALE);
            cellsRange = Options::get<Box2<int>>(opts, "TILES_RANGE", DEF_TILES_RANGE);

            cellsTerrainAmp = Options::get<int>(opts, "CELLS_TERRAIN_AMP", DEF_CELLS_TERRAIN_AMP);
            cellsMeshQuality = Options::get<int>(opts, "TILE_MESH_QUALITY", DEF_TILE_MESH_QUALITY);

            cellScale = Options::get<float>(opts, "CELL_SCALE", DEF_CELL_SCALE);

            worldWidth = cellScale * 2.0 * cellsRange.getWidth();
            worldHeight = worldWidth * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
            //

            heightOfOcean = Options::get<float>(opts, "HEIGHT_OCEAN", DEF_HEIGHT_OCEAN);
            heightOfShore = Options::get<float>(opts, "HEIGHT_SHORE", DEF_HEIGHT_SHORE);
            heightOfPlain = Options::get<float>(opts, "HEIGHT_PLAIN", DEF_HEIGHT_PLAIN);
            heightOfHill = Options::get<float>(opts, "HEIGHT_HILL", DEF_HEIGHT_HILL);
            heightOfMountain = Options::get<float>(opts, "HEIGHT_MOUNTAIN", DEF_HEIGHT_MOUNTAIN);
            heightOfFrozen = Options::get<float>(opts, "HEIGHT_FROZEN", DEF_HEIGHT_FROZEN);
            // height scale of hill & mountain.

            heightAmpOfHill = getConfigByOption<float>("HEIGHT_AMP_OF_HILL", opts, 1.2f); //
            heightAmpOfMountain = getConfigByOption<float>("HEIGHT_AMP_OF_MOUNTAIN", opts, 1.5f);
            //
            seedOfGenerator1 = getConfigByOption<int>("GENERATOR1_SEED", opts, DEF_GENERATOR1_SEED);
            seedOfGenerator2 = getConfigByOption<int>("GENERATOR2_SEED", opts, DEF_GENERATOR2_SEED);

            generatorRoughness1 = getConfigByOption<float>("GENERATOR1_ROUGHNESS", opts, DEF_GENERATOR1_ROUGHNESS);
            generatorRoughness2 = getConfigByOption<float>("GENERATOR2_ROUGHNESS", opts, DEF_GENERATOR2_ROUGHNESS);

            GENERATOR1_OCEAN_RATIO = getConfigByOption<float>("GENERATOR1_OCEAN_RATIO", opts, DEF_GENERATOR1_OCEAN_RATIO);
            GENERATOR1_SHORE_RATIO = getConfigByOption<float>("GENERATOR1_SHORE_RATIO", opts, DEF_GENERATOR1_SHORE_RATIO);
            GENERATOR1_PLAIN_RATIO = getConfigByOption<float>("GENERATOR1_PLAIN_RATIO", opts, DEF_GENERATOR1_PLAIN_RATIO);
            GENERATOR1_HILL_RATIO = getConfigByOption<float>("GENERATOR1_HILL_RATIO", opts, DEF_GENERATOR1_HILL_RATIO);
            GENERATOR1_MOUNTAIN_RATIO = getConfigByOption<float>("GENERATOR1_MOUNTAIN_RATIO", opts, DEF_GENERATOR1_MOUNTAIN_RATIO);
            GENERATOR1_RATIO_LAKE_OF_PLAIN = getConfigByOption<float>("GENERATOR1_RATIO_LAKE_OF_PLAIN", opts, DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN);
            GENERATOR1_MAKE_LAKE = getConfigByOption<bool>("GENERATOR1_MAKE_LAKE", opts, DEF_GENERATOR1_MAKE_LAKE);
            debugPrintTerrainsTexRange = getConfigByOption<Box2<int>>("DEBUG_PRINT_TERRAINS_TEX_RANGE", opts, DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE);
            debugShaderShowCellEdge = getConfigByOption<int>("SHADER_SHOW_CELL_EDGE", opts, DEF_SHADER_SHOW_CELL_EDGE);
            debugShaderShowRegionEdge = getConfigByOption<int>("SHADER_SHOW_REGION_EDGE", opts, DEF_SHADER_SHOW_REGION_EDGE);

            fogOfWarTextRange = getConfigByOption<Box2<int>>("FOG_OF_WAR_TEX_RANGE", opts, DEF_FOG_OF_WAR_TEX_RANGE);
            fogOfWarEreaseRange = getConfigByOption<Box2<int>>("FOG_OF_WAR_EREASE_RANGE", opts, DEF_FOG_OF_WAR_EREASE_RANGE);
            actorMovedEventDistance = getConfigByOption<float>("STATE_MOVED_EVENT_DISTNACE", opts, DEF_STATE_MOVED_EVENT_DISTNACE);
            debugCout = getConfigByOption<bool>("DEBUG_COUT", opts, DEF_DEBUG_COUT);
            debugMovingPosition = getConfigByOption<bool>("DEBUG_MOVING_POSITION", opts, DEF_DEBUG_MOVING_POSITION);
            debugFogOfWar = getConfigByOption<bool>("DEBUG_FOG_OF_WAR", opts, DEF_DEBUG_FOG_OF_WAR);
            hillPeakDistribution = getConfigByOption<float>("HILL_PEAK_DISTRIBUTION", opts, DEF_HILL_PEAK_DISTRIBUTION);
            mountainPeakDistribution = getConfigByOption<float>("MOUNTAIN_PEAK_DISTRIBUTION", opts, DEF_MOUNTAIN_PEAK_DISTRIBUTION);
            makeMountainRange = getConfigByOption<bool>("makeMountainRange", opts, DEF_makeMountainRange);
            frozenDistribution = getConfigByOption<float>("frozenDistribution", opts, DEF_frozenDistribution);
            normalDistribution = getConfigByOption<float>("normalDistribution", opts, DEF_normalDistribution);
            hotDistribution = getConfigByOption<float>("hotDistribution", opts, DEF_hotDistribution);

            temperatureLatitudeWeightPower = getConfigByOption<float>("temperatureLatitudeWeightPower", opts, DEF_temperatureLatitudeWeightPower);
            DEBUG_polygonMode_wireFrame = getConfigByOption<bool>("DEBUG_polygonMode_wireFrame", opts, DEF_DEBUG_polygonMode_wireFrame);

            // // transform
            // TF_CELL_SCALE = {CELL_SCALE};
            // CELLKEY_2_UV = {TILES_RANGE.getWidth(), TILES_RANGE.getHeight()};

            // transformFromD2HToD3Ptr = new Transform::D2H2D3(cellScale, [](float x, float y) -> float
            //                                                 {
            //                                                     // see TilesTerrains.h
            //                                                     return 0.0f; // TODO provide the right height function, instead of one for init and update it in another place.
            //                                                 });
            // Context<Transform::D2H2D3>::set();

            // Context<Transform::D3_NORMAL_D2>::set();
            // transformD3NormalToD2Ptr = new Transform::D3_NORMAL_D2(1.0f / cellScale);

            // Config::D2H2D3 = {};
        }

        int getTerrainQuality()
        {
            return cellsTerrainAmp * this->cellsMeshQuality;
        }
        int getShaderDebug()
        {
            return this->debugShaderShowCellEdge + this->debugShaderShowRegionEdge;
        }

        // Transform::D3_NORMAL_D2 *transformD3NormalToD2Ptr = nullptr;
        // Transform::D3_NORMAL_D2 &getTransformD3NormalToD2()
        // {
        //     return *this->transformD3NormalToD2Ptr;
        // }

        // Transform::D2H2D3 *transformFromD2HToD3Ptr = nullptr;
        // Transform::D2H2D3 &getTransformFromD2HToD3()
        // {
        //     return *transformFromD2HToD3Ptr;
        // }

        float heightScale = DEF_HEIGHT_SCALE;
        Box2<int> cellsRange = DEF_TILES_RANGE;
        int cellsTerrainAmp = DEF_CELLS_TERRAIN_AMP;
        int cellsMeshQuality = DEF_TILE_MESH_QUALITY;
        float cellScale = DEF_CELL_SCALE;
        float worldWidth = DEF_WORLD_WIDTH;           // = CELL_SCALE * 2.0 * TILES_WIDTH;
        float worldHeight = DEF_WORLD_HEIGHT;         // = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
                                                      //
        float heightOfOcean = DEF_HEIGHT_OCEAN;       // = 0.49f * 0.9f;
        float heightOfShore = DEF_HEIGHT_SHORE;       // = 0.50f * 0.9f;
        float heightOfPlain = DEF_HEIGHT_PLAIN;       // = 0.51f * 1.1f;
        float heightOfHill = DEF_HEIGHT_HILL;         // = 0.52f * 1.2f;
        float heightOfMountain = DEF_HEIGHT_MOUNTAIN; // = 0.53f * 1.f;
        float heightOfFrozen = DEF_HEIGHT_FROZEN;     // = 0.54f * 1.f;
        float heightAmpOfHill = 0.2f;                 //
        float heightAmpOfMountain = 0.5f;
        float hillPeakDistribution = 0.1f;
        float mountainPeakDistribution = 0.1f;
        bool makeMountainRange = true;

        // generator
        int seedOfGenerator1 = DEF_GENERATOR1_SEED;
        int seedOfGenerator2 = DEF_GENERATOR2_SEED;
        float generatorRoughness1 = DEF_GENERATOR1_ROUGHNESS;
        float generatorRoughness2 = DEF_GENERATOR2_ROUGHNESS;
        float GENERATOR1_OCEAN_RATIO = DEF_GENERATOR1_OCEAN_RATIO;
        float GENERATOR1_SHORE_RATIO = DEF_GENERATOR1_SHORE_RATIO;
        float GENERATOR1_PLAIN_RATIO = DEF_GENERATOR1_PLAIN_RATIO;
        float GENERATOR1_HILL_RATIO = DEF_GENERATOR1_HILL_RATIO;
        float GENERATOR1_MOUNTAIN_RATIO = DEF_GENERATOR1_MOUNTAIN_RATIO;
        float GENERATOR1_RATIO_LAKE_OF_PLAIN = DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN;
        bool GENERATOR1_MAKE_LAKE = DEF_GENERATOR1_MAKE_LAKE;
        // fragment
        unsigned int debugShaderShowCellEdge = DEF_SHADER_SHOW_CELL_EDGE;
        unsigned int debugShaderShowRegionEdge = DEF_SHADER_SHOW_REGION_EDGE;
        // fog of war
        Box2<int> fogOfWarTextRange = DEF_FOG_OF_WAR_TEX_RANGE;
        std::string FOG_OF_WAR_TEX_NAME = DEF_FOG_OF_WAR_TEX_NAME;
        Box2<int> fogOfWarEreaseRange = DEF_FOG_OF_WAR_EREASE_RANGE;
        float actorMovedEventDistance = DEF_STATE_MOVED_EVENT_DISTNACE;
        // debug
        Box2<int> debugPrintTerrainsTexRange = DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE;
        bool debugCout = DEF_DEBUG_COUT;
        bool debugMovingPosition = DEF_DEBUG_MOVING_POSITION;
        bool debugFogOfWar = DEF_DEBUG_FOG_OF_WAR;
        float frozenDistribution = DEF_frozenDistribution;
        float normalDistribution = DEF_normalDistribution;
        float hotDistribution = DEF_hotDistribution;
        float temperatureLatitudeWeightPower = DEF_temperatureLatitudeWeightPower;
        bool DEBUG_polygonMode_wireFrame = DEF_DEBUG_polygonMode_wireFrame;

    }; // end of class
};
