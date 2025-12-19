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
    struct Transforms
    {
        TransformD2D3 *d2d3;
        TransformD2D3H *d2d3H;
        INJECT(Transforms(TransformD2D3 *d2d3, TransformD2D3H *d3h)) : d2d3(d2d3), d2d3H(d3h)
        {
        }
    };

}; // end of namespace
