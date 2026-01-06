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

                        hMap[x][y].triangles[0].centre = (a + b + d) / 3;
                        hMap[x][y].triangles[1].centre = (b + c + d) / 3;
                    }
                    else
                    {
                        hMap[x][y].triangles[0].centre = (a + b + c) / 3;
                        hMap[x][y].triangles[1].centre = (a + c + d) / 3;
                    }

                    //
                    CellKey cKey0 = CellTransform::transform<Cell::Cartesian, Cell::Offset>(hMap[x][y].triangles[0].centre);
                    CellKey cKey1 = CellTransform::transform<Cell::Cartesian, Cell::Offset>(hMap[x][y].triangles[1].centre);

                    CellData &cell0 = cell(cDatas, cKey0);
                    CellData &cell1 = cell(cDatas, cKey1);

                    hMap[x][y].triangles[0].cKey = cKey0;
                    hMap[x][y].triangles[1].cKey = cKey1;
                    hMap[x][y].triangles[0].type = cell0.type;
                    hMap[x][y].triangles[1].type = cell1.type;

                    hMap[x][y].triangles[0].height = cellTypeHeight(cell0);
                    hMap[x][y].triangles[1].height = cellTypeHeight(cell1);
                    // todo
                }
            }

            // calculate point a height.
            for (int y = 0; y < gridsRows; y++)
            {
                for (int x = 0; x < gridsCols; x++)
                {
                    hMap[x][y].aHeight = makeHeight(hMap, x, y);
                }
            }
        }

        virtual float makeHeight(std::vector<std::vector<CellsGrid>> &hMap, int x, int y)
        {
            std::unordered_set<CellType> typeSet;
            float h = 0;

            CellsGrid::forEachNeiberTriangle(hMap, gridsCols, gridsRows, x, y, [&h, &typeSet](CellsGrid &g, int idx, CellsGrid::Triangle &t)
                                             {
                                                 CellType type = t.type;
                                                 float height = t.height;

                                                 if (auto it = typeSet.find(type); it == typeSet.end())
                                                 {
                                                     typeSet.insert(type);
                                                     h += height;
                                                 } //
                                             });

            return h / typeSet.size();
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
