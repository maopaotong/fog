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
#include <fg/util/Box2.h>

#include "CollectionUtil.h"
#include "Transform.h"
#include "TemplateUtil.h"

// using namespace Ogre;

namespace fog
{

    struct Cell
    {
        using Layout = int;
        using System = uint8_t;
        using Parity = int;
        using Direction = int;

        static constexpr Layout PointyTop = 0;
        static constexpr Layout FlatTop = 1;
        static constexpr System Offset = 0;
        static constexpr System Axial = 1;
        static constexpr System Centre = 2;
        static constexpr System Cartesian = 3;

        static constexpr Parity Even = 0;
        static constexpr Parity Odd = 1;

        template <System sys>
        struct HashOp;
        struct OffsetKey
        {
            using Hash = Cell::HashOp<Cell::Offset>;
            int row;
            int col;
            const int *ptr() const { return &row; }
            OffsetKey()
            {
            }

            OffsetKey(const OffsetKey &cKey)
            {
                col = cKey.col;
                row = cKey.row;
            }
            OffsetKey(const OffsetKey &&cKey)
            {
                col = cKey.col;
                row = cKey.row;
            }
            OffsetKey &operator=(const OffsetKey &cKey)
            {
                col = cKey.col;
                row = cKey.row;
                return *this;
            }
            OffsetKey &operator=(const OffsetKey &&cKey)
            {
                col = cKey.col;
                row = cKey.row;
                return *this;
            }

        private:
            OffsetKey(int x, int y) : col(x), row(y)
            {
            }

        public:
            static OffsetKey colRow(int col, int row)
            {
                return OffsetKey(col, row);
            }

            static OffsetKey rowCol(int row, int col)
            {
                return OffsetKey(col, row);
            }

            static OffsetKey colRow(std::tuple<int, int> colRow)
            {
                return OffsetKey(std::get<0>(colRow), std::get<1>(colRow));
            }

            bool operator==(const OffsetKey &ck) const
            {
                return ptr()[0] == ck.ptr()[0] && ptr()[1] == ck.ptr()[1];
            }
            bool operator!=(const OffsetKey &ck) const
            {
                return !operator==(ck);
            }

            OffsetKey operator+(OffsetKey cKey1)
            {
                return OffsetKey::colRow(col + cKey1.col, row + cKey1.row);
            }
        };

        struct AxialKey
        {
            int q;
            int r;
            const int *ptr() const { return &q; }
            AxialKey()
            {
            }
            AxialKey(int q, int r) : q(q), r(r)
            {
            }
            AxialKey(std::tuple<int, int> xy) : q(std::get<0>(xy)), r(std::get<1>(xy))
            {
            }
            bool operator==(const OffsetKey &ck) const
            {
                return ptr()[0] == ck.ptr()[0] && ptr()[1] == ck.ptr()[1];
            }
            bool operator!=(const OffsetKey &ck) const
            {
                return !operator==(ck);
            }
        };

        template <Layout layout, Parity parity>
        struct NeibersDelta;
        template <>
        struct NeibersDelta<PointyTop, Even>
        {
            constexpr static std::tuple<int, int> deltaKey[6] = {
                {+1, 0},
                {0, +1},
                {-1, +1},
                {-1, 0},
                {-1, -1},
                {0, -1},
            };
        };

        template <>
        struct NeibersDelta<PointyTop, Odd>
        {

            constexpr static std::tuple<int, int> deltaKey[6] = {
                {+1, 0},
                {+1, +1},
                {0, +1},
                {-1, 0},
                {0, -1},
                {+1, -1},
            };
        };

        template <>
        struct NeibersDelta<FlatTop, Even>
        {
            constexpr static std::tuple<int, int> deltaKey[6] = {
                {-1, 0},
                {-1, -1},
                {0, -1},
                {+1, -1},
                {+1, 0},
                {0, +1},
            };
        };

        template <>
        struct NeibersDelta<FlatTop, Odd>
        {

            constexpr static std::tuple<int, int> deltaKey[6] = {
                {-1, +1},
                {-1, 0},
                {0, -1},
                {+1, 0},
                {+1, +1},
                {0, +1},
            };
        };

        template <Layout layout>
        struct LayoutInfo
        {
            constexpr static int type = layout;
        };

        template <>
        struct LayoutInfo<PointyTop>
        {
            constexpr static int type = PointyTop;
            constexpr static float innerRad = 1.0;
            constexpr static float outerRad = 2 / Math::SQRT3;
            constexpr static float cellWidth = 2;
            constexpr static float cellHeight = outerRad * 2;
            constexpr static float unitHeight = cellHeight * 3 / 4; // (2/sqrt3) * 2 *3 / 4 = sqrt3;
            constexpr static float unitWidth = cellWidth;
            constexpr static int qDegree = -30;
            constexpr static int qZLDegree = qDegree + 90;
            constexpr static int rDegree = qDegree + 120; //=90
            constexpr static int rZLDegree = rDegree + 90;
            constexpr static float axialUnit = Math::SQRT3;
            constexpr static int unitCols = 2;
            constexpr static int unitRows = 3;

        };

        template <>
        struct LayoutInfo<FlatTop>
        {
            constexpr static int type = FlatTop;
            constexpr static float innerRad = 1.0;
            constexpr static float outerRad = 2 / Math::SQRT3;
            constexpr static float cellWidth = outerRad * 2;
            constexpr static float cellHeight = 2;
            constexpr static float unitHeight = cellHeight;
            constexpr static float unitWidth = cellWidth * 3 / 4;
            constexpr static int qDegree = 0;
            constexpr static int qZLDegree = qDegree + 90;
            constexpr static int rDegree = qDegree + 120;
            constexpr static int rZLDegree = rDegree + 90;
            constexpr static float axialUnit = Math::SQRT3;
            constexpr static int unitCols = 3;
            constexpr static int unitRows = 2;
        };

        template <System sys>
        struct SystemInfo;
        template <>
        struct SystemInfo<Cartesian>
        {
            using type = Point2<float>;
            using tuple = std::tuple<float, float>;
        };
        template <>
        struct SystemInfo<Centre>
        {
            using type = Point2<float>;

            using tuple = std::tuple<float, float>;
        };

        template <>
        struct SystemInfo<Offset>
        {
            using type = OffsetKey;

            using tuple = std::tuple<int, int>;
        };
        template <>
        struct SystemInfo<Axial>
        {
            using type = AxialKey;
            using tuple = std::tuple<int, int>;
        };

        // // layout info
        // const static inline LayoutInfo<PointyTop> PointyTopInfo;
        // const static inline LayoutInfo<FlatTop> FlatTopInfo;
        // // system info
        // const static inline SystemInfo<Cartesian> CartesianSysInfo;
        // const static inline SystemInfo<Offset> OffsetSysInfo;
        // const static inline SystemInfo<Axial> AxialInfo;

        template <System sys>
        struct HashOp
        {
            std::size_t operator()(const typename SystemInfo<sys>::type &k) const
            {
                using tType = SystemInfo<sys>::tuple;
                using eType = std::tuple_element_t<0, tType>;
                auto h1 = std::hash<eType>{}(*k.ptr());
                auto h2 = std::hash<eType>{}(*(k.ptr() + 1));
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        template <Layout layout>
        struct Transform
        {

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Centre && s2 == Offset, Cell::SystemInfo<Offset>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
            {

                constexpr float unitHeight = Cell::LayoutInfo<layout>::unitHeight;
                constexpr float unitWidth = Cell::LayoutInfo<layout>::unitWidth;
                float fx = cKey1.x / unitWidth;
                float fy = cKey1.y / unitHeight;
                int col;
                int row;
                if (constexpr(layout == Cell::PointyTop))
                {

                    row = std::round(fy);
                    fx -= row % 2 == 0 ? 0.0 : 0.5;

                    col = std::round(fx);
                }
                else
                {
                    col = std::round(fx);
                    fy -= col % 2 == 0 ? 0.0 : 0.5;
                    row = std::round(fy);
                }
                return Cell::OffsetKey::colRow(col, row);
            }
            template <System s1, System s2>
            static
                typename std::enable_if_t<s1 == Offset && s2 == Centre, Cell::SystemInfo<Centre>::type>
                transform(const typename Cell::SystemInfo<Offset>::type &cKey1)
            {
                constexpr float unitHeight = LayoutInfo<layout>::unitHeight;
                constexpr float unitWidth = LayoutInfo<layout>::unitWidth;

                // innerradius = 1.0f;
                int x = cKey1.col;
                int y = cKey1.row;

                float fx;
                float fy;
                if (constexpr(layout == Cell::PointyTop))
                {
                    fx = x * unitWidth + (y % 2 == 0 ? 0 : unitWidth / 2.0);
                    fy = y * unitHeight;
                }
                else if (constexpr(layout == Cell::FlatTop))
                {
                    fx = x * unitWidth;
                    fy = y * unitHeight + (x % 2 == 0 ? 0 : unitHeight / 2.0);
                }

                return Point2<float>(fx, fy);
            }
            /**   PointyTop(V1)                  FlatTop                   PointyTop(V2)
             *    . . . . .-3. . . . . .        . . . . . . . . . . .      . . . . . .r3. . . . . .
             *    . . . . .-2 . . . .q2       r2. . . . . . . . . . .      -2. . . . .r2. . . . . .
             *    . . . . .-1 .q1 . . .         . .r1 . . . . . . . .      . . . -1. .r1. . . . . .
             *    . . . . .00 . . . . .         . . . . .00q1q2 3 4 5      . . . . . .00. . . . . .
             *    . .-1 . .r1 . . . . .         . .s1 . . . . . . . .      . . . . . .-1. .q1 . . .
             *  -2. . . . .r2 . . . . .       s2. . . . . . . . . . .      . . . . . .-2. . . . q2.
             *    . . . . .r3 . . . . .         . . . . . . . . . . .      . . . . . .-2. . . . . .
             *
             */
            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Axial && s2 == Offset, Cell::SystemInfo<Offset>::type> transform(const typename Cell::SystemInfo<Axial>::type &cKey1)
            {
                int q = cKey1.q;
                int r = cKey1.r;
                int row;
                int col;
                if (constexpr(layout == Cell::PointyTop))
                {
                    col = q + (r - (r & 1)) / 2;
                    row = r;
                }
                else if (constexpr(layout == Cell::FlatTop))
                {
                    col = q;
                    row = r + (q - (q & 1)) / 2;
                }
                return OffsetKey::colRow(col, row);
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Offset && s2 == Axial, Cell::SystemInfo<Axial>::type> transform(const typename Cell::SystemInfo<Offset>::type &cKey1)
            {
                int col = cKey1.col;
                int row = cKey1.row;
                int r;
                int q;
                if (constexpr(layout == Cell::PointyTop))
                {
                    q = col - (row - (row & 1)) / 2;
                    r = row;
                }
                else if (constexpr(layout == Cell::FlatTop))
                {
                    q = col;
                    r = row - (col - (col & 1)) / 2;
                }
                return AxialKey(q, r);
            }
            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Cartesian && s2 == Offset, Cell::SystemInfo<Offset>::type> transform(const typename Cell::SystemInfo<Cartesian>::type &cKey1)
            {
                Cell::AxialKey cKey2 = transform<Cartesian, Axial>(cKey1);
                return transform<Axial, Offset>(cKey2);
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Centre && s2 == Cartesian, Cell::SystemInfo<Cartesian>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
            {
                return Point2<float>(cKey1.x, cKey1.y);
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Cartesian && s2 == Axial, Cell::SystemInfo<Axial>::type> transform(const typename Cell::SystemInfo<Cartesian>::type &cKey1)
            {
                const static float axialUnit = LayoutInfo<layout>::axialUnit;
                constexpr int qDegree = LayoutInfo<layout>::qDegree;
                constexpr int rDegree = LayoutInfo<layout>::rDegree;

                float x = cKey1.x;
                float y = cKey1.y;
                Point2<float> p(x, y);

                float qDistance = p.rotateAndGetX<-qDegree>(); // distance to the number line of .
                float rDistance = p.rotateAndGetX<-rDegree>(); // distance to the number line of .

                float fq = qDistance / axialUnit;
                float fr = rDistance / axialUnit;

                return Cell::AxialKey(cubeRound(fq, fr));
            }
        };

        static std::tuple<int, int> cubeRound(float fq, float fr)
        {
            const float cx = fq;
            const float cz = fr;
            const float cy = -cx - cz;

            // Step 3: cube rounding to nearest hex
            int rx = (int)std::round(cx);
            int ry = (int)std::round(cy);
            int rz = (int)std::round(cz);

            float dx = std::abs(rx - cx);
            float dy = std::abs(ry - cy);
            float dz = std::abs(rz - cz);

            if (dx > dy && dx > dz)
            {
                rx = -ry - rz;
            }
            else if (dy > dz)
            {
                ry = -rx - rz;
            }
            else
            {
                rz = -rx - ry;
            }
            return {rx, rz};
        };
        // using PointyCentre = SystemInfo<Centre>::type;//Point2<float>;

        template <Layout layout, System s>
        static typename std::enable_if_t<s == Offset, std::array<SystemInfo<Offset>::type, 6>> getNeighbors(SystemInfo<Offset>::type &cKey1)
        {
            std::array<SystemInfo<Offset>::type, 6> ret;
            for (int i = 0; i < 6; i++)
            {
                ret[i] = getNeibor<layout, s>(cKey1, i);
            }
            return ret;
        }
        template <Layout layout, System s>
        static typename std::enable_if_t<s == Offset, SystemInfo<Offset>::type> getNeibor(SystemInfo<Offset>::type &cKey1, Direction direction)
        {
            int col = cKey1.col;
            int row = cKey1.row;
            if (constexpr(layout == PointyTop))
            {
                if (row % 2 == 0)
                {
                    return cKey1 + OffsetKey::colRow(Cell::NeibersDelta<layout, Cell::Even>::deltaKey[direction]);
                }
                else
                {
                    return cKey1 + OffsetKey::colRow(Cell::NeibersDelta<layout, Cell::Odd>::deltaKey[direction]);
                }
            }
            else if (constexpr(layout == FlatTop))
            {
                if (col % 2 == 0)
                {
                    return cKey1 + OffsetKey::colRow(Cell::NeibersDelta<layout, Cell::Even>::deltaKey[direction]);
                }
                else
                {
                    return cKey1 + OffsetKey::colRow(Cell::NeibersDelta<layout, Cell::Odd>::deltaKey[direction]);
                }
            }

            //
        }
    };

    using CellKey = Cell::OffsetKey;
    // TODO configurable CellLayout.
    // static constexpr Cell::Layout CellLayout = Cell::PointyTop;
    static constexpr Cell::Layout CellLayout = Cell::FlatTop;
    using CellTransform = Cell::Transform<CellLayout>;
    // using Hash = Cell::HashOp<Cell::Offset>;

    static constexpr float unitHeight = Cell::LayoutInfo<CellLayout>::unitHeight;
    static constexpr float unitWidth = Cell::LayoutInfo<CellLayout>::unitWidth;
    static constexpr float cellWidth = Cell::LayoutInfo<CellLayout>::cellWidth;
    static constexpr float cellHeight = Cell::LayoutInfo<CellLayout>::cellHeight;

};
