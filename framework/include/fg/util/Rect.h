/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "Point2.h"
namespace fog
{

    class Rect
    {
    private:
    public:
        static bool isPointInSide(Vector2 point, Vector2 rectCentre, float rectWidth, float rectHeight)
        {
            float minX = rectCentre.x - rectWidth / 2.0f;
            float minY = rectCentre.y - rectHeight / 2.0f;
            float maxX = rectCentre.x + rectWidth / 2.0f;
            float maxY = rectCentre.y + rectHeight / 2.0f;
            return point.x >= minX && point.x < maxX && point.y >= minY && point.y < maxY;
        }
    };
}; // end of namespace
