/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
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
