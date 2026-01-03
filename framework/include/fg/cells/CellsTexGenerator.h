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

    struct CellsTexGenerator
    {
        Config *config;
        CellsDatas &cellsDatas;
        INJECT(CellsTexGenerator(Config *config,
                                 CellsDatas &cellsDatas,
                                 CellsGridsGenerator::Args opts)) : cellsDatas(cellsDatas), config(config)
        {
        }
        ~CellsTexGenerator()
        {
        }

        template <typename F>
        void generateData(unsigned char *data, int cellsCols, int cellsRows, F &&func)
        {
            int typePlot[11] = {0}; // for debug.
            for (int x = 0; x < cellsCols; x++)
            {
                for (int y = 0; y < cellsRows; y++)
                {

                    Box2<int> debugRange = config->debugPrintTerrainsTexRange;
                    CellData &v = cellsDatas.cells[x][y];

                    if (v.type < 10)
                    {
                        typePlot[v.type]++;
                    }
                    else
                    {
                        typePlot[10]++;
                    }

                    int idx = (y * cellsCols + x) * 4;

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
        void createCellsTextures(std::string name1)
        {

            createCellsTexture(name1, [](unsigned char *data, int idx, CellData &v)
                               {
                                   data[idx] = v.type; /** 0 .. 15 **/
                                   data[idx +1] = v.type; /** 0 .. 15 **/
                                   data[idx +2] = v.type; /** 0 .. 15 **/
                                   data[idx +3] = v.type; /** 0 .. 15 **/ });
        }
        template <typename F>
        void createCellsTexture(std::string name, F &&dataF)
        {
            int cellsCols = this->cellsDatas.cols;
            int cellsRows = this->cellsDatas.rows;
            unsigned char *data = new unsigned char[cellsCols * cellsRows * 4];
            this->generateData(data, cellsCols, cellsRows, dataF);

            Ogre::TexturePtr texture = TextureFactory::createTexture(name, cellsCols, cellsRows);

            TextureFactory::updateTexture(texture, cellsCols, cellsRows, data);

            delete[] data;
        }
    };
}; // end of namespace
