#pragma once
#include "fg/ogre.h"

namespace fog
{

    struct ColorMap
    {

        static void createTexture(std::string name)
        {
            OgreUtil::createTexture(name, 64, 64, [](unsigned char *pixels, int cols, int rows)
                                    {
                                        for (int x = 0; x < cols; x++)
                                        {
                                            for (int y = 0; y < rows; y++)
                                            {

                                                float e = 2 * x / cols - 1,
                                                      m = y / rows;

                                                float r, g, b;

                                                if (x == cols / 2 - 1)
                                                {
                                                    r = 48;
                                                    g = 120;
                                                    b = 160;
                                                }
                                                else if (x == cols / 2 - 2)
                                                {
                                                    r = 48;
                                                    g = 100;
                                                    b = 150;
                                                }
                                                else if (x == cols / 2 - 3)
                                                {
                                                    r = 48;
                                                    g = 80;
                                                    b = 140;
                                                }
                                                else if (e < 0.0)
                                                {
                                                    r = 48 + 48 * e;
                                                    g = 64 + 64 * e;
                                                    b = 127 + 127 * e;
                                                }
                                                else
                                                {                    // adapted from terrain-from-noise article
                                                    m = m * (1 - e); // higher elevation holds less moisture; TODO: should be based on slope, not elevation

                                                    r = 210 - 100 * m;
                                                    g = 185 - 45 * m;
                                                    b = 139 - 45 * m;
                                                    r = 255 * e + r * (1 - e),
                                                    g = 255 * e + g * (1 - e),
                                                    b = 255 * e + b * (1 - e);
                                                }
                                                int p = (y * cols + x) * 4;
                                                pixels[p++] = static_cast<unsigned char>(r);
                                                pixels[p++] = static_cast<unsigned char>(g);
                                                pixels[p++] = static_cast<unsigned char>(b);
                                                pixels[p++] = 255;
                                            } // end for
                                        } // end for
                                    });
        }
    };
};