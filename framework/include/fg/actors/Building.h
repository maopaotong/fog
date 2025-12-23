/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"
#include "fg/cells.h"
namespace fog
{
    class Building : public ManualState
    {

    protected:
        BuildingType type;
        Transforms *tfs;
        Config *config;

    public:
        INJECT(Building(BuildingType type, Transforms *tfs, CoreMod *core, Config *config)) : ManualState(core),
                                                                                              tfs(tfs),
                                                                                              config(config),
                                                                                              type(type)
        {
            
            this->sceNode->setScale(config->cellScale * 0.75, config->cellScale * 0.25, config->cellScale * 0.75);
            MeshBuild::Cylinder cylinder(obj);

            cylinder.begin(this->material); //

            int layers = 3;
            float deltaH = 1.0f / (layers - 2);

            cylinder(layers, [this, layers, deltaH](Vector2 pos2, int layer) -> Vector3
                     {
                         float h;
                         if (layer == 0)
                         {
                             pos2 = Vector2(0.0f, 0.0f); // close the top roof.
                             h = 1.0f;                   // normal roof height
                         }
                         else
                         {
                             h = 1 - deltaH * (layer - 1);
                         }

                         return Vector3(pos2.x, h, -pos2.y); //
                     },
                     MeshBuild::CyclinderTriangle(), [](int layer)
                     { return 6; }, ColourValue::Green);

            cylinder.end();
        }

        virtual ~Building()
        {
        }
    };
}; // end of namespace
