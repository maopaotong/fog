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
            MeshBuild::SpiderNet buildMesh(obj);

            buildMesh.begin(this->material); //

            int layers = 10;
            float heightL = 1 / (layers - 1);

            buildMesh(layers, [this, layers, heightL](Vector2 &pos2, int layer)
                      {
                        
                          return Vector3(pos2.x, 1.0 - layer * heightL, -pos2.y); //
                      },
                      ColourValue::Green);

            buildMesh(layers, [this, layers, heightL](Vector2 &pos2, int layer)
                      {
                          return Vector3(pos2.x, -(1.0 - layer * heightL), -pos2.y); //
                      },
                      ColourValue::Green);

            buildMesh.end();
        }

        ~Sphere()
        {
        }
    };
}; // end of namespace
