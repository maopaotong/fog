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
#include <Ogre.h>

namespace fog{
using namespace Ogre;

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
