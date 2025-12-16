#pragma once
#include "fg/util.h"
#include "CellsGenerator.h"
namespace fog
{

    struct CellsDatas
    {
        struct Options
        {
            int tsWidth;
            int tsHeight;
            INJECT(Options(Config *config)) : tsWidth(config->TILES_RANGE.getWidth()), tsHeight(config->TILES_RANGE.getHeight())
            {
            }
        };
        std::vector<std::vector<CellData>> tiles;
        Config *config;
        INJECT(CellsDatas(Options pts, Config *config)) : config(config)
        {
            CellsGenerator::generateCells(tiles, pts.tsWidth, pts.tsHeight, config);
        }
    };

};