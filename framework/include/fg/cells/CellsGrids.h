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

        CellsGridsGenerator::Args opts;

        INJECT(CellsGrids(CellsGridsGenerator::Args opts, CellsDatas *cDatas, CellsGridsGenerator *generator, Config *config)) : config(config), opts(opts), grids(opts.gridsCols, std::vector<CellsGrid>(opts.gridsRows, CellsGrid()))
        {
            generator->generate(grids, cDatas);
        }

    };
}; // end of namespace
