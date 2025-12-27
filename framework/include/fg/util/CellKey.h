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
        using LayoutType = uint8_t;
        using System = uint8_t;

        // horizontal odd row move half step to right.
        static constexpr LayoutType PointyTopOddRow = 0; // pointy top.
        // vertical odd col move half step up.
        static constexpr LayoutType FlatTopOddCol = 1; // flat top.
        static constexpr LayoutType Q0 = 2;            // Q <=> X , R <=> -X - bY
        static constexpr LayoutType Q30 = 3;           // Q <=> X + (bY), R <=> 0X - bY

        static constexpr System OffsetSys = 0;
        static constexpr System AxialSys = 1;

        template <System, typename, LayoutType>
        struct Key;
        using Centre = Key<OffsetSys, float, PointyTopOddRow>;
        using OffsetPointy = Cell::Key<Cell::OffsetSys, int, Cell::PointyTopOddRow>;
        using OffsetFlat = Cell::Key<Cell::OffsetSys, int, Cell::FlatTopOddCol>;

        template <System sys, typename T, LayoutType layout>
        struct HashOp
        {
            std::size_t operator()(const Key<sys, T, layout> &k) const
            {
                auto h1 = std::hash<T>{}(*k.ptr());
                auto h2 = std::hash<T>{}(*(k.ptr() + 1));
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        template <System sys, typename T, LayoutType layout>
        struct Key
        {
            using Hash = HashOp<sys, T, layout>;

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

        template <typename T, LayoutType layout>
        struct Key<OffsetSys, T, layout>
        {
            using Hash = HashOp<OffsetSys, T, layout>;

            T x;
            T y;
            const T *ptr() const { return &x; }
            Key(){

            }
            Key(T x, T y) : x(x), y(y)
            {
            }
            Key(std::tuple<T, T> xy) : x(std::get<0>(xy)), y(std::get<1>(xy))
            {
            }
            bool operator==(const Key<OffsetSys, T, layout> &ck) const
            {
                return ptr()[0] == ck.ptr()[0] && ptr()[1] == ck.ptr()[1];
            }
            bool operator!=(const Key<OffsetSys, T, layout> &ck) const
            {
                return !operator==(ck);
            }
        };

        template <typename T, LayoutType layout>
        struct Key<AxialSys, T, layout>
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
            bool operator==(const Key<AxialSys, T, layout> &ck) const
            {
                return ptr()[0] == ck.ptr()[0] && ptr()[1] == ck.ptr()[1];
            }
            bool operator!=(const Key<AxialSys, T, layout> &ck) const
            {
                return !operator==(ck);
            }
        };
    };
    using CellKey = Cell::Key<Cell::OffsetSys, int, Cell::PointyTopOddRow>;
};
