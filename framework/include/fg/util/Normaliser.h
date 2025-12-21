/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Common.h"
namespace fog
{

    
    struct Normaliser
    {
    
        static void normalise(std::vector<std::vector<float>> & vMap, int w, int h){
            float min_val = vMap[0][0], max_val = vMap[0][0];
            float total = 0.0f;
            for (const auto &row : vMap)
            {
                for (float h : row)
                {
                    if (h < min_val)
                    {
                        min_val = h;
                    }
                    if (h > max_val)
                    {

                        max_val = h;
                    }
                    total += 1.0f;
                }
            }

            float rangeFloat = max_val - min_val + 1;
            // counting the samples scatter on [0..1000]
            int length = 1000;
            float *percentage = new float[length]{0.0f};

            for (int i = 0; i < w; ++i)
            {
                for (int j = 0; j < h; ++j)
                {
                    int idx = static_cast<int>(std::floor((vMap[i][j] - min_val) / rangeFloat * length));
                    percentage[idx]++;
                }
            }

            // incremental percentage scatter on [0..1000]
            float count = 0.0f;
            for (int i = 0; i < length; i++)
            {
                count += percentage[i];
                percentage[i] = count / total; // percentage,
            }

            // incremental percentage as the height of position.
            for (int i = 0; i < w; ++i)
            {
                for (int j = 0; j < h; ++j)
                {
                    // height as index of incremental histogram.
                    int hAsIdx = static_cast<int>(std::floor((vMap[i][j] - min_val) / rangeFloat * length));
                    vMap[i][j] = percentage[hAsIdx];
                }
            }
            delete[] percentage;
        }
    };


}; // end of namespace
