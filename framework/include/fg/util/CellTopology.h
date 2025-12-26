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

    /**
     *
     *    * * *     [0,3]     [1,3]    [2,3]
     *   * * *    [0,2]   [1,2]    [2,2]
     *    * * *     [0,1]     [1,1]    [2,1]
     *   * * *    [0,0]   [1,0]    [2,0]
     *
     */

    struct PointyTopNeibersOp
    {
        const static inline int offsetEvenX[6] = {0, 1, 0, -1, -1, -1};
        const static inline int offsetEvenY[6] = {-1, 0, 1, 1, 0, -1};
        const static inline int offsetOddX[6] = {1, 1, 1, 0, -1, 0};
        const static inline int offsetOddY[6] = {-1, 0, 1, 1, 0, -1};

        void operator()(int x, int y, CellKey::Offset *neibers)
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
    struct FlatTopNeibersOp
    {

        const inline static int offsetEvenX[6] = {-1, 0, 1, 1, 0, -1};
        const inline static int offsetEvenY[6] = {-1, -1, -1, 0, 1, -1};
        const inline static int offsetOddX[6] = {-1, 0, 1, 1, 0, -1};
        const inline static int offsetOddY[6] = {0, -1, 0, 1, 1, 1};

        void operator()(int x, int y, CellKey::Offset *neibers)
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
        using LayoutType = uint8_t;

        // horizontal odd row move half step to right.
        static constexpr LayoutType PointyTop = 0; // pointy top.
        // vertical odd col move half step up.
        static constexpr LayoutType FlatTop = 1; // flat top.

        template <LayoutType T>
        static void getNeibers(int x, int y, CellKey::Offset *neibers)
        {
            static_assert(false, "un-supported topology");
        }

        template <>
        static void getNeibers<PointyTop>(int x, int y, CellKey::Offset *neibers)
        {
            static PointyTopNeibersOp neiber;
            return neiber(x, y, neibers);
        }

        template <>
        static void getNeibers<FlatTop>(int x, int y, CellKey::Offset *neibers)
        {
            static FlatTopNeibersOp neiber;
            return neiber(x, y, neibers);
        }
    };

};
