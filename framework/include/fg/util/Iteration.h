/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
namespace fog
{
    class Iteration
    {
    public:
        template <typename... Args, typename F1, typename F2>
        static void forEachAsTable(int width, int height, F1 &&rowFunc, F2 &&colFunc, Args... args)
        {
            for (int i = 0; i < width; i++)
            {
                rowFunc(i, args...);
                for (int j = 0; j < height; j++)
                {
                    colFunc(i, j, args...);
                }
            }
        }
        template <typename T, typename F>
        static void forEach(std::vector<std::vector<T>> &table, int width, int height, F &&visit)
        {
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    visit(i, j, table[i][j]);
                }
            }
        }

        template <typename T, typename F>
        static void forEach(std::unordered_set<T> &set, F &&visit)
        {
            for (auto it = set.begin(); it != set.end(); it++)
            {
                visit(*it);
            }
        }
    };
}; // end of namespace
