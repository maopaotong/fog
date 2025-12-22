/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/ogre.h"

#include "fg/ogre.h"
#include "fg/util.h"
#include "Common.h"
namespace fog
{

    struct TemperatureGenerator
    {
        
        static void generate(std::vector<std::vector<float>> &hMap, std::vector<std::vector<float>> &tMap, int w, int h, float temperatureLatitudeWeightPower)
        {
            Range<float> yRange(0, h);
            Range<float> latRange(-1, 1);

            std::mt19937 randGen(23669983);

            for (int y = 0; y < h; y++)
            {
                float lat = yRange.mapTo(y, latRange);
                float absLatStar = std::pow(std::abs(lat), temperatureLatitudeWeightPower); //(-1, 0)
                int k = 1;
                // float latWeight = 0.5 + 0.5 * std::pow(absLat / 90.0, k); //
                // float latWeight = absLatRange.mapTo(absLat, 0.5, 1.0);
                // float latW = 0.5 + std::abs(lat) / 2.0f;
                float noiseWeight = (1 - absLatStar) * 0.9;
                float altWeight = (1 - absLatStar) * 0.2;                                 //
                std::uniform_real_distribution<float> noiseTp(-noiseWeight, noiseWeight); // noise.

                for (int x = 0; x < w; x++)
                {

                    float alt = hMap[x][y] - 0.5f; //
                    alt = std::max(0.0f, alt);     //(0, 0.5) ocean 's temperature is stable, 0.
                    // tp += -alt;           // alt impact on temperature.,(-2, 0)
                    // tp = std::clamp<float>(tp, -1.2, 0);//drop the extreme value from height.

                    float noise = 0.0f;
                    if (hMap[x][y] > 0.5) // random the highest mountain as frozon.
                    {
                        noise = noiseTp(randGen); //(-2.1, 0.1)//only apply noise on the non-ocean, ocean's temperature is stable.
                    }

                    // tp = std::clamp<float>(tp, -2.1, 0.1);

                    // tp = std::clamp<float>(tp, -1.2, 0);//drop the extrem value form noise.
                    alt = alt * altWeight;

                    tMap[x][y] = -absLatStar - alt + noise;
                }

            } //
        }
    };

}; // end of namespace
