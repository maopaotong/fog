/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
namespace fog
{
    class Transform
    {
    public:
       
        struct CentreToCellKey
        {
            float rad;
            CentreToCellKey() : rad(1)
            {
            }
            CentreToCellKey(float rad) : rad(rad)
            {
            }

            void operator()(float &fx, float &fy) const
            {
                float cx = fx;
                float cy = fy;
                const float sqrt3 = std::sqrt(3.0f);
                const float R = (2.0f / sqrt3) * rad; //
                float q = ((sqrt3 / 3.0f) * cx - (1.0f / 3.0f) * cy) / R;
                float r = ((2.0f / 3.0f) * cy) / R;

                // Step 2: axial -> cube
                float x = q;
                float z = r;
                float y = -x - z;

                // Step 3: cube rounding to nearest hex
                int rx = (int)std::round(x);
                int ry = (int)std::round(y);
                int rz = (int)std::round(z);

                float dx = std::abs(rx - x);
                float dy = std::abs(ry - y);
                float dz = std::abs(rz - z);

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

                // Step 4: cube -> odd-r offset coordinates
                // 在 odd-r 中：row = z, col = x + (row - (row & 1)) / 2
                int row = rz;
                int col = rx + (row - (row & 1)) / 2;
                fx = col;
                fy = row;
            }
        };

        struct CellKeyToCentre
        {
            CellKeyToCentre()
            {
            }
            void operator()(float &fx, float &fy)
            {
                int x = fx;
                int y = fy;
                fx = x * 2 * 1 + (y % 2 == 0 ? 0 : 1);
                fy = y * 1 * std::sqrt(3.0f);
            }
        }; // struct

        struct D2CellWorldUV
        {
            int tlsWidth;
            int tlsHeight;
            D2CellWorldUV(int width, int height) : tlsWidth(width), tlsHeight(height)
            {
            }
            void operator()(float &fx, float &fy)
            {
                fx /= (2.0 * static_cast<float>(tlsWidth));
                fy /= (std::sqrt(3.0) * static_cast<float>(tlsHeight));
            }
        };

        // struct UVByKey
        // {

        //     CellCentreByKey centre;
        //     Scale2<float> scale;
        //     UVByKey(int w, int h) : UVByKey(static_cast<float>(w), static_cast<float>(h))
        //     {
        //     }
        //     UVByKey(float w, float h) : scale(1.0 / (2.0 * w), 1.0 / (std::sqrt(3.0) * h))
        //     {
        //     }

        //     void operator()(float &fx, float &fy)
        //     {
        //         centre(fx, fy);
        //         scale(fx, fy);
        //     }
        // };

        // struct D2H2D3
        // {
        //     std::function<float(float x, float y)> height;
        //     float scale;

        //     D2H2D3(float scale, std::function<float(float, float)> height) : height(height), scale(scale)
        //     {
        //     }

        //     // D2H2D3(const D2H2D3 &dhd)
        //     // {
        //     //     this->height = dhd.height;
        //     //     this->scale = dhd.scale;
        //     // }
        //     // D2H2D3(const D2H2D3 &&dhd)
        //     // {
        //     //     this->height = dhd.height;
        //     //     this->scale = dhd.scale;
        //     // }
        //     // D2H2D3 &operator=(const D2H2D3 &) = delete;
        //     template <typename F>
        //     void setHeight(F &&func)
        //     {
        //         this->height = func;
        //     }

        //     void operator()(float &x, float &y, float &z)
        //     {
        //         x *= scale;
        //         y *= scale;
        //         z = -y;
        //         y = height(x, y);
        //     }
        // };

        // struct D2_HV_D3
        // {
        //     float h;
        //     float scale;
        //     D2_HV_D3(D2H2D3 scale, float h = 0) : h(h), scale(scale.scale)
        //     {
        //     }

        //     void operator()(float &x, float &y, float &z)
        //     {
        //         x *= scale;
        //         y *= scale;
        //         z = -y;
        //         y = h;
        //     }
        // };

        // struct D3_NORMAL_D2
        // {
        //     float h;
        //     float scale;
        //     D3_NORMAL_D2(float scale_, float h = 0) : h(h), scale(scale_)
        //     {
        //     }

        //     void operator()(float &x, float &y, float &z)
        //     {
        //         y = -z;
        //         z = h;
        //         x *= scale;
        //         y *= scale;
        //     }
        // };
    };

}; // end of namespace
