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
        using Layout = Cell::Layout;
        using System = Cell::System;

        // // pointy
        // template <Layout l, System s11, System s22>
        // struct TransformTag
        // {
        //     static constexpr Layout layout = l;
        //     static constexpr System s1 = s11;
        //     static constexpr System s2 = s22;
        // };

        static constexpr Layout Pointy = Cell::PointyTop;
        static constexpr Layout Flat = Cell::FlatTop;

        static constexpr System Offset = Cell::Offset;
        static constexpr System Centre = Cell::Centre;
        static constexpr System Axial = Cell::Axial;
        static constexpr System Cartesian = Cell::Cartesian;

        
        template <Layout layout>
        struct Transform;
        template <>
        struct Transform<Pointy>
        {
           
            template <System s1, System s2>
            static
                typename std::enable_if_t<s1 == Offset && s2 == Centre, Cell::SystemInfo<Centre>::type>
                transform(const typename Cell::SystemInfo<Offset>::type &cKey1)
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

                return Point2<float>(fx, fy);
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Centre && s2 == Axial, Cell::SystemInfo<Axial>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
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

                return Cell::AxialKey(cubeRound(fq, fr));
            }

            // Pointy,Centre => Offset.
            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Centre && s2 == Offset, Cell::SystemInfo<Offset>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
            {

                Cell::AxialKey cKey2 = transform<Centre, Axial>(cKey1);

                // Step 4: cube -> odd-r offset coordinates
                // odd-row ：row = z, col = x + (row - (row & 1)) / 2
                int row = -cKey2.r;
                int col = cKey2.q + (-row - (row & 1)) / 2;
                return Cell::OffsetKey(std::round(col), std::round(row));
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Centre && s2 == Cartesian, Cell::SystemInfo<Cartesian>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
            {
                return Point2<float>(cKey1.x, cKey1.y);
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Cartesian && s2 == Centre, Cell::SystemInfo<Centre>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
            {
                return Point2<float>(cKey1.x, cKey1.y);
            }
        };

        // // 
        // // transform impl
        // template <Layout l, System s1, System s2>
        // static typename Cell::SystemInfo<s2>::type transform(const typename Cell::SystemInfo<s1>::type &cKey1)
        // {
        //     static_assert(sizeof(Layout) == 0, "not supported transform between types.");
        // }


        // Flat , Offset => Centre
        template <>
        struct Transform<Flat>
        {

            template <System s1, System s2>
            static
                typename std::enable_if_t<s1 == Offset && s2 == Centre, Cell::SystemInfo<Centre>::type>
                transform(const typename Cell::SystemInfo<Offset>::type &cKey1)
            {
                float fx = cKey1.x * Cell::LayoutInfo<Cell::FlatTop>::colWidth;
                float fy = (cKey1.y + (cKey1.x % 2 == 0 ? 0 : 0.5)) * Cell::LayoutInfo<Cell::FlatTop>::rowHeight;

                return Point2<float>(fx, fy);
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Centre && s2 == Axial, Cell::SystemInfo<Axial>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
            {
                const static float sqrt3 = std::sqrt(3.0f);

                // innerradius = 1.0f
                const static float qUnit = sqrt3;
                const static float rUnit = sqrt3;
                //
                float fq = cKey1.x / qUnit;
                //
                Point2<float> centreP(cKey1.x, cKey1.y);
                float rotateY = centreP.rotateAndGetX<-120>(); // distance to the number line of r.
                float fr = rotateY / rUnit;

                return Cell::AxialKey(cubeRound(fq, fr));
            }

            // Pointy,Centre => Offset.
            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Centre && s2 == Offset, Cell::SystemInfo<Offset>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
            {
                float fx = cKey1.x / Cell::LayoutInfo<Cell::FlatTop>::colWidth;
                int x = std::round(fx);

                float fy = cKey1.y / Cell::LayoutInfo<Cell::FlatTop>::rowHeight;
                fy -= x % 2 == 0 ? 0.0 : 0.5;

                int y = std::round(fy);
                return Cell::OffsetKey(x, y);
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Axial && s2 == Centre, Cell::SystemInfo<Centre>::type> transform(const typename Cell::SystemInfo<Axial>::type &cKey1)
            {
                const static float sqrt3 = std::sqrt(3.0f);
                const static float qUnit = sqrt3;
                Point2<float> p = Point2<float>::makeByDistanceToLines<90, 210>(cKey1.q, cKey1.r);

                return Point2<float>(p.x * qUnit, p.y * qUnit);
            }

            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Cartesian && s2 == Centre, Cell::SystemInfo<Centre>::type> transform(const typename Cell::SystemInfo<Centre>::type &cKey1)
            {
                return Point2<float>(cKey1.x, cKey1.y);
            }
            template <System s1, System s2>
            static typename std::enable_if_t<s1 == Centre && s2 == Cartesian, Cell::SystemInfo<Centre>::type> transform(const typename Cell::SystemInfo<Cartesian>::type &cKey1)
            {
                return Point2<float>(cKey1.x, cKey1.y);
            }
            
        };
        // template 
        
        template <Layout layout,System s1,System s2>
            static typename std::vector<typename Cell::SystemInfo<s2>::type>
            transformAll(std::vector<typename Cell::SystemInfo<s1>::type> &cks)
            {

                using K1 = typename Cell::SystemInfo<s1>::type;
                using K2 = typename Cell::SystemInfo<s2>::type;

                std::vector<K2> ret;

                CollectionUtil::transform<K1, K2>(cks, ret, [](K1 &ck)
                                                  { return Transform<layout>::transform<s1,s2>(ck); });
                return ret;
            }
    private:
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
    };

};
