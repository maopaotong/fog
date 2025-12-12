/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
#pragma once
#include <Ogre.h>
#include <array>

namespace fog
{
    using namespace Ogre;

    class OgreCode
    {

    public:
        //---------------------------------------------------------------------
        //
        template <typename F>
        static float getHeightAtTerrainPosition(Real x, Real y, F &&getHeightAtPoint, int mSize, Vector3 *pNorm, bool normalise = false)
        {
            // get left / bottom points (rounded down)
            Real factor = (Real)mSize - 1.0f;
            Real invFactor = 1.0f / factor;

            uint32 startX = static_cast<uint32>(x * factor);
            uint32 startY = static_cast<uint32>(y * factor);
            uint32 endX = startX + 1;
            uint32 endY = startY + 1;

            // now get points in terrain space (effectively rounding them to boundaries)
            // note that we do not clamp! We need a valid plane
            Real startXTS = startX * invFactor;
            Real startYTS = startY * invFactor;
            Real endXTS = endX * invFactor;
            Real endYTS = endY * invFactor;

            // now clamp
            endX = std::min(endX, mSize - 1u);
            endY = std::min(endY, mSize - 1u);

            // get parametric from start coord to next point
            Real xParam = (x - startXTS) / invFactor;
            Real yParam = (y - startYTS) / invFactor;

            /* For even / odd tri strip rows, triangles are this shape:
            even     odd
            3---2   3---2
            | / |   | \ |
            0---1   0---1
            */

            // Build all 4 positions in terrain space, using point-sampled height

            Vector3 v0(startXTS, startYTS, getHeightAtPoint(startX, startY));
            Vector3 v1(endXTS, startYTS, getHeightAtPoint(endX, startY));
            Vector3 v2(endXTS, endYTS, getHeightAtPoint(endX, endY));
            Vector3 v3(startXTS, endYTS, getHeightAtPoint(startX, endY));

            // define a plane in terrain space
            // do not normalise as the normalization factor cancels out in the final
            // equation anyway
            Vector4 plane;
            if (startY % 2)
            {
                // odd row
                bool secondTri = ((1.0 - yParam) > xParam);
                if (secondTri)
                    plane = Math::calculateFaceNormalWithoutNormalize(v0, v1, v3);
                else
                    plane = Math::calculateFaceNormalWithoutNormalize(v1, v2, v3);
            }
            else
            {
                // even row
                bool secondTri = (yParam > xParam);
                if (secondTri)
                    plane = Math::calculateFaceNormalWithoutNormalize(v0, v2, v3);
                else
                    plane = Math::calculateFaceNormalWithoutNormalize(v0, v1, v2);
            }

            if (pNorm)
            {
                *pNorm = Vector3(plane.x, plane.z, -plane.y);

                if (normalise)
                {
                    pNorm->normalise();
                }
            }
            // Solve plane equation for z
            return (-plane.x * x - plane.y * y - plane.w) / plane.z;
        }
    };
};
