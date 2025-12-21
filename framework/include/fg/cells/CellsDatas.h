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
            INJECT(Options(Config *config)) : tsWidth(config->cellsRange.getWidth()), tsHeight(config->cellsRange.getHeight())
            {
            }
        };
        std::vector<std::vector<CellData>> cells;
        Config *config;
        INJECT(CellsDatas(Options pts, Config *config, CellsGenerator *generator)) : config(config)
        {
            generator->generateCells(cells, pts.tsWidth, pts.tsHeight, config);
        }
    };

};