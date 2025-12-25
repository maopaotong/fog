#pragma once
#include "fg/util.h"
#include "CellsGenerator.h"
namespace fog
{

    struct CellsDatas
    {

        struct Options
        {
            Box2<int> cellsRange;

            SELF(Options)
            GROUP("config")
            MEMBERK(cellsRange, "TILES_RANGE")

            INJECT(Options())
            {
            }
            Options(const Options &&) = delete;
            Options(const Options &) = delete;
            Options &operator()(const Options &) = delete;
            Options &operator()(const Options &&) = delete;
        };
        std::vector<std::vector<CellData>> cells;
        INJECT(CellsDatas(Options& pts, CellsGenerator *generator))
        {
            generator->generateCells(cells, pts.cellsRange.getWidth(), pts.cellsRange.getHeight());
        }
    };

};