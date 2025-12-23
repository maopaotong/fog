/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "Common.h"
#include "fg/util.h"

#include "fg/cells.h"

#include "PathState.h"
#include "PathFollow2MissionState.h"
namespace fog
{

    struct Sphere : public ManualState
    {

        INJECT(Sphere(CoreMod *core)) : ManualState(core)
        {
            this->sceNode->setScale(Vector3(10.0f, 10.0f, 10.0f));
            MeshBuild::Cylinder cyclinder(obj);

            cyclinder.begin(this->material); //

            int layers = 10;
            float yAnglar = Math::PI / 2.0f / layers;

            cyclinder(layers, [this, layers, yAnglar](Vector2 pos2, int layer)
                      {
                          float h = std::cosf(layer * yAnglar); //
                          float r = std::sqrt(1.0f - h * h);
                          pos2 = pos2 * r;

                          return Vector3(pos2.x, h, -pos2.y); //
                      },
                      MeshBuild::SpiderNetTriangle(), [](int layer)
                      { return std::pow(2, layer) * 6; }, ColourValue::Green);

            cyclinder.end();
        }

        ~Sphere()
        {
        }
    };
}; // end of namespace
