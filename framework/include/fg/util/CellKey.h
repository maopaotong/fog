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
            constexpr static float rowHeight = cellHeight * 3 / 4;
            constexpr static float colWidth = cellWidth;
            constexpr static int qDegree = 30;
            constexpr static int rDegree = 270; //=-90
            constexpr static float qUnit = Math::SQRT3;
        };

        template <>
        struct LayoutInfo<FlatTop>
        {
            constexpr static int type = FlatTop;
            constexpr static float innerRad = 1.0;
            constexpr static float outerRad = 2 / Math::SQRT3;
            constexpr static float cellWidth = outerRad * 2;
            constexpr static float cellHeight = 2;
            constexpr static float rowHeight = cellHeight;
            constexpr static float colWidth = cellWidth * 3 / 4;
            constexpr static int qDegree = 0;
            constexpr static int rDegree = 120;
            constexpr static float qUnit = Math::SQRT3;
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

        // layout info
        const static inline LayoutInfo<PointyTop> PointyTopInfo;
        const static inline LayoutInfo<FlatTop> FlatTopInfo;
        // system info
        const static inline SystemInfo<Cartesian> CartesianSysInfo;
        const static inline SystemInfo<Offset> OffsetSysInfo;
        const static inline SystemInfo<Axial> AxialInfo;

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

        using PointyCentre = Point2<float>;
    };

    using CellKey = Cell::OffsetKey;
    using Hash = Cell::HashOp<Cell::Offset>;
};
