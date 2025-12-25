/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "util.h"
namespace fog
{
    struct Config
    {
    private:
        static inline float DEF_CELL_SCALE = 30.0f;
        //
        static inline float DEF_HEIGHT_OCEAN = 0.49f;
        static inline float DEF_HEIGHT_SHORE = 0.50f;
        static inline float DEF_HEIGHT_PLAIN = 0.51f;
        static inline float DEF_HEIGHT_HILL = 0.52f;
        static inline float DEF_HEIGHT_MOUNTAIN = 0.53f;
        static inline float DEF_HEIGHT_FROZEN = 0.54f;
        static inline float DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN = 0.05; //
        static inline int DEF_SHADER_SHOW_CELL_EDGE = 0;
        static inline int DEF_SHADER_SHOW_REGION_EDGE = 0;
        static inline Box2<int> DEF_FOG_OF_WAR_TEX_RANGE = Box2<int>(32);
        static inline std::string DEF_FOG_OF_WAR_TEX_NAME = "FogOfWarTex";
        static inline Box2<int> DEF_FOG_OF_WAR_EREASE_RANGE = Box2<int>(0);
        static inline float DEF_STATE_MOVED_EVENT_DISTNACE = DEF_CELL_SCALE / 2.0; //
        // debug
        static inline bool DEF_DEBUG_COUT = false;
        static inline bool DEF_DEBUG_MOVING_POSITION = false;
        static inline bool DEF_DEBUG_FOG_OF_WAR = false;
        static inline Box2<int> DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE = Box2<int>(0, 0, 0, 0);
        static inline int DEF_CELLS_TERRAIN_AMP = 1;
        static inline int DEF_TILE_MESH_QUALITY = 8;
        static inline float DEF_HILL_PEAK_DISTRIBUTION = 0.01f;
        static inline float DEF_MOUNTAIN_PEAK_DISTRIBUTION = 0.01f;
        static inline float DEF_frozenDistribution = 0.1f;
        static inline float DEF_normalDistribution = 0.75;
        static inline float DEF_hotDistribution = 0.95;
        static inline float DEF_temperatureLatitudeWeightPower = 3.0f;
        static inline bool DEF_makeMountainRange = true;
        static inline bool DEF_DEBUG_polygonMode_wireFrame = false;

    public:
        float cellScale = DEF_CELL_SCALE;
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

        float GENERATOR1_RATIO_LAKE_OF_PLAIN = DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN;
        // fragment
        int debugShaderShowCellEdge = DEF_SHADER_SHOW_CELL_EDGE;
        int debugShaderShowRegionEdge = DEF_SHADER_SHOW_REGION_EDGE;
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

    public:
        
        SELFG(Config, "config")
        MEMBERKD(GENERATOR1_RATIO_LAKE_OF_PLAIN, "GENERATOR1_RATIO_LAKE_OF_PLAIN", DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN)
        MEMBERKD(heightOfOcean, "HEIGHT_OCEAN", DEF_HEIGHT_OCEAN)
        MEMBERKD(heightOfShore, "HEIGHT_SHORE", DEF_HEIGHT_SHORE)
        MEMBERKD(heightOfPlain, "HEIGHT_PLAIN", DEF_HEIGHT_PLAIN)
        MEMBERKD(heightOfHill, "HEIGHT_HILL", DEF_HEIGHT_HILL)
        MEMBERKD(heightOfMountain, "HEIGHT_MOUNTAIN", DEF_HEIGHT_MOUNTAIN)
        MEMBERKD(heightOfFrozen, "HEIGHT_FROZEN", DEF_HEIGHT_FROZEN)
        MEMBERKD(heightAmpOfHill, "HEIGHT_AMP_OF_HILL", 1.2f) //
        MEMBERKD(heightAmpOfMountain, "HEIGHT_AMP_OF_MOUNTAIN", 1.5f)
        MEMBERKD(debugShaderShowCellEdge, "SHADER_SHOW_CELL_EDGE", DEF_SHADER_SHOW_CELL_EDGE)
        MEMBERKD(debugShaderShowRegionEdge, "SHADER_SHOW_REGION_EDGE", DEF_SHADER_SHOW_REGION_EDGE)

        MEMBERKD(actorMovedEventDistance, "STATE_MOVED_EVENT_DISTNACE", DEF_STATE_MOVED_EVENT_DISTNACE)
        MEMBERKD(debugCout, "DEBUG_COUT", DEF_DEBUG_COUT)
        MEMBERKD(debugMovingPosition, "DEBUG_MOVING_POSITION", DEF_DEBUG_MOVING_POSITION)
        MEMBERKD(debugFogOfWar, "DEBUG_FOG_OF_WAR", DEF_DEBUG_FOG_OF_WAR)
        MEMBERKD(hillPeakDistribution, "HILL_PEAK_DISTRIBUTION", DEF_HILL_PEAK_DISTRIBUTION)
        MEMBERKD(mountainPeakDistribution, "MOUNTAIN_PEAK_DISTRIBUTION", DEF_MOUNTAIN_PEAK_DISTRIBUTION)
        MEMBERKD(makeMountainRange, "makeMountainRange", DEF_makeMountainRange)
        MEMBERKD(frozenDistribution, "frozenDistribution", DEF_frozenDistribution)
        MEMBERKD(normalDistribution, "normalDistribution", DEF_normalDistribution)
        MEMBERKD(hotDistribution, "hotDistribution", DEF_hotDistribution)

        MEMBERKD(temperatureLatitudeWeightPower, "temperatureLatitudeWeightPower", DEF_temperatureLatitudeWeightPower)
        MEMBERKD(DEBUG_polygonMode_wireFrame, "DEBUG_polygonMode_wireFrame", DEF_DEBUG_polygonMode_wireFrame)

        MEMBERKD(debugPrintTerrainsTexRange, "DEBUG_PRINT_TERRAINS_TEX_RANGE", DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE)
        MEMBERKD(fogOfWarTextRange, "FOG_OF_WAR_TEX_RANGE", DEF_FOG_OF_WAR_TEX_RANGE)
        MEMBERKD(fogOfWarEreaseRange, "FOG_OF_WAR_EREASE_RANGE", DEF_FOG_OF_WAR_EREASE_RANGE)

        INJECT(Config())
        {
        }

        int getShaderDebug()
        {
            return this->debugShaderShowCellEdge + this->debugShaderShowRegionEdge;
        }

    }; // end of class
};
