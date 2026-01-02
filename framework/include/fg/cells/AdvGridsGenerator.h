#pragma once
#include "CellsGridsGenerator.h"

namespace fog
{

    struct AdvGridsGenerator : public CellsGridsGenerator
    {
        INJECT(AdvGridsGenerator(Args opts, Config *config)) : CellsGridsGenerator(opts, config)
        {
        }

        void generate(std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas) override
        {
            CellsGridsGenerator::generate(hMap, cDatas);
            //
            //expandBorderRects(hMap, cDatas);
        }
        struct ExpandCtx
        {
            std::vector<std::vector<CellsGrid>> &hMap;
            CellsDatas *cDatas;
            std::unordered_set<int> processed;
            ExpandCtx(std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas) : hMap(hMap), cDatas(cDatas)
            {
            }
        };
        void expandBorderRects(std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas)
        {
            ExpandCtx ctx(hMap, cDatas);
            for (int x = 0; x < cols; x++)
            {
                for (int y = 0; y < rows; y++)
                {
                    doExpand(x, y, ctx);
                }
            }
        }

        void doExpand(int x, int y, ExpandCtx &ctx)
        {

            if (ctx.processed.count(makeKey(x, y)))
            {
                return;
            }
            if (ctx.hMap[x][y].types[0] != ctx.hMap[x][y].types[2])
            {
                doExpanded(x, y, ctx, x, y + 1);
                doExpanded(x, y, ctx, x + 1, y);
                doExpanded(x, y, ctx, x, y - 1);
                doExpanded(x, y, ctx, x - 1, y);
            }
        }

        int makeKey(int x, int y)
        {
            return y * cols + x;
        }

        void doExpanded(int x, int y, ExpandCtx &ctx, int x1, int y1)
        {
            if (x1 < 0 || x1 > cols - 1 || y1 < 0 || y1 > rows - 1)
            {
                return;
            }
            if (ctx.hMap[x1][y1].cKeys[0] == ctx.hMap[x][y].cKeys[0] && ctx.hMap[x1][y1].cKeys[0] != ctx.hMap[x1][y1].cKeys[2])
            {
                ctx.hMap[x1][y1].types[2] = ctx.hMap[x][y].types[2];
                ctx.hMap[x1][y1].cKeys[2] = ctx.hMap[x][y].cKeys[2];

                ctx.processed.insert(makeKey(x1, y1));
            }
        }
    };
};