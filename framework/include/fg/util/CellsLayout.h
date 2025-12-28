/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util/Common.h"
#include <cmath>
#include <algorithm>
#include "Common.h"

#include "Point2.h"
#include "Box2.h"

#include "CollectionUtil.h"
#include "Transform.h"
#include "CellKey.h"

namespace fog
{

    using OffsetPointy = Cell::PointyOffset;
    using OffsetFlat = Cell::FlatOffset;
        

    /**
     *
     *    * * *     [0,3]     [1,3]    [2,3]
     *   * * *    [0,2]   [1,2]    [2,2]
     *    * * *     [0,1]     [1,1]    [2,1]
     *   * * *    [0,0]   [1,0]    [2,0]
     *
     */

    struct OffsetPointyNeibersOp
    {
        const static inline int offsetEvenX[6] = {0, 1, 0, -1, -1, -1};
        const static inline int offsetEvenY[6] = {-1, 0, 1, 1, 0, -1};
        const static inline int offsetOddX[6] = {1, 1, 1, 0, -1, 0};
        const static inline int offsetOddY[6] = {-1, 0, 1, 1, 0, -1};

        void operator()(int x, int y, OffsetPointy *neibers)
        {
            // e.g. :[1,2]
            if (y % 2 == 0)
            {
                for (int i = 0; i < 6; i++)
                {
                    neibers[i].x = x + offsetEvenX[i];
                    neibers[i].y = y + offsetEvenY[i];
                }
            }
            else
            { // e.g. [1,1]

                for (int i = 0; i < 6; i++)
                {
                    neibers[i].x = x + offsetOddX[i];
                    neibers[i].y = y + offsetOddY[i];
                }
            }
        }
    };

    /**                12
     *         02                22                42
     *       ******    11      ******     31     ******     52
     *     *        *        *        *        *        *
     *   *     01     ******     21     ******     41     ******
     *     *        *        *        *        *        *        *
     *       ******     10     ******     30     ******     50     *
     *     *        *        *        *        *        *        *
     *   *     00     ******     20     ******     40     ******
     *     *        *        *        *        *        *
     *       ******            ******            ******
     */
    struct OffsetFlatNeibersOp
    {

        const inline static int offsetEvenX[6] = {-1, 0, 1, 1, 0, -1};
        const inline static int offsetEvenY[6] = {-1, -1, -1, 0, 1, -1};
        const inline static int offsetOddX[6] = {-1, 0, 1, 1, 0, -1};
        const inline static int offsetOddY[6] = {0, -1, 0, 1, 1, 1};

        void operator()(int x, int y, OffsetFlat *neibers)
        {

            // e.g. :[2,1]
            if (x % 2 == 0)
            {
                for (int i = 0; i < 6; i++)
                {
                    neibers[i].x = x + offsetEvenX[i];
                    neibers[i].y = y + offsetEvenY[i];
                }
            }
            else
            { // e.g. [1,1]

                for (int i = 0; i < 6; i++)
                {
                    neibers[i].x = x + offsetOddX[i];
                    neibers[i].y = y + offsetOddY[i];
                }
            }
        }
    };

    struct CellsLayout
    {

        template <typename K>
        static void getNeibers(K &cKey, K *neibers)
        {
            static_assert(false, "un-supported type of cell ckey.");
        }

        template <>
        static void getNeibers<CellKey>(CellKey &cKey, CellKey *neibers)
        {
            static OffsetPointyNeibersOp neiber;
            return neiber(cKey.x, cKey.y, neibers);
        }

        template <>
        static void getNeibers<OffsetFlat>(OffsetFlat &off, OffsetFlat *neibers)
        {
            static OffsetFlatNeibersOp neiber;
            return neiber(off.x, off.y, neibers);
        }
    };

};
