/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

namespace fog
{
    struct Transform
    {

        struct D2CellWorldUV
        {
            int tlsWidth;
            int tlsHeight;
            float unitWidth;
            float unitHeight;
            D2CellWorldUV(int width, int height, float cw, float rh) : tlsWidth(width), tlsHeight(height), unitWidth(cw), unitHeight(rh)
            {
            }
            void operator()(float &fx, float &fy)
            {
                fx = fx / (unitWidth * static_cast<float>(tlsWidth));
                fy = fy / (unitHeight * static_cast<float>(tlsHeight));
            }
        };
    };

}; // end of namespace
