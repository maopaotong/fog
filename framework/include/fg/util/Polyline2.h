/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <vector>
#include "Point2.h"

namespace fog{
//using namespace Ogre;

class Polyline2
{
protected:
    std::vector<Ogre::Vector2> points;

public:
    Polyline2(Vector2 &p1, Vector2 &p2, Vector2 &p3, Vector2 &p4)
    {
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        points.push_back(p4);
    }
    Polyline2(Vector2 &p1, Vector2 &p2, Vector2 &p3)
    {
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
    }

    Polyline2 &operator*=(const float factor)
    {
        for (auto &p : points)
        {
            p *= factor;
        }
        return *this;
    }

    Polyline2 &operator+=(const Vector2 &point)
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

};
};//end of namespace
