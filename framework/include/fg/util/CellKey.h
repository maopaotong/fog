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
        static constexpr LayoutType Offset_PointyTop = 0; // pointy top.
        // vertical odd col move half step up.
        static constexpr LayoutType Offset_FlatTop = 1; // flat top.

        //using Type = uint8_t;
        // static constexpr Type OFFSET = 0;
        // static constexpr Type AXIAL = 1;
        // static constexpr Type CENTRE = 2;
        template<LayoutType>
        struct OffsetBase;
        using OffsetPointy = OffsetBase<Offset_PointyTop>;
        using OffsetFlat = OffsetBase<Offset_FlatTop>;

        struct Axial;
        struct Centre;
        using OC = std::tuple<OffsetPointy,Centre>;
        using CO = std::tuple<Centre, OffsetPointy>;
        // Note: for the reason of template impl limitation, the order is not what you see: Offset => Centre => Point2<float>
        using OCP = std::tuple<OffsetPointy, Point2<float>, Centre>;

        struct Hash
        {
            std::size_t operator()(const Point2<int> &p) const
            {
                auto h1 = std::hash<int>{}(p.x);
                auto h2 = std::hash<int>{}(p.y);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };


    
        template<LayoutType layout>
        struct OffsetBase
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
            OffsetBase() : OffsetBase(0, 0) {}
            OffsetBase(int x, int y) : x(x), y(y)
            {
            }

            bool operator==(const OffsetBase &ck) const
            {
                return this->x == ck.x && this->y == ck.y;
            }
            bool operator!=(const OffsetBase &ck) const
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





        // template <typename K>
        // Box2<float> getBoxIn2D(K &cKey)
        // {
        //     // Vector2 centre = getOrigin2D(cKey);
        //     Point2<float> centre = CellsTransform::transform<K, CellKey::Centre>(cKey);
        //     float left = centre.x - 1.0; // rad
        //     float bottom = centre.y - std::sqrt(3.0) / 2.0;
        //     return Box2<float>(left, bottom, left + 2.0, bottom + std::sqrt(3.0));
        // }

        // template <typename K>
        // static Box2<float> getOuterBoxIn2D(K &cKey)
        // {
        //     // Vector2 centre = getOrigin2D(cKey);

        //     Point2<float> centre = CellsTransform::transform<K, CellKey::Centre>(cKey);

        //     float left = centre.x - 1.0;                    // rad
        //     float bottom = centre.y - 2.0 / std::sqrt(3.0); // R
        //     return Box2<float>(left, bottom, left + 2.0, bottom + 4.0 / std::sqrt(3.0));
        // }

        // template <typename K>
        // static Box2<float> getBoxInUV(K &cKey, int width, int height)
        // {
        //     Box2<float> r = getBoxIn2D<K>(cKey);
        //     return r.transform(Transform::D2CellWorldUV(width, height));
        // }

        // template <typename K>
        // static Box2<float> getOuterBoxInUV(K &cKey, int width, int height)
        // {
        //     Box2<float> r = getOuterBoxIn2D<K>(cKey);
        //     return r.transform(Transform::D2CellWorldUV(width, height));
        // }

        // //
        // template <typename Tuple>
        // static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 2, std::vector<std::tuple_element_t<1, Tuple>>>
        // transformAll(std::vector<std::tuple_element_t<0, Tuple>> &cks)
        // {
        //     using K1 = std::tuple_element_t<0, Tuple>;
        //     using K2 = std::tuple_element_t<1, Tuple>;
        //     std::vector<K2> ret;

        //     CollectionUtil::transform<K1, K2>(cks, ret, [](K &ck)
        //                                       { return CellsTransform::transform<Tuple>(ck); });
        //     return ret;
        // }

        // template <typename Tuple>
        // static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 3, std::vector<std::tuple_element_t<1, Tuple>>>
        // transformAll(std::vector<std::tuple_element_t<0, Tuple>> &cks)
        // {
        //     using K1 = std::tuple_element_t<0, Tuple>;
        //     using K3 = std::tuple_element_t<2, Tuple>;
        //     using K2 = std::tuple_element_t<1, Tuple>;

        //     std::vector<K3> cks3;

        //     CollectionUtil::transform<K1, K3>(cks, cks3, [](K1 &ck)
        //                                       { return CellsTransform::transform<K1, K3>(ck); });

        //     std::vector<K2> ret;
        //     CollectionUtil::transform<K3, K2>(cks3, ret, [](K3 &ck)
        //                                       { return CellsTransform::transform<K3, K2>(ck); });
        //     return ret;
        // }

        // // transform tuple easy to call.
        // template <typename Tuple>
        // static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 2, std::tuple_element_t<1, Tuple>>
        // transform(const std::tuple_element_t<0, Tuple> &cKey1)
        // {
        //     using K1 = std::tuple_element_t<0, Tuple>;
        //     using K2 = std::tuple_element_t<1, Tuple>;
        //     return transform<K1, K2>(cKey1);
        // }

        // template <typename Tuple>
        // static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 3, std::tuple_element_t<1, Tuple>>
        // transform(const std::tuple_element_t<0, Tuple> &cKey1)
        // {
        //     using K1 = std::tuple_element_t<0, Tuple>;
        //     using K3 = std::tuple_element_t<2, Tuple>;
        //     using K2 = std::tuple_element_t<1, Tuple>;

        //     K3 k3 = transform<K1, K3>(cKey1);
        //     return transform<K3, K2>(k3);
        // }

        // // transform impl
        // template <typename K1, typename K2>
        // static K2 transform(const K1 &cKey1)
        // {
        //     static_assert(!std::is_same(K1, K1), "not supported transform between types.");
        // }

        // template <>
        // static Centre transform<OffsetPointy, Centre>(const OffsetPointy &cKey1)
        // {
        //     static Transform::CellKeyToCentre TF_K2C;

        //     return Point2<float>(cKey1.x, cKey1.y).transform(TF_K2C);
        // }

        // template <>
        // static OffsetPointy transform<Centre, OffsetPointy>(const Centre &cKey1)
        // {
        //     static Transform::CentreToCellKey TF_C2K;
        //     Point2<float> pC = Point2<float>(cKey1.x, cKey1.y).transform(TF_C2K);
        //     return CellKey::OffsetPointy(std::round(pC.x), std::round(pC.y));
        // }

        // template <>
        // static Point2<float> transform<Centre, Point2<float>>(const Centre &cKey1)
        // {
        //     return Point2<float>(cKey1.x, cKey1.y);
        // }
    };

};
