/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/ogre.h"

#include "fg/util.h"
#include "Common.h"

namespace fog
{
    struct CellsGrid
    {

    private:
        static std::array<std::array<std::tuple<int, int>, 6>, 2> initNeiberPointsOffset()
        {
            std::array<std::array<std::tuple<int, int>, 6>, 2> points;

            points[0][0] = {1, 0};
            points[0][1] = {0, 1};
            points[0][2] = {-1, 1};
            points[0][3] = {-1, 0};
            points[0][4] = {-1, -1};
            points[0][5] = {0, -1};

            points[1][0] = {1, 0};
            points[1][1] = {1, 1};
            points[1][2] = {0, 1};
            points[1][3] = {-1, 0};
            points[1][4] = {0, -1};
            points[1][5] = {1, -1};
            return points;
        }

        static std::array<std::array<std::tuple<int, int, int>, 6>, 2> initNeiberTrianglesOffset()
        {
            std::array<std::array<std::tuple<int, int, int>, 6>, 2> triangles;

            triangles[0][0] = {0, 0, 0};
            triangles[0][0] = {-1, 0, 1};
            triangles[0][1] = {-1, 0, 0};
            triangles[0][3] = {-1, -1, 1};
            triangles[0][4] = {-1, -1, 0};
            triangles[0][5] = {0, -1, 1};

            triangles[1][0] = {0, 0, 0};
            triangles[1][0] = {0, 0, 1};
            triangles[1][1] = {-1, 0, 0};
            triangles[1][3] = {-1, -1, 1};
            triangles[1][4] = {0, -1, 0};
            triangles[1][5] = {0, -1, 1};
            return triangles;
        }

        static CellsGrid &grid(std::vector<std::vector<CellsGrid>> &hMap, int cols, int rows, int x, int y)
        {
            if (x < 0)
            {
                x += cols;
            }
            if (y < 0)
            {
                y += rows;
            }
            return hMap[x][y];
        }

    public:
        inline static std::array<std::array<std::tuple<int, int>, 6>, 2> neiberPointsOffset = initNeiberPointsOffset();
        inline static std::array<std::array<std::tuple<int, int, int>, 6>, 2> neiberTrianglesOffset = initNeiberTrianglesOffset();

        template <typename F>
        static void forEachNeiberTriangle(std::vector<std::vector<CellsGrid>> &hMap, int cols, int rows, int x, int y, F &&visit)
        {
            for (int i = 0; i < 6; i++)
            {
                std::tuple<int, int, int> &tp = CellsGrid::neiberTrianglesOffset[y % 2][i];
                CellsGrid &g = grid(hMap, cols, rows, x + std::get<0>(tp), y + std::get<1>(tp)); // index of grid.
                int T = std::get<2>(tp);
                visit(g, i, g.triangles[T]);
            }
        }

        struct Triangle
        {
            CellKey cKey;
            CellType type;

            Point2<float> centre;
            float height;
        };

        Point2<float> a;
        Point2<float> b;
        Point2<float> c;
        Point2<float> d;
        //
        Triangle triangles[2];

        float aHeight;
        // distance between a and the centre of the the cell0, the cell contains the triangle0.
        bool isACellCentre;
        bool isACellEdge;
    };

}; // end of namespace
