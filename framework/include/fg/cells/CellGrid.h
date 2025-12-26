/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/ogre.h"

#include "fg/util.h"
#include "Common.h"

namespace fog
{

    static constexpr float UNRESOLVED_HEIGHT = -100;
    struct CellsGrid
    {
        float height;
        CellKey::OffsetPointy cKey;
        Vector2 originInCell;
        std::array<CellType, 3> types;
        std::array<CellKey::OffsetPointy, 3> cKeys;
        bool isPeak;
        int userData;

        CellsGrid() : CellsGrid(UNRESOLVED_HEIGHT, -1, -1)
        {
        }
        CellsGrid(int tx, int ty, float th) : height(UNRESOLVED_HEIGHT), cKey(tx, ty),
                                              types{CellTypes::UNKNOW, CellTypes::UNKNOW, CellTypes::UNKNOW},
                                              cKeys{CellKey::OffsetPointy(-1, -1), CellKey::OffsetPointy(-1, -1), CellKey::OffsetPointy(-1, -1)},
                                              isPeak(false), userData(0)
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

        int getRegions()
        {
            int typeC = 3;
            if (types[0] == types[1] && types[0] == types[2])
            {
                typeC = 1;
            }
            else if (types[0] == types[1] || types[0] == types[2] || types[1] == types[2])
            {
                typeC = 2;
            }
            return typeC;
        }
    };

}; // end of namespace
