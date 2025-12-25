/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util.h"
#include "fg/Config.h"
#include <fstream>
#include <filesystem>
namespace fog
{

    // float Config::DEF_HEIGHT_SCALE = 100;
    // Box2<int> Config::DEF_TILES_RANGE = Box2<int>(128 + 1);
    // int Config::DEF_CELLS_TERRAIN_AMP = 4;
    // int Config::DEF_TILE_MESH_QUALITY = 2;
    float Config::DEF_CELL_SCALE = 30.0f;
    // float Config::DEF_WORLD_WIDTH = DEF_CELL_SCALE * 2.0 * DEF_TILES_RANGE.getWidth();
    // float Config::DEF_WORLD_HEIGHT = DEF_WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378

    //
    float Config::DEF_HEIGHT_OCEAN = 0.49f;
    float Config::DEF_HEIGHT_SHORE = 0.50f;
    float Config::DEF_HEIGHT_PLAIN = 0.51f;
    float Config::DEF_HEIGHT_HILL = 0.52f;
    float Config::DEF_HEIGHT_MOUNTAIN = 0.53f;
    float Config::DEF_HEIGHT_FROZEN = 0.54f;
    // map generator.
    // int Config::DEF_GENERATOR1_SEED = 8151245;
    // int Config::DEF_GENERATOR2_SEED = 2359812;
    // float Config::DEF_GENERATOR2_ROUGHNESS = 0.65;
    // float Config::DEF_GENERATOR1_OCEAN_RATIO = 0.35;
    // float Config::DEF_GENERATOR1_SHORE_RATIO = 0.50;
    // float Config::DEF_GENERATOR1_PLAIN_RATIO = 0.85;
    // float Config::DEF_GENERATOR1_HILL_RATIO = 0.90;
    // float Config::DEF_GENERATOR1_MOUNTAIN_RATIO = 0.95;
    float Config::DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN = 0.05; //
    //bool Config::DEF_GENERATOR1_MAKE_LAKE = false;
    int Config::DEF_SHADER_SHOW_CELL_EDGE = 0;
    int Config::DEF_SHADER_SHOW_REGION_EDGE = 0;
    Box2<int> Config::DEF_FOG_OF_WAR_TEX_RANGE = Box2<int>(32);
    std::string Config::DEF_FOG_OF_WAR_TEX_NAME = "FogOfWarTex";

    Box2<int> Config::DEF_FOG_OF_WAR_EREASE_RANGE = Box2<int>(0);
    float Config::DEF_STATE_MOVED_EVENT_DISTNACE = DEF_CELL_SCALE / 2.0; //
    // debug
    bool Config::DEF_DEBUG_COUT = false;
    bool Config::DEF_DEBUG_MOVING_POSITION = false;
    bool Config::DEF_DEBUG_FOG_OF_WAR = false;

    Box2<int> Config::DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE = Box2<int>(0, 0, 0, 0);

    

    
};
