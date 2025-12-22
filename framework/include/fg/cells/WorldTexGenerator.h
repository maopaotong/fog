/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"
#include "CellsDatas.h"
#include "CellsGrids.h"
#include "CellsGridsGenerator.h"
namespace fog
{

    struct WorldTexGenerator
    {
        int width;
        int height;
        Config *config;
        std::vector<std::vector<CellsGrid>> &hMap;
        INJECT(WorldTexGenerator(Config *config,
                                 CellsGrids *grids,
                                 CellsGridsGenerator::Options opts)) : hMap(grids->grids), config(config),
                                                                       width(opts.terWidth),
                                                                       height(opts.terHeight)
        {
        }
        ~WorldTexGenerator()
        {
        }

        template <typename F>
        void generateData(unsigned char *data, F &&func)
        {
            int typePlot[11] = {0}; // for debug.
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

                    func(data, idx, v);
                    // data[idx + 3] = v.temperature * 100; //
                    if (debugRange.isIn(x, y))
                    {
                        std::cout << fmt::format("texure[{:>2},{:>2}]:({:>3}|{:>3}|{:>3}|{:>3})", x, y, data[idx], data[idx + 1], data[idx + 2], data[idx + 3]) << std::endl;
                    }
                }
            }

            for (int i = 0; i < 11; i++)
            {
                // std::cout << fmt::format("typePlot[{}] is {:>3}", i, texOp.typePlot[i]) << std::endl;
            }
        }

        // TODO create texture by a texture manager.
        // TODO as well as the FogOfWar Texture creation.
        // World texture is used as the meta data for the shader to determine the child texture.
        void createWorlTextures(std::string name1, std::string name2)
        {

            createWorldTexture(name1, [](unsigned char *data, int idx, CellsGrid &v)
                               {
                                   data[idx] = v.types[0]; /** 0 .. 15 **/
                                   // G 2rd type of the rect.
                                   data[idx + 1] = v.types[1];
                                   // B 3rd type of the rect.
                                   data[idx + 2] = v.types[2];
                                   // A
                                   data[idx + 3] = v.distanceToEdge(1.0) * 100; //
                               });
            createWorldTexture(name2, [](unsigned char *data, int idx, CellsGrid &v)
                               {
                                   if (v.cKey.x > 255 || v.cKey.y > 255)
                                   {
                                       data[idx] = 0;
                                       data[idx + 1] = 0;
                                       data[idx + 2] = 0;
                                       data[idx + 3] = 0;
                                   }
                                   else
                                   {
                                       data[idx] = v.cKey.x;         //
                                       data[idx + 1] = v.cKey.y;     //
                                       data[idx + 2] = v.cKeys[1].x; //
                                       data[idx + 3] = v.cKeys[1].y; //

                                   } //
                               });
        }
        template <typename F>
        void createWorldTexture(std::string name, F &&dataF)
        {
            unsigned char *data = new unsigned char[width * height * 4];
            this->generateData(data, dataF);

            Ogre::TexturePtr texture = TextureFactory::createTexture(name, width, height);

            TextureFactory::updateTexture(texture, width, height, data);

            delete[] data;
        }
    };
}; // end of namespace
