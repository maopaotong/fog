/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/ogre.h"

#include "fg/util.h"
#include "Cells.h"

namespace fog
{

    static constexpr float UNRESOLVED_HEIGHT = -100;
    struct CellsVertex
    {
        float height;
        CellKey cKey;
        Vector2 originInCell;
        std::array<CellType, 3> types;
        bool isPeak;

        CellsVertex() : CellsVertex(UNRESOLVED_HEIGHT, -1, -1)
        {
        }
        CellsVertex(int tx, int ty, float th) : height(UNRESOLVED_HEIGHT), cKey(tx, ty),
                                                types{CellTypes::UNKNOW, CellTypes::UNKNOW, CellTypes::UNKNOW},
                                                isPeak(false)
        {
        }
        bool isHeightResolved()
        {
            return std::abs(this->height - UNRESOLVED_HEIGHT) > 0.01f; // height is not unknown.
        }

        float distanceToCell()
        {
            return originInCell.length();
        }

        float distanceToEdge(float rad)
        {
            Vector2 p = this->originInCell;
            if (p.x < 0)
            {
                p.x = -p.x;
            }
            if (p.y < 0)
            {
                p.y = -p.y;
            }
            if (p.y > (1.0 / std::sqrt(3.0) * p.x))
            {
                p = rotateClockwise60(p);
            }
            return std::abs(p.x - rad); // < 0 inner, > 0 outer.
        }

        Vector2 rotateClockwise60(Vector2 p)
        {
            const float c = 0.5;                // cos(60°)
            const float s = std::sqrt(3) / 2.0; // sin(60°)
            return Vector2(c * p.x + s * p.y, -s * p.x + c * p.y);
        }

        float distance(CellKey cKey2)
        {
            return (Vector2(cKey.x, cKey.y) + originInCell).distance(Vector2(cKey2.x, cKey2.y));
        }
    };

}; // end of namespace
