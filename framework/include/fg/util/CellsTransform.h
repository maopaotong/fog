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
        using OffsetPointy = Cell::PointyOffset;
        using OffsetFlat = Cell::FlatOffset;
        using AxialQ30 = Cell::PointyAxial;
        using AxialQ0 = Cell::FlatAxial;
        using Centre = Cell::Centre;

    private:
        // Pointy top
        using Pointy_O_C = std::tuple<OffsetPointy, Centre>;
        using Pointy_C_O = std::tuple<Centre, OffsetPointy>;
        // Note: for the reason of template impl limitation, the order is not what you see: Offset => Centre => Point2<float>
        using Pointy_O_C_P = std::tuple<OffsetPointy, Point2<float>, Centre>;
        using Pointy_P_C_O = std::tuple<Point2<float>, OffsetPointy, Centre>;

        // Flat top
        using Flat_O_C = std::tuple<OffsetFlat, Centre>;
        using Flat_C_O = std::tuple<Centre, OffsetFlat>;
        using Flat_O_C_P = std::tuple<OffsetFlat, Point2<float>, Centre>; //
        using Flat_P_C_O = std::tuple<Point2<float>, OffsetFlat, Centre>;

    public:
        // default using.
        using K2C = Pointy_O_C;   // cellkey to centre in 2d
        using C2K = Pointy_C_O;   // centre in 2d to cellkey.
        using K2P = Pointy_O_C_P; //
        using P2K = Pointy_P_C_O; //

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
            static_assert(sizeof(K1) == 0, "not supported transform between types.");
        }

        template <>
        static Centre transform<OffsetPointy, Centre>(const OffsetPointy &cKey1)
        {
            const static float sqrt3 = std::sqrt(3.0f);
            const static float rUnit = sqrt3;
            const static float cUnit = 2.0f;             // 2*radius
            const static float cUnitHalf = cUnit / 2.0f; // half cUnit.

            // innerradius = 1.0f;
            int x = cKey1.x;
            int y = cKey1.y;
            float fx = x * cUnit + (y % 2 == 0 ? 0 : cUnitHalf);
            float fy = y * rUnit;

            return Centre(fx, fy);
        }

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

        template <>
        static AxialQ30 transform<Cell::Centre, AxialQ30>(const Centre &cKey1)
        {
            const static float sqrt3 = std::sqrt(3.0f);

            // innerradius = 1.0f
            const static float qUnit = sqrt3;
            const static float rUnit = sqrt3;
            //

            Point2<float> centreP(cKey1.x, cKey1.y);

            float rotateX = centreP.rotateAndGetX<-30>(); // distance to the number line of q.
            float fq = rotateX / qUnit;
            float fr = -cKey1.y / rUnit;

            return AxialQ30(cubeRound(fq, fr));
        }

        template <>
        static AxialQ0 transform<Cell::Centre, AxialQ0>(const Centre &cKey1)
        {
            const static float sqrt3 = std::sqrt(3.0f);

            // innerradius = 1.0f
            const static float qUnit = sqrt3;
            const static float rUnit = sqrt3;
            //
            float fq = cKey1.x / qUnit;
            //
            Point2<float> centreP(cKey1.x, cKey1.y);
            float rotateY = centreP.rotateAndGetY<-30>(); // distance to the number line of r.
            float fr = rotateY / rUnit;

            return AxialQ0(cubeRound(fq, fr));
        }
        template <>
        static Centre transform<AxialQ0, Cell::Centre>(const AxialQ0 &cKey1)
        {
            const static float sqrt3 = std::sqrt(3.0f);
            const static float qUnit = sqrt3;
            Point2<float> p = Point2<float>::makeByDistanceToLines<90,210>(cKey1.q, cKey1.r);

            return Cell::Centre(p.x * qUnit, p.y * qUnit);
        }

        template <>
        static OffsetPointy transform<Centre, OffsetPointy>(const Centre &cKey1)
        {

            AxialQ30 cKey2 = transform<Cell::Centre, AxialQ30>(cKey1);

            // Step 4: cube -> odd-r offset coordinates
            // odd-row ：row = z, col = x + (row - (row & 1)) / 2
            int row = -cKey2.r;
            int col = cKey2.q + (-row - (row & 1)) / 2;
            return OffsetPointy(std::round(col), std::round(row));
        }

        template <>
        static Point2<float> transform<Cell::Centre, Point2<float>>(const Cell::Centre &cKey1)
        {
            return Point2<float>(cKey1.x, cKey1.y);
        }

        template <>
        static Centre transform<Point2<float>, Centre>(const Point2<float> &cKey1)
        {
            return Centre(cKey1.x, cKey1.y);
        }
    };

};
