/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <Ogre.h>
#include <OgreVector.h>
namespace fog
{
    using namespace Ogre;

    class Terrains
    {
    public:
        virtual ~Terrains() {};

        virtual float getHeightWithNormalAtWorldPosition(Vector3 pos, Vector3 *norm) = 0;

        float getHeightAtWorldPosition(Vector3 pos)
        {
            return getHeightWithNormalAtWorldPosition(pos, nullptr);
        }

        virtual Vector3 getOrigin() = 0;
    };
};
