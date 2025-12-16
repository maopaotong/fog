#pragma once
#include <fg/util.h>
#include <fg/cells.h>
#include "fg/cells/CellsGenerator.h"
namespace fog
{

    struct CellsDatas
    {
        struct Options
        {
            int tsWidth;
            int tsHeight;
            INJECT(Options()) : tsWidth(Config::TILES_RANGE.getWidth()), tsHeight(Config::TILES_RANGE.getHeight())
            {
            }
        };
        std::vector<std::vector<CellData>> tiles;

        INJECT(CellsDatas(Options pts))
        {
            CellsGenerator::generateCells(tiles, pts.tsWidth, pts.tsHeight);
        }
    };

};