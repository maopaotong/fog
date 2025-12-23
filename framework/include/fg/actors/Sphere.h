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

    struct Sphere : public Actor
    {

        INJECT(Sphere(CoreMod *core, SceneNode *sceNode)) : Actor(sceNode)
        {

            sceNode = sceNode->createChildSceneNode(); //

            ManualObject *obj = core->createManualObject();
            obj->setQueryFlags(0x00000001);
            sceNode->attachObject(obj);
            float scale = 5;
            sceNode->setScale(Vector3(scale, scale, scale));
            sceNode->setPosition(0, scale * 2, 0); //
            MeshBuild::Cylinder cylinder(obj);

            cylinder.begin(MaterialNames::materialNameForActor); //

            int layers = 5 * 2 + 1;
            float yAnglar = Math::PI / (layers - 1);

            cylinder(layers, [this, layers, yAnglar](Vector2 pos2, int layer)
                      {
                          float h = std::cosf(layer * yAnglar); //

                          float r = std::sqrt(1.0f - h * h);
                          pos2 = pos2 * r;

                          return Vector3(pos2.x, h, -pos2.y); //
                      },
                      MeshBuild::SpiderNetTriangle(), [](int layer)
                      { return std::pow(2, layer) * 6; }, ColourValue::Green);

            cylinder.end();
        }

        ~Sphere()
        {
        }
    };
}; // end of namespace
