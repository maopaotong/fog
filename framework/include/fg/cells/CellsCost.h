/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util/Common.h"
#include "fg/cells/Cells.h"
#include "fg/demo/CellsDatas.h"
namespace fog
{

    struct CellsCost
    {
        std::vector<std::vector<CellData>> *tiles;

        INJECT(CellsCost(CellsDatas *cDatas)) : tiles(&cDatas->tiles)
        {
        }
        int operator()(Point2<int> &cKey)
        {
            CellType type = (*tiles)[cKey.x][cKey.y].type;
            int cost = CostMap::DEFAULT_COST;
            switch (type)
            {
            case CellTypes::OCEAN:
            case CellTypes::MOUNTAIN:
            case CellTypes::LAKE:

                cost = CostMap::OBSTACLE;
                break;
            case CellTypes::HILL:
            case CellTypes::FROZEN:
            case CellTypes::RIVER:
                cost = 2;
                break;
            }
            return cost;
        }
    };

}; // end of namespace
