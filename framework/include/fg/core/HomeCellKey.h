/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util.h"
#include "fg/cells.h"

namespace fog
{

    struct HomeCellKey
    {

        CellsDatas *cDatas;
        CellKey cKey;
        INJECT(HomeCellKey(CellsDatas *cDatas)) : cDatas(cDatas)
        {
            cKey = this->findCellToStandOn();
        }

    private:
        CellKey findCellToStandOn()
        {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<int> rPosX(0, Config::TILES_RANGE.getWidth() - 1);  //
            std::uniform_int_distribution<int> rPosY(0, Config::TILES_RANGE.getHeight() - 1); //

            for (int i = 0; i < 100; i++)
            {
                int x = rPosX(gen);
                int y = rPosX(gen);
                CellType type = cDatas->tiles[x][y].type;
                if (type == CellTypes::OCEAN || type == CellTypes::MOUNTAIN)
                {
                    continue;
                }
                return CellKey(x, y);
            }

            return CellKey(0, 0);
        }
    };//end class
};
