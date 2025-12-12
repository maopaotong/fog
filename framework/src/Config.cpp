/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/Config.h"
#include "fg/util/Context.h"
#include <fstream>
namespace fog
{

    float Config::HEIGHT_SCALE = 100;
    Box2<int> Config::TILES_RANGE = Box2<int>(128 + 1);
    int Config::TILE_TERRAIN_QUALITY = 4;
    int Config::TILE_MESH_QUALITY = 2;
    float Config::CELL_SCALE = 30.0f;
    float Config::WORLD_WIDTH = CELL_SCALE * 2.0 * TILES_RANGE.getWidth();
    float Config::WORLD_HEIGHT = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378

    //
    float Config::HEIGHT_OCEAN = 0.49f * 0.9f;
    float Config::HEIGHT_SHORE = 0.50f * 0.9f;
    float Config::HEIGHT_PLAIN = 0.51f * 1.1f;
    float Config::HEIGHT_HILL = 0.52f * 1.2f;
    float Config::HEIGHT_MOUNTAIN = 0.53f * 1.f;
    float Config::HEIGHT_FROZEN = 0.54f * 1.f;
    // map generator.
    int Config::GENERATOR1_SEED = 8151245;
    int Config::GENERATOR2_SEED = 2359812;
    float Config::GENERATOR1_ROUGHNESS = 0.45;
    float Config::GENERATOR2_ROUGHNESS = 0.65;
    float Config::GENERATOR1_OCEAN_RATIO = 0.35;
    float Config::GENERATOR1_SHORE_RATIO = 0.50;
    float Config::GENERATOR1_PLAIN_RATIO = 0.85;
    float Config::GENERATOR1_HILL_RATIO = 0.90;
    float Config::GENERATOR1_MOUNTAIN_RATIO = 0.95;
    float Config ::GENERATOR1_RATIO_LAKE_OF_PLAIN = 0.05; //
    bool Config::GENERATOR1_MAKE_LAKE = false;
    bool Config::FRAG_SHOW_EDGE = false;
    Box2<int> Config::FOG_OF_WAR_TEX_RANGE = Box2<int>(32);

    Box2<int> Config::FOG_OF_WAR_EREASE_RANGE = Box2<int>(0);
    float Config::STATE_MOVED_EVENT_DISTNACE = CELL_SCALE / 2.0; //
    // debug
    bool Config::DEBUG_COUT = false;
    bool Config::DEBUG_MOVING_POSITION = false;
    bool Config::DEBUG_FOG_OF_WAR = false;

    Box2<int> Config::DEBUG_PRINT_TERRAINS_TEX_RANGE = Box2<int>(0, 0, 0, 0);

    void Config::init(std::string file)
    {
        Options opts;
        load(file,opts);
        HEIGHT_SCALE = Options::get<float>(opts, "HEIGHT_SCALE", HEIGHT_SCALE);
        TILES_RANGE = Options::get<Box2<int>>(opts, "TILES_RANGE", TILES_RANGE);

        TILE_TERRAIN_QUALITY = Options::get<int>(opts, "TILE_TERRAIN_QUALITY", TILE_TERRAIN_QUALITY);
        TILE_MESH_QUALITY = Options::get<int>(opts, "TILE_MESH_QUALITY", TILE_MESH_QUALITY);
        if (TILE_MESH_QUALITY > TILE_TERRAIN_QUALITY)
        {
            TILE_MESH_QUALITY = TILE_TERRAIN_QUALITY;
        }
        CELL_SCALE = Options::get<float>(opts, "CELL_SCALE", CELL_SCALE);

        WORLD_WIDTH = CELL_SCALE * 2.0 * TILES_RANGE.getWidth();
        WORLD_HEIGHT = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
        //

        HEIGHT_OCEAN = Options::get<float>(opts, "HEIGHT_OCEAN", HEIGHT_OCEAN);
        HEIGHT_SHORE = Options::get<float>(opts, "HEIGHT_SHORE", HEIGHT_SHORE);
        HEIGHT_PLAIN = Options::get<float>(opts, "HEIGHT_PLAIN", HEIGHT_PLAIN);
        HEIGHT_HILL = Options::get<float>(opts, "HEIGHT_HILL", HEIGHT_HILL);
        HEIGHT_MOUNTAIN = Options::get<float>(opts, "HEIGHT_MOUNTAIN", HEIGHT_MOUNTAIN);
        HEIGHT_FROZEN = Options::get<float>(opts, "HEIGHT_FROZEN", HEIGHT_FROZEN);
        //
        getConfigByOption<int>("GENERATOR1_SEED", opts, GENERATOR1_SEED);
        getConfigByOption<int>("GENERATOR2_SEED", opts, GENERATOR2_SEED);
        getConfigByOption<float>("GENERATOR1_OCEAN_RATIO", opts, GENERATOR1_OCEAN_RATIO);
        getConfigByOption<float>("GENERATOR1_SHORE_RATIO", opts, GENERATOR1_SHORE_RATIO);
        getConfigByOption<float>("GENERATOR1_PLAIN_RATIO", opts, GENERATOR1_PLAIN_RATIO);
        getConfigByOption<float>("GENERATOR1_HILL_RATIO", opts, GENERATOR1_HILL_RATIO);
        getConfigByOption<float>("GENERATOR1_MOUNTAIN_RATIO", opts, GENERATOR1_MOUNTAIN_RATIO);
        getConfigByOption<float>("GENERATOR1_RATIO_LAKE_OF_PLAIN", opts, GENERATOR1_RATIO_LAKE_OF_PLAIN);
        getConfigByOption<bool>("GENERATOR1_MAKE_LAKE", opts, GENERATOR1_MAKE_LAKE);
        getConfigByOption<Box2<int>>("DEBUG_PRINT_TERRAINS_TEX_RANGE", opts, DEBUG_PRINT_TERRAINS_TEX_RANGE);
        getConfigByOption<bool>("FRAG_SHOW_EDGE", opts, FRAG_SHOW_EDGE);
        getConfigByOption<Box2<int>>("FOG_OF_WAR_TEX_RANGE", opts, FOG_OF_WAR_TEX_RANGE);
        getConfigByOption<Box2<int>>("FOG_OF_WAR_EREASE_RANGE", opts, FOG_OF_WAR_EREASE_RANGE);
        getConfigByOption<float>("STATE_MOVED_EVENT_DISTNACE", opts, STATE_MOVED_EVENT_DISTNACE);
        getConfigByOption<bool>("DEBUG_COUT", opts, DEBUG_COUT);
        getConfigByOption<bool>("DEBUG_MOVING_POSITION", opts, DEBUG_MOVING_POSITION);
        getConfigByOption<bool>("DEBUG_FOG_OF_WAR", opts, DEBUG_FOG_OF_WAR);
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );
        // getConfigByOption<bool>("", opts, );

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

    void Config::load(std::string file, Options &opts)
    {
        std::ifstream f(file);
        std::string line;
        int lNum = 0;
        while (std::getline(f, line))
        {
            lNum++;

            if (line.empty() || line[0] == '#' || line[0] == '/' && line.length() > 1 && line[1] == '/')
            {
                continue;
            }
            auto eq = line.find('=');
            if (eq == std::string::npos)
            {
                // ignore
                continue;
            }
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            auto typeLeft = key.find("<");
            std::string type = "string";
            if (typeLeft != std::string::npos)
            {

                auto typeRight = key.find_last_of(">");

                if (typeRight == std::string::npos || typeRight != key.length() - 1)
                {
                    throw std::runtime_error(std::string("config format error for lineNum:") + std::to_string(lNum));
                }

                type = key.substr(typeLeft + 1, typeRight - typeLeft - 1);
                key = key.substr(0, typeLeft);
            }

            bool ok;
            if (type == "string")
            {
                ok = opts.tryAdd<std::string>(key, value);
            }
            else if (type == "float")
            {
                ok = opts.tryAdd<float>(key, std::stof(value));
            }
            else if (type == "int")
            {
                ok = opts.tryAdd<int>(key, std::stoi(value));
            }
            else if (type == "bool")
            {
                ok = opts.tryAdd<bool>(key, (value == "true" || value == "yes" || value == "Y" || value == "1"));
            }
            else if (type == "range2<int>")
            {
                Box2<int> v = Config::parseValueOfRange2Int(value);
                ok = opts.tryAdd<Box2<int>>(key, v);
            }
            else
            {
                throw std::runtime_error(std::string("not supported type:") + type);
            }
            if (!ok)
            {
                throw std::runtime_error(std::string("config key already exists."));
            }

        } // end while.

    } // end load

    Box2<int> Config::parseValueOfRange2Int(std::string string)
    {
        std::vector<int> xyxy;
        int p1 = 0;
        std::string str = string;
        while (str.length() > 0)
        {
            std::string vS;
            if (xyxy.size() < 3)
            {

                auto p2 = str.find_first_of(",");

                if (p2 == std::string::npos)
                {
                    vS = str;
                    str = ""; // empty string left
                }
                else
                {
                    vS = str.substr(0, p2);
                    str = str.substr(p2 + 1, str.length() - p2 - 1);
                }
            }
            else
            {
                vS = str;
                str = ""; // empty string left
            }

            int v = std::stoi(vS);
            xyxy.push_back(v);
        }

        if (xyxy.size() == 1)
        {
            return Box2<int>(xyxy[0]);
        }
        if (xyxy.size() == 2)
        {
            return Box2<int>(xyxy[0], xyxy[1]);
        }

        if (xyxy.size() == 3)
        {
            return Box2<int>(xyxy[0], xyxy[1], xyxy[2], xyxy[2]);
        }
        if (xyxy.size() == 4)
        {
            return Box2<int>(xyxy[0], xyxy[1], xyxy[2], xyxy[3]);
        }
        throw std::runtime_error("cannot parse value with type of range<int>");
    }
};
