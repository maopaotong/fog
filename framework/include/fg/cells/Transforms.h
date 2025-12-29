/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "TransformD2D3.h"
namespace fog
{
    struct TransformD2D3H
    {

        float scale;
        INJECT(TransformD2D3H(float scale)) : scale(scale)
        {
        }
        void operator()(float &x, float &y, float &z)
        {
            x *= scale;
            float yy = y;
            y = z; // height
            z = -yy * scale;
        }
    };
    struct TransformD3D2
    {
        float h = 0;
        float scale;
        INJECT(TransformD3D2(float scale_)) : scale(scale_)
        {
        }

        void operator()(float &x, float &y, float &z)
        {
            y = -z;
            z = h;
            x *= scale;
            y *= scale;
        }
    };
    struct Transforms
    {
        //TODO private:
        TransformD2TD3 *d2td3; // d2 + terrains height => d3
        TransformD2D3H *d2hd3; // d2 + z= fixed h => d3
        TransformD3D2 *d3d2;

    public:
        INJECT(Transforms(TransformD2TD3 *d2d3, TransformD2D3H *d3h, TransformD3D2 *d3d2)) : d2td3(d2d3), d2hd3(d3h), d3d2(d3d2)
        {
        }

        template <Cell::System s1>
        Vector3 transform3(CellKey &cKey)
        {
            Cell::PointyCentre cKey2 = CellsTransform::Transform<Cell::PointyTop>::transform<s1, Cell::Centre>(cKey);
            return transform3(cKey2, 0, *this->d2td3);
        }

        template <Cell::System s1>
        Vector3 transform3(CellKey &cKey, Point2<float> pInC)
        {
            Cell::PointyCentre cKey2 = CellsTransform::Transform<Cell::PointyTop>::transform<s1, Cell::Centre>(cKey);
            return transform3(cKey2, pInC);
        }

        template <typename F>
        Vector3 transform3(Cell::PointyCentre &cKey, Point2<float> pInC, float h, F &&func)
        {
            return transform3((cKey) + pInC, h, func);
        }

        // template <typename F>
        // Vector3 transform3(Cell::PointyCentre &cKey, float h, F &&func)
        // {
        //     return transform3(CellsTransform::Transform<Cell::PointyTop>::transform<Cell::PointyCentre, Point2<float>>(cKey), h, func);
        // }

        Vector3 transform3(Point2<float> &cKey)
        {
            float x = cKey.x;
            float y = cKey.y;
            float z;
            (*this->d2td3)(x, y, z);
            return Vector3(x, y, z);
        }
        template <typename F>
        Vector3 transform3(Point2<float> &cKey, float h, F &&func)
        {
            float x = cKey.x;
            float y = cKey.y;
            float z = h;
            func(x, y, z);
            return Vector3(x, y, z);
        }
    };

}; // end of namespace
