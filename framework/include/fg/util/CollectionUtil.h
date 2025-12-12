/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>

namespace fog
{
    class CollectionUtil
    {

    public:
        template <typename T>
        static std::vector<std::vector<T>> &vectorVector(int width, int height)
        {
            return std::vector<std::vector<T>>(width, std::vector<T>(height, T()));
        }

        template <typename T>
        static bool removeFromVector(std::vector<T> &vec, const T &ele)
        {
            return remove(vec, ele);
        }

        template <typename C, typename E>
        static bool remove(C &c, E &ele)
        {
            auto it = std::find(c.begin(), c.end(), ele);
            if (it != c.end())
            {
                c.erase(it);
                return true;
            }
            return false;
        }
        template <typename T>
        static void intersects(std::unordered_set<T> &set1, std::unordered_set<T> &set2)
        {
            for (auto it = set1.begin(); it != set1.end(); it)
            {
                if (set2.find(*it) == set2.end())
                {
                    it = set1.erase(it);
                }
                else
                {
                    it++;
                }
            }
        }
        template <typename T1, typename T2, typename F>
        static void transform(std::vector<T1> &v1, std::vector<T2> &v2, F &&func)
        {
            v2.reserve(v1.size());
            for (auto it = v1.begin(); it != v1.end(); it++)
            {
                v2.push_back(func(*it));
            }
        }
    };

}; // end of namespace
