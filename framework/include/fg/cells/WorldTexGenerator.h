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
        int cols;
        int rows;
        INJECT(WorldTexGenerator(CellsGridsGenerator::Args &opts)) : cols(std::pow(2, 12) + 1),
                                                                     rows(cols)
        {
        }
        ~WorldTexGenerator()
        {
        }

        void createWorlTextures(std::string texName)
        {
            int size = std::max(cols, rows);
            createWorldTexture(texName, [this, size](unsigned char *data)
                               {
                                   std::vector<std::vector<float>> heightmap(size, std::vector<float>(size, 0));
                                   DiamondSquare::generate(heightmap, size, 0.8f, 123546782);
                                   Normaliser::normalise(heightmap, size, size); //
                                   for (int y = 0; y < rows; y++)
                                   {
                                       for (int x = 0; x < cols; x++)
                                       {
                                           int idx = (y * cols + x) * 4;
                                           data[idx] = heightmap[x][y] * 255;
                                       }
                                   } //
                               });
        }
        template <typename F>
        void createWorldTexture(std::string name, F &&dataF)
        {
            unsigned char *data = new unsigned char[cols * rows * 4];
            dataF(data);

            Ogre::TexturePtr texture = TextureFactory::createTexture(name, cols, rows);

            TextureFactory::updateTexture(texture, cols, rows, data);

            delete[] data;
        }
    };
}; // end of namespace
