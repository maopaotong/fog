/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"
#include "CellsDatas.h"
#include "CellGrid.h"

namespace fog
{

    struct CellsGridsGenerator
    {
        struct Args
        {
            int gridsCols;
            int gridsRows;
            int gridAmp;
            float gridEdgeLength;
            float gridHeight;
            float gridHalfEdgeLength;
            CellsDatas::Args &cells;

            SELFG(Args, "config")
            MEMBERKD(gridAmp, "gridAmp", 8)
            INJECT(Args(Config *config, CellsDatas::Args &cells)) : cells(cells)
            {
            }

            INIT(init)()
            {
                if (CellLayout == Cell::FlatTop)
                {
                    this->gridsCols = cells.cellsRange.getWidth() * this->gridAmp * 3 / 2; //
                    this->gridsRows = cells.cellsRange.getHeight() * this->gridAmp * 2;    //
                    this->gridEdgeLength = Cell::LayoutInfo<CellLayout>::outerRad / gridAmp;
                    this->gridHeight = Cell::LayoutInfo<CellLayout>::innerRad / gridAmp;
                    this->gridHalfEdgeLength = this->gridEdgeLength / 2;
                }
                else
                {
                    throw std::runtime_error("not supported.");
                }
            }
        };

        Config *config;
        int cellsCols;
        int cellsRows;
        int gridsCols;
        int gridsRows;

        float heightAmpOfHill;
        float heightAmpOfMountain;

        Args opts;

        INJECT(CellsGridsGenerator(Args opts, Config *config)) : opts(opts),
                                                                 config(config)
        {
            cellsCols = opts.cells.cellsRange.getWidth();
            cellsRows = opts.cells.cellsRange.getHeight();
            gridsCols = opts.gridsCols;
            gridsRows = opts.gridsRows;
        }

        virtual void generate(std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas)
        {

            for (int x = 0; x < gridsCols; x++)
            {
                for (int y = 0; y < gridsRows; y++)
                {

                    auto &cells = cDatas->cells;
                    float offsetX = (y % 2 == 0) ? 0 : opts.gridEdgeLength / 2;

                    float xA = opts.gridEdgeLength * x + offsetX;
                    float xB = opts.gridEdgeLength * (x + 1) + offsetX;
                    float xC = opts.gridEdgeLength * (x + 1.5f) + offsetX;
                    float xD = opts.gridEdgeLength * (x + 0.5f) + offsetX;
                    float yA = opts.gridHeight * y;
                    float yB = yA;
                    float yC = opts.gridHeight * (y + 1);
                    float yD = yC;
                    Point2<float> a{xA, yA};
                    Point2<float> b{xB, yB};
                    Point2<float> c{xC, yC};
                    Point2<float> d{xD, yD};

                    Point2<float> centre1 = (a + b + d) / 2;
                    Point2<float> centre2 = (b + c + d) / 2;

                    hMap[x][y].a = a;
                    hMap[x][y].b = b;
                    hMap[x][y].c = c;
                    hMap[x][y].d = d;
                    hMap[x][y].centre1 = centre1;
                    hMap[x][y].centre2 = centre2;
                    //
                    CellKey cKey1 = CellTransform::transform<Cell::Cartesian, Cell::Offset>(centre1);
                    CellKey cKey2 = CellTransform::transform<Cell::Cartesian, Cell::Offset>(centre2);

                    CellData & cell1 = cell(cDatas,cKey1);
                    CellData & cell2 = cell(cDatas,cKey2);

                    hMap[x][y].cKey1 = cKey1;
                    hMap[x][y].cKey2 = cKey2;
                    hMap[x][y].type1 = cell1.type;
                    hMap[x][y].type2 = cell2.type;

                    hMap[x][y].height1 = cellTypeHeight(cell1);
                    hMap[x][y].height2 = cellTypeHeight(cell2);
                    // todo
                }
            }

            // calculate point a height.
            for (int x = 0; x < gridsCols; x++)
            {
                for (int y = 0; y < gridsRows; y++)
                {
                    float hs[6];
                    hs[0] = hMap[x][y].height1;
                    hs[1] = grid(hMap, x - 1, y).height2;
                    hs[2] = grid(hMap, x - 1, y).height1;
                    hs[3] = grid(hMap, x - 2, y - 1).height2;
                    hs[4] = grid(hMap, x - 1, y - 1).height2;
                    hs[5] = grid(hMap, x - 1, y - 1).height1;

                    float h = 0;
                    for (int i = 0; i < 6; i++)
                    {
                        h += hs[i];
                    }
                    hMap[x][y].aHeight = h / 6;
                }
            }
        }
        CellData &cell(CellsDatas *cDatas, CellKey cKey)
        {
            int x = cKey.col % cellsCols;
            int y = cKey.row % cellsRows;

            if (x < 0)
            {
                x += cellsCols;
            }
            if (y < 0)
            {
                y += cellsRows;
            }
            return cDatas->cells[x][y];
        }

        CellsGrid &grid(std::vector<std::vector<CellsGrid>> &hMap, int x, int y)
        {
            if (x < 0)
            {
                x += gridsCols;
            }
            if (y < 0)
            {
                y += gridsRows;
            }
            return hMap[x][y];
        }

        static float map(float low1, float heigh1, float low2, float high2, float value)
        {
            return low2 + (high2 - low2) * ((value - low1) / (heigh1 - low1));
        }

        /**
         *
         */
        float cellTypeHeight(CellData &tl)
        {

            float tlHeight = 0.0;

            switch (tl.type)
            {
            case (CellTypes::OCEAN):
                tlHeight = config->heightOfOcean;
                break;
            case (CellTypes::SHORE):
            case (CellTypes::FRZ_SHORE):
            case (CellTypes::LAKE):
                tlHeight = config->heightOfShore;
                break;
            case (CellTypes::PLAIN):
            case (CellTypes::FRZ_PLAIN):
                tlHeight = config->heightOfPlain;
                break;
            case (CellTypes::HILL):
                tlHeight = config->heightOfHill;
                break;
            case (CellTypes::MOUNTAIN):
            case (CellTypes::FRZ_MOUNTAIN):

                tlHeight = config->heightOfMountain;
                break;
            default:
                tlHeight = config->heightOfFrozen;
                break;
            }
            return tlHeight;
        }
    };

}; // end of namespace
