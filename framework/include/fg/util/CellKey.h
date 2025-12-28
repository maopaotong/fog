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
        static constexpr System Cartesian = 2;

        template <Layout layout>
        struct LayoutInfo
        {
            constexpr static int type = layout;
            // constexpr static float innerRad;
            // constexpr static float outerRad;
            // constexpr static float cellWidth;
            // constexpr static float cellHeight;
            // constexpr static float rowHeight;
            // constexpr static float colWidth;            
            // constexpr static float qDegree;
            // constexpr static float rDegree;
            // constexpr static float qUnit;
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

        const static inline LayoutInfo<PointyTop> PointyTopInfo;
        const static inline LayoutInfo<FlatTop> FlatTopInfo;

        template <typename, Layout, System>
        struct Key;
        using PointyCentre = Key<float, Cell::PointyTop, Cartesian>;
        using FlatCentre = Key<float, Cell::FlatTop, Cartesian>;
        using PointyOffset = Cell::Key<int, Cell::PointyTop, Offset>;
        using FlatOffset = Cell::Key<int, Cell::FlatTop, Offset>;
        using PointyAxial = Cell::Key<int, Cell::PointyTop, Axial>;
        using FlatAxial = Cell::Key<int, Cell::FlatTop, Axial>;

        template <typename T, Layout layout, System sys>
        struct HashOp
        {
            std::size_t operator()(const Key<T, layout, sys> &k) const
            {
                auto h1 = std::hash<T>{}(*k.ptr());
                auto h2 = std::hash<T>{}(*(k.ptr() + 1));
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        template <typename T, Layout layout, System sys>
        struct Key
        {
            using Hash = HashOp<T, layout, sys>;
            using LayoutInfo = LayoutInfo<layout>;

        protected:
            T data[2];
            T *ptr() const
            {
                return data;
            }
            friend struct Hash;

        public:
            bool operator==(const Key &ck) const
            {
                return ptr()[0] == ck.ptr()[0] && ptr()[1] == ck.ptr()[1];
            }
            bool operator!=(const Key &ck) const
            {
                return !operator==(ck);
            }
        };

        template <typename T, Layout layout>
        struct Key<T, layout, Offset>
        {
            using Hash = HashOp<T, layout, Offset>;

            T x;
            T y;
            const T *ptr() const { return &x; }
            Key()
            {
            }
            Key(T x, T y) : x(x), y(y)
            {
            }
            Key(std::tuple<T, T> xy) : x(std::get<0>(xy)), y(std::get<1>(xy))
            {
            }
            bool operator==(const Key<T, layout, Offset> &ck) const
            {
                return ptr()[0] == ck.ptr()[0] && ptr()[1] == ck.ptr()[1];
            }
            bool operator!=(const Key<T, layout, Offset> &ck) const
            {
                return !operator==(ck);
            }
        };

        template <typename T, Layout layout>
        struct Key<T, layout, Cartesian>
        {
            using Hash = HashOp<T, layout, Cartesian>;

            T x;
            T y;
            const T *ptr() const { return &x; }
            Key()
            {
            }
            Key(T x, T y) : x(x), y(y)
            {
            }
            Key(std::tuple<T, T> xy) : x(std::get<0>(xy)), y(std::get<1>(xy))
            {
            }
            bool operator==(const Key<T, layout, Cartesian> &ck) const
            {
                return ptr()[0] == ck.ptr()[0] && ptr()[1] == ck.ptr()[1];
            }
            bool operator!=(const Key<T, layout, Cartesian> &ck) const
            {
                return !operator==(ck);
            }
        };

        template <typename T, Layout layout>
        struct Key<T, layout, Axial>
        {
            T q;
            T r;
            const T *ptr() const { return &q; }
            Key(T q, T r) : q(q), r(r)
            {
            }
            Key(std::tuple<T, T> qr) : q(std::get<0>(qr)), r(std::get<1>(qr))
            {
            }
            bool operator==(const Key<T, layout, Axial> &ck) const
            {
                return ptr()[0] == ck.ptr()[0] && ptr()[1] == ck.ptr()[1];
            }
            bool operator!=(const Key<T, layout, Axial> &ck) const
            {
                return !operator==(ck);
            }
        };
    };

    using CellKey = Cell::Key<int, Cell::PointyTop, Cell::Offset>;
};
