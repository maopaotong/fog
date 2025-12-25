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
        TransformD2TD3 *d2td3;//d2 terrains height => d3
        TransformD2D3H *d2hd3; //d2+z=h => d3
        TransformD3D2 *d3d2;
        INJECT(Transforms(TransformD2TD3 *d2d3, TransformD2D3H *d3h, TransformD3D2 *d3d2)) : d2td3(d2d3), d2hd3(d3h), d3d2(d3d2)
        {
        }
    };

}; // end of namespace
