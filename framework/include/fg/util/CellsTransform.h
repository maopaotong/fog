/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "CellKey.h"

// using namespace Ogre;

namespace fog
{

    struct CellsTransform
    {

        
        template <typename K>
        Box2<float> getBoxIn2D(K &cKey)
        {
            // Vector2 centre = getOrigin2D(cKey);
            Point2<float> centre = CellsTransform::transform<K, CellKey::Centre>(cKey);
            float left = centre.x - 1.0; // rad
            float bottom = centre.y - std::sqrt(3.0) / 2.0;
            return Box2<float>(left, bottom, left + 2.0, bottom + std::sqrt(3.0));
        }

        template <typename K>
        static Box2<float> getOuterBoxIn2D(K &cKey)
        {
            // Vector2 centre = getOrigin2D(cKey);

            Point2<float> centre = CellsTransform::transform<K, CellKey::Centre>(cKey);

            float left = centre.x - 1.0;                    // rad
            float bottom = centre.y - 2.0 / std::sqrt(3.0); // R
            return Box2<float>(left, bottom, left + 2.0, bottom + 4.0 / std::sqrt(3.0));
        }

        template <typename K>
        static Box2<float> getBoxInUV(K &cKey, int width, int height)
        {
            Box2<float> r = getBoxIn2D<K>(cKey);
            return r.transform(Transform::D2CellWorldUV(width, height));
        }

        template <typename K>
        static Box2<float> getOuterBoxInUV(K &cKey, int width, int height)
        {
            Box2<float> r = getOuterBoxIn2D<K>(cKey);
            return r.transform(Transform::D2CellWorldUV(width, height));
        }

        //
        template <typename Tuple>
        static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 2, std::vector<std::tuple_element_t<1, Tuple>>>
        transformAll(std::vector<std::tuple_element_t<0, Tuple>> &cks)
        {
            using K1 = std::tuple_element_t<0, Tuple>;
            using K2 = std::tuple_element_t<1, Tuple>;
            std::vector<K2> ret;

            CollectionUtil::transform<K1, K2>(cks, ret, [](K &ck)
                                              { return CellsTransform::transform<Tuple>(ck); });
            return ret;
        }

        template <typename Tuple>
        static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 3, std::vector<std::tuple_element_t<1, Tuple>>>
        transformAll(std::vector<std::tuple_element_t<0, Tuple>> &cks)
        {
            using K1 = std::tuple_element_t<0, Tuple>;
            using K3 = std::tuple_element_t<2, Tuple>;
            using K2 = std::tuple_element_t<1, Tuple>;

            std::vector<K3> cks3;

            CollectionUtil::transform<K1, K3>(cks, cks3, [](K1 &ck)
                                              { return CellsTransform::transform<K1, K3>(ck); });

            std::vector<K2> ret;
            CollectionUtil::transform<K3, K2>(cks3, ret, [](K3 &ck)
                                              { return CellsTransform::transform<K3, K2>(ck); });
            return ret;
        }

        // transform tuple easy to call.
        template <typename Tuple>
        static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 2, std::tuple_element_t<1, Tuple>>
        transform(const std::tuple_element_t<0, Tuple> &cKey1)
        {
            using K1 = std::tuple_element_t<0, Tuple>;
            using K2 = std::tuple_element_t<1, Tuple>;
            return transform<K1, K2>(cKey1);
        }

        template <typename Tuple>
        static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 3, std::tuple_element_t<1, Tuple>>
        transform(const std::tuple_element_t<0, Tuple> &cKey1)
        {
            using K1 = std::tuple_element_t<0, Tuple>;
            using K3 = std::tuple_element_t<2, Tuple>;
            using K2 = std::tuple_element_t<1, Tuple>;

            K3 k3 = transform<K1, K3>(cKey1);
            return transform<K3, K2>(k3);
        }

        // transform impl
        template <typename K1, typename K2>
        static K2 transform(const K1 &cKey1)
        {
            static_assert(!std::is_same(K1, K1), "not supported transform between types.");
        }

        template <>
        static CellKey::Centre transform<CellKey::OffsetPointy, CellKey::Centre>(const CellKey::OffsetPointy &cKey1)
        {
            static Transform::CellKeyToCentre TF_K2C;

            return Point2<float>(cKey1.x, cKey1.y).transform(TF_K2C);
        }

        template <>
        static CellKey::OffsetPointy transform<CellKey::Centre, CellKey::OffsetPointy>(const CellKey::Centre &cKey1)
        {
            static Transform::CentreToCellKey TF_C2K;
            Point2<float> pC = Point2<float>(cKey1.x, cKey1.y).transform(TF_C2K);
            return CellKey::OffsetPointy(std::round(pC.x), std::round(pC.y));
        }

        template <>
        static Point2<float> transform<CellKey::Centre, Point2<float>>(const CellKey::Centre &cKey1)
        {
            return Point2<float>(cKey1.x, cKey1.y);
        }
    };

};
