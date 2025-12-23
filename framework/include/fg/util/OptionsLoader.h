/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <fstream>
#include <filesystem>
#include "Options.h"
#include "Box2.h"
namespace fog
{

    struct OptionsLoader
    {

        void load(std::string file, Options & opts, std::string group, bool strict){
            std::unordered_map<std::string, Options> optsMap;
            load(file, optsMap, strict);
            opts.merge(optsMap[group]);
        }
        
        void load(std::string file, std::unordered_map<std::string, Options> &optMap, bool strict)
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
            std::string group;
            while (std::getline(f, line))
            {
                lNum++;

                // skip space
                while (!line.empty() && line[0] == ' ')
                {
                    line = line.substr(1);
                }

                if (line.empty() || line[0] == '#' || line[0] == '/' && line.length() > 1 && line[1] == '/')
                {
                    continue;
                }
                
                if (line[0] == '[')
                {
                    // process group
                    auto b2 = line.find(']');
                    if (b2 == std::string::npos)
                    {
                        throw std::runtime_error("group format error.");
                    }
                    group = line.substr(1, b2 - 1);
                }
                
                Options & opts = optMap[group];

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
                    Box2<int> v = parseValueOfRange2Int(value);
                    ok = opts.tryAdd<Box2<int>>(key, v);
                }
                else if (type == "unsigned int")
                {
                    ok = opts.tryAdd<unsigned int>(key, static_cast<unsigned int>(std::stoul(value)));
                }
                else
                {
                    throw std::runtime_error(std::string("not supported type:") + type);
                }
                if (!ok && strict)
                {
                    throw std::runtime_error(std::string("options loading is in strict mode and item already exists:") + key);
                }

            } // end while.

        } // end load
    private:
        Box2<int> parseValueOfRange2Int(std::string string)
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

};
