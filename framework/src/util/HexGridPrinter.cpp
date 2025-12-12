/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#include <iostream>
#include "fg/util/HexGridPrinter.h"
namespace fog
{
void HexGridPrinter::printCostGrid(CostMap &grid)
{
    std::cout << "Original Cost Grid (0=obstacle, 1=normal, 2=costly, 3=very costly):\n";
    for (int y = 0; y < grid.height; y++)
    {
        if (y % 2 == 1)
            std::cout << " ";
        for (int x = 0; x < grid.width; x++)
        {
            // int cost = grid.costGrid[y][x];
            // if (cost == grid.OBSTACLE)
            // {
            //     std::cout << "# ";
            // }
            // else
            // {
            //     std::cout << cost << " ";
            // }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// === Print path result grid ===
void HexGridPrinter::printPathGrid(CostMap *grid, int startx , int starty , int endx , int endy ,
                                   const std::vector<Ogre::Vector2> &path)
{
    std::cout << "Path Result (S=start, E=end, *=path, number=cost):\n";

    std::unordered_set<CellKey, CellKey::Hash> pathSet;
    for (const auto &p : path)
    {
        pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
    }

    for (int y = 0; y < grid->height; y++)
    {
        if (y % 2 == 1)
            std::cout << " ";
        for (int x = 0; x < grid->width; x++)
        {
            char c = '.';
            int cost =0;// grid->costGrid[y][x];

            if (x == startx && y == starty)
            {
                c = 'S';
            }
            else if (x == endx && y == endy)
            {
                c = 'E';
            }
            else if (cost == CostMap::OBSTACLE)
            {
                c = '#';
            }
            else if (pathSet.find({x, y}) != pathSet.end() &&
                     !(x == startx && y == starty) && !(x == endx && y == endy))
            {
                c = '*'; // Path
            }
            else
            {
                if (cost == CostMap::DEFAULT_COST)
                {
                    c = '.'; // Default cost shown as dot
                }
                else
                {
                    c = '0' + cost; // Show specific cost number
                }
            }

            std::cout << c << " ";
        }
        std::cout << "\n";
    }
}
};
