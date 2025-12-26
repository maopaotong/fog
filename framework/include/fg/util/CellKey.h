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

    struct CellKey
    {
        using LayoutType = uint8_t;

        // horizontal odd row move half step to right.
        static constexpr LayoutType PointyTop = 0; // pointy top.
        // vertical odd col move half step up.
        static constexpr LayoutType FlatTop = 1; // flat top.

        template <LayoutType>
        struct Offset;
        using OffsetPointy = Offset<PointyTop>;
        using OffsetFlat = Offset<FlatTop>;

        struct Axial;
        struct Centre;

        struct Hash
        {
            std::size_t operator()(const Point2<int> &p) const
            {
                auto h1 = std::hash<int>{}(p.x);
                auto h2 = std::hash<int>{}(p.y);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        template <LayoutType layout>
        struct Offset
        {
            struct Hash
            {
                std::size_t operator()(const OffsetPointy &p) const
                {
                    auto h1 = std::hash<int>{}(p.x);
                    auto h2 = std::hash<int>{}(p.y);
                    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
                }
            };
            int x;
            int y;
            Offset() : Offset(0, 0) {}
            Offset(int x, int y) : x(x), y(y)
            {
            }

            bool operator==(const Offset &ck) const
            {
                return this->x == ck.x && this->y == ck.y;
            }
            bool operator!=(const Offset &ck) const
            {
                return !operator==(ck);
            }
        };

        struct Axial
        {
            int q;
            int r;
            Axial() : Axial(0, 0) {}
            Axial(int q, int r) : q(q), r(r)
            {
            }

            bool operator==(const Axial &ck) const
            {
                return this->q == ck.q && this->r == ck.r;
            }
        };

        struct Centre : public Point2<float>
        {
            Centre() : Centre(-1, -1) {}
            Centre(float x, float y) : Point2<float>(x, y)
            {
            }
            Centre(const Point2<float> cKey) : Point2<float>(cKey.x, cKey.y)
            {
            }
        };

        struct Pos3D : public Point3<float>
        {
        };
    };

};
