/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util/Point2.h"
#include "fg/util/Polyline2.h"

namespace fog
{
    using Vector2 = Point2<float>;
    class PathFollow2
    {
    public:
        std::vector<Point2<float>> path;
        float speedFactor = 30.0f;
        float speed;
        int next = 1; // ignore the first one.
        Point2<float> position;

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
