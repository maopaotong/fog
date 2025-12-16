/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/Config.h"
#include "fg/util/Context.h"
#include <fstream>
#include <filesystem>
namespace fog
{

    float Config::DEF_HEIGHT_SCALE = 100;
    Box2<int> Config::DEF_TILES_RANGE = Box2<int>(128 + 1);
    int Config::DEF_TILE_TERRAIN_QUALITY = 4;
    int Config::DEF_TILE_MESH_QUALITY = 2;
    float Config::DEF_CELL_SCALE = 30.0f;
    float Config::DEF_WORLD_WIDTH = DEF_CELL_SCALE * 2.0 * DEF_TILES_RANGE.getWidth();
    float Config::DEF_WORLD_HEIGHT = DEF_WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378

    //
    float Config::DEF_HEIGHT_OCEAN = 0.49f * 0.9f;
    float Config::DEF_HEIGHT_SHORE = 0.50f * 0.9f;
    float Config::DEF_HEIGHT_PLAIN = 0.51f * 1.1f;
    float Config::DEF_HEIGHT_HILL = 0.52f * 1.2f;
    float Config::DEF_HEIGHT_MOUNTAIN = 0.53f * 1.f;
    float Config::DEF_HEIGHT_FROZEN = 0.54f * 1.f;
    // map generator.
    int Config::DEF_GENERATOR1_SEED = 8151245;
    int Config::DEF_GENERATOR2_SEED = 2359812;
    float Config::DEF_GENERATOR1_ROUGHNESS = 0.45;
    float Config::DEF_GENERATOR2_ROUGHNESS = 0.65;
    float Config::DEF_GENERATOR1_OCEAN_RATIO = 0.35;
    float Config::DEF_GENERATOR1_SHORE_RATIO = 0.50;
    float Config::DEF_GENERATOR1_PLAIN_RATIO = 0.85;
    float Config::DEF_GENERATOR1_HILL_RATIO = 0.90;
    float Config::DEF_GENERATOR1_MOUNTAIN_RATIO = 0.95;
    float Config::DEF_GENERATOR1_RATIO_LAKE_OF_PLAIN = 0.05; //
    bool Config::DEF_GENERATOR1_MAKE_LAKE = false;
    bool Config::DEF_FRAG_SHOW_EDGE = false;
    Box2<int> Config::DEF_FOG_OF_WAR_TEX_RANGE = Box2<int>(32);
    std::string Config::DEF_FOG_OF_WAR_TEX_NAME = "FogOfWarTex";

    Box2<int> Config::DEF_FOG_OF_WAR_EREASE_RANGE = Box2<int>(0);
    float Config::DEF_STATE_MOVED_EVENT_DISTNACE = DEF_CELL_SCALE / 2.0; //
    // debug
    bool Config::DEF_DEBUG_COUT = false;
    bool Config::DEF_DEBUG_MOVING_POSITION = false;
    bool Config::DEF_DEBUG_FOG_OF_WAR = false;

    Box2<int> Config::DEF_DEBUG_PRINT_TERRAINS_TEX_RANGE = Box2<int>(0, 0, 0, 0);

    

    void Config::load(std::string file, Options &opts, bool strict)
    {
        std::filesystem::path fpath(file.c_str());
        if (!std::filesystem::exists(fpath))
        {
            throw std::runtime_error(std::string("no such file:" + file));
        }
        std::ifstream f(fpath);

        if (!f.is_open())
        {
            throw std::runtime_error("failed to open file for read: " + file);
        }
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
            if (!ok && strict)
            {
                throw std::runtime_error(std::string("configration loading is in strict mode and item already exists:") + key);
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
