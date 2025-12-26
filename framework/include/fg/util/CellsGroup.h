/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "CellKey.h"

// using namespace Ogre;

namespace fog
{

    struct CellsGroup
    {
       
        template <typename K>
        Box2<float> getBoxIn2D(K &cKey)
        {
            // Vector2 centre = getOrigin2D(cKey);
            Point2<float> centre = CellsTransform::transform<K, Cell::Centre>(cKey);
            float left = centre.x - 1.0; // rad
            float bottom = centre.y - std::sqrt(3.0) / 2.0;
            return Box2<float>(left, bottom, left + 2.0, bottom + std::sqrt(3.0));
        }

        template <typename K>
        static Box2<float> getOuterBoxIn2D(K &cKey)
        {
            // Vector2 centre = getOrigin2D(cKey);

            Point2<float> centre = CellsTransform::transform<K, Cell::Centre>(cKey);

            float left = centre.x - 1.0;                    // rad
            float bottom = centre.y - 2.0 / std::sqrt(3.0); // R
            return Box2<float>(left, bottom, left + 2.0, bottom + 4.0 / std::sqrt(3.0));
        }

        template <typename K>
        static Box2<float> getBoxInUV(K &cKey, int width, int height)
        {
            Box2<float> r = getBoxIn2D<K>(cKey);
            return r.transform(Transform::D2CellWorldUV(width, height));
        }

        template <typename K>
        static Box2<float> getOuterBoxInUV(K &cKey, int width, int height)
        {
            Box2<float> r = getOuterBoxIn2D<K>(cKey);
            return r.transform(Transform::D2CellWorldUV(width, height));
        }

    };

};
