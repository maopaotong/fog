/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"
#include "CellsDatas.h"
#include "CellGrid.h"
#include "CellsGridsGenerator.h"
namespace fog
{

    struct CellsGrids
    {

        Config *config;
        std::vector<std::vector<CellsGrid>> grids;

        std::vector<std::vector<CellData>> tiles;

        CellsGridsGenerator::Options opts;

        INJECT(CellsGrids(CellsGridsGenerator::Options opts, CellsDatas *cDatas, CellsGridsGenerator *generator, Config *config)) : config(config), opts(opts), grids(opts.terWidth, std::vector<CellsGrid>(opts.terHeight, CellsGrid()))
        {
            generator->generate(grids, cDatas);
        }

        struct WorldTexOp
        {

            std::vector<std::vector<CellsGrid>> &hMap;
            int typePlot[11] = {0}; // for debug.
            int width;
            int height;
            unsigned char *data;
            Config *config;
            WorldTexOp(std::vector<std::vector<CellsGrid>> &hMap, int w, int h, Config *config) : hMap(hMap), width(w), height(h), config(config)
            {
                data = new unsigned char[w * h * 4];
            }
            ~WorldTexOp()
            {
                delete[] data;
            }

            void operator()()
            {

                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {

                        Box2<int> debugRange = config->debugPrintTerrainsTexRange;
                        CellsGrid &v = hMap[x][y];
                        if (v.types[0] < 10)
                        {
                            typePlot[v.types[0]]++;
                        }
                        else
                        {
                            typePlot[10]++;
                        }

                        int idx = (y * width + x) * 4;

                        // R as the type of the centre point of the rect, the precision is based on the qulity parameter configred.

                        data[idx] = v.types[0]; /** 0 .. 15 **/
                        // G 2rd type of the rect.
                        data[idx + 1] = v.types[1];
                        // B 3rd type of the rect.
                        data[idx + 2] = v.types[2];
                        // A
                        data[idx + 3] = v.distanceToEdge(1.0) * 100; //
                        // data[idx + 3] = v.temperature * 100; //
                        if (debugRange.isIn(x, y))
                        {
                            std::cout << fmt::format("texure[{:>2},{:>2}]:({:>3}|{:>3}|{:>3}|{:>3})", x, y, data[idx], data[idx + 1], data[idx + 2], data[idx + 3]) << std::endl;
                        }
                    }
                }
            }
        };

        // TODO create texture by a texture manager.
        // TODO as well as the FogOfWar Texture creation.
        // World texture is used as the meta data for the shader to determine the child texture.
        void createWorldTexture(std::string name, std::vector<std::vector<CellsGrid>> &hMap)
        {
            WorldTexOp texOp(hMap, opts.terWidth, opts.terHeight, config);
            texOp();
            Ogre::TexturePtr texture = TextureFactory::createTexture(name, opts.terWidth, opts.terHeight);

            TextureFactory::updateTexture(texture, opts.terWidth, opts.terHeight, texOp.data);

            for (int i = 0; i < 11; i++)
            {
                std::cout << fmt::format("typePlot[{}] is {:>3}", i, texOp.typePlot[i]) << std::endl;
            }
        }
    };
}; // end of namespace
