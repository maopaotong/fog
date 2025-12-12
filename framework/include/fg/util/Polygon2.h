/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <vector>
#include <Ogre.h>

namespace fog{
using namespace Ogre;

class Polygon2
{
private:
    std::vector<Ogre::Vector2> points;

public:
    Polygon2()
    {
    }
    Polygon2(Vector2 &p1, Vector2 &p2, Vector2 &p3, Vector2 &p4)
    {
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        points.push_back(p4);
    }
    Polygon2(Vector2 &p1, Vector2 &p2, Vector2 &p3)
    {
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
    }
    void add(float x, float y)
    {
        add(Vector2(x, y));
    }

    void add(Vector2 &p)
    {
        points.push_back(p);
    }

    Polygon2 &operator*=(const float factor)
    {
        for (auto &p : points)
        {
            p *= factor;
        }
        return *this;
    }

    Polygon2 &operator+=(const Vector2 &point)
    {
        for (auto &p : points)
        {
            p += point;
        }
        return *this;
    }

    int size() const
    {
        return points.size();
    }
    Ogre::Vector2 &operator[](int i)
    {
        return points[i];
    }
    const Ogre::Vector2 &operator[](int i) const
    {
        return points[i];
    }

    bool isPointInPolygon(float px, float py)
    {

        // 叉积判断是否在所有边的左侧
        int size = points.size();
        for (int i = 0; i < size; ++i)
        {
            // 逆时针
            auto p1 = points[i];
            auto p2 = points[(i + 1) % size];

            // 向量 edge = p2 - p1
            // 向量 point = (mx, my) - p1
            float cross = (px - p1.x) * (p2.y - p1.y) - (py - p1.y) * (p2.x - p1.x);
            if (cross < 0)
                return false;
        }

        return true;
    }
};
};//end of namespace
