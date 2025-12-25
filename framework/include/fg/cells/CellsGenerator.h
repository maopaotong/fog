/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/ogre.h"

#include "fg/ogre.h"
#include "fg/util.h"
#include "Common.h"
#include "TemperatureGenerator.h"
namespace fog
{

    using CellType = uint8;
    /**
     * Generator of tiles with types.
     */
    struct CellsGenerator
    {

        struct Args
        {

            float frozenDistribution;             //, 0.1f)
            float temperatureLatitudeWeightPower; //, 3.0f)
            float normalDistribution;             //,  0.75f)
            float hotDistribution;                //,  0.95f)
            bool GENERATOR1_MAKE_LAKE;
            float generatorRoughness1;
            int seedOfGenerator1;
            float generatorRoughness2;
            int seedOfGenerator2;
            float GENERATOR1_OCEAN_RATIO;
            float GENERATOR1_SHORE_RATIO;
            float GENERATOR1_PLAIN_RATIO;
            float GENERATOR1_HILL_RATIO;

            SELF(Args)
            // GROUP("cellsgenerator#args")
            GROUP("config")
            MEMBERKD(frozenDistribution, "frozenDistribution", 0.1f)
            MEMBERKD(temperatureLatitudeWeightPower, "temperatureLatitudeWeightPower", 3.0f)
            MEMBERKD(normalDistribution, "normalDistribution", 0.75f)
            MEMBERKD(hotDistribution, "hotDistribution", 0.95f)
            MEMBERKD(GENERATOR1_MAKE_LAKE, "GENERATOR1_MAKE_LAKE", true)
            MEMBERKD(generatorRoughness1, "GENERATOR1_ROUGHNESS", 0.45)
            MEMBERKD(generatorRoughness2, "GENERATOR2_ROUGHNESS", 0.65)
            MEMBERKD(seedOfGenerator1, "GENERATOR1_SEED", 8151245)
            MEMBERKD(seedOfGenerator2, "GENERATOR2_SEED", 2359812)

            MEMBERKD(GENERATOR1_OCEAN_RATIO, "GENERATOR1_OCEAN_RATIO", 0.35f)
            MEMBERKD(GENERATOR1_SHORE_RATIO, "GENERATOR1_SHORE_RATIO", 0.50f)
            MEMBERKD(GENERATOR1_PLAIN_RATIO, "GENERATOR1_PLAIN_RATIO", 0.85f)
            MEMBERKD(GENERATOR1_HILL_RATIO, "GENERATOR1_HILL_RATIO", 0.90f)

            INJECT(Args())
            // : frozenDistribution(config->frozenDistribution), temperatureLatitudeWeightPower(config->temperatureLatitudeWeightPower),
            //                                   normalDistribution(config->normalDistribution),
            //                                   hotDistribution(config->hotDistribution)

            {
            }
        };
        struct GenerateOpCtx
        {
            std::vector<std::vector<CellData>> &tiles;
            int w;
            int h;
            std::vector<std::vector<float>> &heightmap;
            std::vector<std::vector<float>> &tpMap;
        };

        Args opts;
        INJECT(CellsGenerator(Args opts)) : opts(opts)
        {
        }

        virtual void generateCells(std::vector<std::vector<CellData>> &tiles, int w, int h)
        {
            assert(w == h && "cannot generate tiles because w<>h.");
            Iteration::forEachAsTable<std::vector<std::vector<CellData>> &>(w, h, //
                                                                            [](int row, std::vector<std::vector<CellData>> &tiles)
                                                                            { tiles.push_back(std::vector<CellData>()); }, //
                                                                            [](int row, int col, std::vector<std::vector<CellData>> &tiles)
                                                                            { tiles[row].emplace_back(); }, //
                                                                            tiles                           //
            );
            std::vector<std::vector<float>> heightmap(w, std::vector<float>(w, 0.0f));
            std::vector<std::vector<float>> tpMap(w, std::vector<float>(h, 0.0f));

            GenerateOpCtx goc{tiles, w, h, heightmap, tpMap};

            generateHeightmap(goc);

            generateTemperaturemap(goc);

            generateCellsTypes(goc);
        };

        virtual void generateHeightmap(GenerateOpCtx &goc)
        {
            DiamondSquare::generate(goc.heightmap, goc.w, opts.generatorRoughness1, opts.seedOfGenerator1);
            Normaliser::normalise(goc.heightmap, goc.w, goc.h);
        }

        virtual void generateTemperaturemap(GenerateOpCtx &goc)
        {
            TemperatureGenerator::generate(goc.heightmap, goc.tpMap, goc.w, goc.h, opts.temperatureLatitudeWeightPower);
            Normaliser::normalise(goc.tpMap, goc.w, goc.h);
        }

        virtual void generateCellsTypes(GenerateOpCtx &goc)
        {
            Iteration::forEach<float>(goc.heightmap, goc.w, goc.w, [this, &goc](int x, int y, float h)
                                      {
                                          CellType type = CellTypes::UNKNOW;
                                          if (h < opts.GENERATOR1_OCEAN_RATIO) // TODo calculate the actual ratio instead of the height.
                                          {
                                              type = CellTypes::OCEAN;
                                          }
                                          else if (h < opts.GENERATOR1_SHORE_RATIO)
                                          {
                                              type = CellTypes::SHORE;
                                          }
                                          else if (h < opts.GENERATOR1_PLAIN_RATIO)
                                          {
                                              type = CellTypes::PLAIN;
                                          }
                                          else if (h < opts.GENERATOR1_HILL_RATIO)
                                          {
                                              type = CellTypes::HILL;
                                          }
                                          else
                                          {
                                              type = CellTypes::MOUNTAIN;
                                          }

                                          goc.tiles[x][y].type = type; //
                                      });
        }

    }; // end of class

}; // end of namespace
