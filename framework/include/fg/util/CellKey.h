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
#include "TemplateUtil.h"

// using namespace Ogre;

namespace fog
{

    struct Cell
    {
        using Layout = int;
        using System = uint8_t;

        static constexpr Layout PointyTop = 0;
        static constexpr Layout FlatTop = 1;
        static constexpr System Offset = 0;
        static constexpr System Axial = 1;
        static constexpr System Centre = 2;
        static constexpr System Cartesian = 3;
        template <System sys>
        struct HashOp;
        struct OffsetKey
        {
            using Hash = Cell::HashOp<Cell::Offset>;
            int x;
            int y;
            const int *ptr() const { return &x; }
            OffsetKey()
            {
            }
            OffsetKey(int x, int y) : x(x), y(y)
            {
            }
            OffsetKey(std::tuple<int, int> xy) : x(std::get<0>(xy)), y(std::get<1>(xy))
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
            constexpr static float unitHeight = cellHeight * 3 / 4;// (2/sqrt3) * 2 *3 / 4 = sqrt3; 
            constexpr static float unitWidth = cellWidth;
            constexpr static int qDegree = -30;     
            constexpr static int qZLDegree = qDegree + 90;
            constexpr static int rDegree = qDegree + 120 ; //=90
            constexpr static int rZLDegree = rDegree + 90;
            constexpr static float axialUnit = Math::SQRT3;
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
                return Cell::OffsetKey(col, row);
            }
            template <System s1, System s2>
            static
                typename std::enable_if_t<s1 == Offset && s2 == Centre, Cell::SystemInfo<Centre>::type>
                transform(const typename Cell::SystemInfo<Offset>::type &cKey1)
            {
                constexpr float unitHeight = LayoutInfo<layout>::unitHeight;
                constexpr float unitWidth = LayoutInfo<layout>::unitWidth;

                // innerradius = 1.0f;
                int x = cKey1.x;
                int y = cKey1.y;

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
            static typename std::enable_if_t<s1 == Cartesian && s2 == Offset, Cell::SystemInfo<Offset>::type> transform(const typename Cell::SystemInfo<Cartesian>::type &cKey1)
            {
                Cell::AxialKey cKey2 = transform<Cartesian, Axial>(cKey1);
                int q = cKey2.q;
                int r = cKey2.r;
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
                return OffsetKey(col, row);
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
        }
        // using PointyCentre = SystemInfo<Centre>::type;//Point2<float>;
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
