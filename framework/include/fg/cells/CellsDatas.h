#pragma once
#include "fg/util.h"
#include "CellsGenerator.h"
namespace fog
{

    struct CellsDatas
    {

        struct Args
        {
            Box2<int> cellsRange;

            SELF(Args)
            GROUP("config")
            MEMBERK(cellsRange, "TILES_RANGE")

            INJECT(Args())
            {
            }
            Args(const Args &&) = delete;
            Args(const Args &) = delete;
            Args &operator()(const Args &) = delete;
            Args &operator()(const Args &&) = delete;
        };
        std::vector<std::vector<CellData>> cells;
        INJECT(CellsDatas(Args& pts, CellsGenerator *generator))
        {
            generator->generateCells(cells, pts.cellsRange.getWidth(), pts.cellsRange.getHeight());
        }
    };

};