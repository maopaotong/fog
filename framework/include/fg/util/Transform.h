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
            int colWidth;
            int rowHeight;
            D2CellWorldUV(int width, int height, int cw, int rh) : tlsWidth(width), tlsHeight(height),colWidth(cw),rowHeight(rh)
            {
            }
            void operator()(float &fx, float &fy)
            {
                fx /= (colWidth * static_cast<float>(tlsWidth));
                fy /= (rowHeight * static_cast<float>(tlsHeight));
            }
        };

    };

}; // end of namespace
