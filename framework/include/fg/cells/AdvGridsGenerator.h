#pragma once
#include "CellsGridsGenerator.h"

namespace fog
{

    struct AdvGridsGenerator : public CellsGridsGenerator
    {

        std::mt19937 randGen;
        std::bernoulli_distribution randHill;
        std::uniform_real_distribution<float> randHeightHill;
        std::bernoulli_distribution randMountain;
        std::uniform_real_distribution<float> randHeightMountain;

        INJECT(AdvGridsGenerator(Args opts, Config *config)) : CellsGridsGenerator(opts, config), randGen(23665289), //
                                                               randHill(0.5f), randHeightHill(0.01f, 0.03f),         //
                                                               randMountain(0.01), randHeightMountain(0.05, 0.1)
        {
            std::mt19937 randGen(23665289);
            std::bernoulli_distribution randHill(0.1f);
        }

        void generate(std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas) override
        {
            CellsGridsGenerator::generate(hMap, cDatas);
        }

        void makeHeight(std::vector<std::vector<CellsGrid>> &hMap) override
        {

            for (int y = 0; y < gridsRows; y++)
            {
                for (int x = 0; x < gridsCols; x++)
                {
                    float maxH = -1;
                    float minH = 2;

                    CellsGrid::forEachNeiberTriangle(hMap, gridsCols, gridsRows, x, y, [&maxH, &minH](CellsGrid &g, int idx, CellsGrid::Triangle &t)
                                                     {
                                                         CellType type = t.type;
                                                         float height = t.height;
                                                         if (height > maxH)
                                                         {
                                                             maxH = height;
                                                         }
                                                         if (height < minH)
                                                         {
                                                             minH = height;
                                                         } //
                                                     });

                    hMap[x][y].aHeight = (maxH + minH) / 2;
                                }
            }
        }
    };
};