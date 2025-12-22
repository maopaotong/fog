/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util/Common.h"
#include <cmath>
#include <algorithm>
#include "Common.h"

#include "Point2.h"
#include "Box2.h"

#include "CollectionUtil.h"
#include "Transform.h"
using namespace Ogre;

namespace fog
{

    struct Circle
    {

        //
        template <typename F>
        static void forEachPointOnCircle(float density, F &&visit)
        {

            // rad=1, RAD=2
            float angle = 360.0f / density;
            for (int i = 0; i < density; i++)
            {

                float angle_rad = (angle * i + 0.0f) * Ogre::Math::PI / 180.0f;

                float dx = std::cos(angle_rad);
                float dy = std::sin(angle_rad);

                visit(i, Vector2(dx, dy));
            }
        };
    };

};
