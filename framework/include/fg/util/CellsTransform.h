/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "CellKey.h"

// using namespace Ogre;

namespace fog
{
    struct Centre_OffsetPointy
    {
        float rad;
        Centre_OffsetPointy() : rad(1)
        {
        }
        Centre_OffsetPointy(float rad) : rad(rad)
        {
        }
    };

    struct CellsTransform
    {
        using OffsetPointy = Cell::Offset<Cell::PointyTopOddRow>;
        using OffsetFlat = Cell::Offset<Cell::FlatTopOddCol>;

    private:
        // Pointy top
        using OPC = std::tuple<Cell::Offset<Cell::PointyTopOddRow>, Cell::Centre>;
        using COP = std::tuple<Cell::Centre, Cell::Offset<Cell::PointyTopOddRow>>;
        // Note: for the reason of template impl limitation, the order is not what you see: Offset => Centre => Point2<float>
        using OPCP = std::tuple<Cell::Offset<Cell::PointyTopOddRow>, Point2<float>, Cell::Centre>;

        // Flat top
        using OFC = std::tuple<Cell::Offset<Cell::FlatTopOddCol>, Cell::Centre>;
        using COF = std::tuple<Cell::Centre, Cell::Offset<Cell::FlatTopOddCol>>;
        using OFCP = std::tuple<Cell::Offset<Cell::FlatTopOddCol>, Point2<float>, Cell::Centre>; //

    public:
        // default using.
        using K2C = OPC;  // cellkey to centre in 2d
        using C2K = COP;  // centre in 2d to cellkey.
        using K2P = OPCP; //

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
        static Cell::Centre transform<OffsetPointy, Cell::Centre>(const OffsetPointy &cKey1)
        {
            static Transform::CellKeyToCentre TF_K2C;

            return Point2<float>(cKey1.x, cKey1.y).transform(TF_K2C);
        }

        template <>
        static Cell::Axial transform<Cell::Centre, Cell::Axial>(const Cell::Centre &cKey1)
        {
            constexpr float rad = 1.0f;
            const static float sqrt3 = std::sqrt(3.0f);
            const static float R = (2.0f / sqrt3) * rad; //
            //
            const float x = cKey1.x;
            const float y = cKey1.y;
            float fq = ((sqrt3 / 3.0f) * x - (1.0f / 3.0f) * y) / R;
            float fr = ((2.0f / 3.0f) * y) / R;
            // Step 2: axial -> cube
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

            return Cell::Axial(rx, rz);
        }

        template <>
        static Cell::Centre transform<Cell::Axial, Cell::Centre>(const Cell::Axial &cKey1)
        {
            return {0, 0};
        }

        template <>
        static OffsetPointy transform<Cell::Centre, OffsetPointy>(const Cell::Centre &cKey1)
        {

            Cell::Axial cKey2 = transform<Cell::Centre, Cell::Axial>(cKey1);

            // Step 4: cube -> odd-r offset coordinates
            // odd-row ：row = z, col = x + (row - (row & 1)) / 2
            int row = cKey2.r;
            int col = cKey2.q + (row - (row & 1)) / 2;
            return OffsetPointy(std::round(col), std::round(row));
        }

        template <>
        static Point2<float> transform<Cell::Centre, Point2<float>>(const Cell::Centre &cKey1)
        {
            return Point2<float>(cKey1.x, cKey1.y);
        }
    };

};
