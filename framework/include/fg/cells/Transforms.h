/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "TransformD2D3.h"
namespace fog
{
    
    struct Transforms
    {
        TransformD2D3 *d2d3;
        INJECT(Transforms(TransformD2D3 *d2d3)) : d2d3(d2d3)
        {
        }
    };

}; // end of namespace
