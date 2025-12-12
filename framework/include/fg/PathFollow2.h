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

#include "fg/util/Point2.h"
#include "fg/util/Polyline2.h"

namespace fog
{
    
    class PathFollow2
    {
    public:
        std::vector<Point2<float>> path;
        float speedFactor = 30.0f;
        float speed;
        int next = 1; // ignore the first one.
        Vector2 position;

    public:
        PathFollow2(Vector2 position, std::vector<Point2<float>> path, float speed) : speed(speed)
        {
            this->position = position;
            this->path = path;
        }

        float getSpeed()
        {
            return speed;
        }

        bool move(float timeEscape, Vector2 &currentPos, Vector2 &direction)
        {
            bool rt = false;
            while (next < path.size())
            {
                Vector2 nextPos = path[next];   // next position
                direction = nextPos - position; // direction

                float distance = direction.length();
                if (distance < 0.01f)
                {
                    next++;
                    continue;
                }
                // found the right position
                direction.normalise();
                float move = speed * speedFactor * timeEscape;
                if (move > distance)
                {
                    move = distance;
                }

                position += direction * move;
                currentPos = this->position;
                rt = true;
                break;
            }
            return rt;
        }
    };
};
