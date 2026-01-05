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
            MEMBERKD(gridAmp, "gridAmp", 1)
            INJECT(Args(Config *config, CellsDatas::Args &cells)) : cells(cells)
            {
            }

            INIT(init)()
            {
                if (CellLayout == Cell::FlatTop)
                {
                    this->gridsCols = cells.cellsRange.getWidth() * this->gridAmp * 1.5; //
                    this->gridsRows = cells.cellsRange.getHeight() * this->gridAmp * 2;  //
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

            for (int y = 0; y < gridsRows; y++)
            {
                bool evenRow = (y % 2 == 0);
                float offsetBottomEdge = evenRow ? 0 : opts.gridEdgeLength / 2;
                float offsetTopEdge = evenRow ? opts.gridEdgeLength / 2 : 0;

                for (int x = 0; x < gridsCols; x++)
                {

                    auto &cells = cDatas->cells;

                    float xA = opts.gridEdgeLength * x + offsetBottomEdge;
                    float xB = opts.gridEdgeLength * (x + 1) + offsetBottomEdge;
                    float xC = opts.gridEdgeLength * (x + 1) + offsetTopEdge;
                    float xD = opts.gridEdgeLength * x + offsetTopEdge;

                    float yA = opts.gridHeight * y;
                    float yB = opts.gridHeight * y;
                    float yC = opts.gridHeight * (y + 1);
                    float yD = opts.gridHeight * (y + 1);

                    Point2<float> a{xA, yA};
                    Point2<float> b{xB, yB};
                    Point2<float> c{xC, yC};
                    Point2<float> d{xD, yD};

                    hMap[x][y].a = a;
                    hMap[x][y].b = b;
                    hMap[x][y].c = c;
                    hMap[x][y].d = d;
                    if (evenRow)
                    {

                        hMap[x][y].centre1 = (a + b + d) / 3;
                        hMap[x][y].centre2 = (b + c + d) / 3;
                    }
                    else
                    {
                        hMap[x][y].centre1 = (a + b + c) / 3;
                        hMap[x][y].centre2 = (a + c + d) / 3;
                    }

                    //
                    CellKey cKey1 = CellTransform::transform<Cell::Cartesian, Cell::Offset>(hMap[x][y].centre1);
                    CellKey cKey2 = CellTransform::transform<Cell::Cartesian, Cell::Offset>(hMap[x][y].centre2);

                    CellData &cell1 = cell(cDatas, cKey1);
                    CellData &cell2 = cell(cDatas, cKey2);

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
            for (int y = 0; y < gridsRows; y++)
            {
                bool evenRow = (y % 2 == 0);
                for (int x = 0; x < gridsCols; x++)
                {
                    std::unordered_set<CellType> typeSet;
                    std::array<std::tuple<int, int, int>, 6> triangles;
                    if (evenRow)
                    {
                        triangles[0] = {0, 0, 1};
                        triangles[1] = {-1, 0, 2};
                        triangles[2] = {-1, 0, 1};
                        triangles[3] = {-1, -1, 2};
                        triangles[4] = {-1, -1, 1};
                        triangles[5] = {0, -1, 2};
                    }
                    else
                    {
                        triangles[0] = {0, 0, 1};
                        triangles[1] = {0, 0, 2};
                        triangles[2] = {-1, 0, 1};
                        triangles[3] = {-1, -1, 2};
                        triangles[4] = {0, -1, 1};
                        triangles[5] = {0, -1, 2};
                    }

                    float h = 0;
                    for (int i = 0; i < 6; i++)
                    {
                        std::tuple<int, int, int> tp = triangles[i];
                        CellsGrid &g = grid(hMap, x + std::get<0>(tp), y + std::get<1>(tp)); // index of grid.

                        CellType type;
                        float height;
                        if (std::get<2>(tp) == 1) // first trigangle
                        {
                            type = g.type1;
                            height = g.height1;
                        }
                        else
                        { // second triangle
                            type = g.type2;
                            height = g.height2;
                        }
                        if (auto it = typeSet.find(type); it == typeSet.end())
                        {
                            typeSet.insert(type);
                            h += height;
                        }
                    }
                    hMap[x][y].aHeight = h / typeSet.size();
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

}; // end of
