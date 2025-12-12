/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <Ogre.h>
#include "fg/util.h"
#include "fg/State.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/core/ManualState.h"
#include "fg/MeshBuild.h"
namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;

    class Building : public ManualState
    {

    protected:
        BuildingType type;

    public:
        Building(BuildingType type) : ManualState(), type(type)
        {
            this->material = MaterialNames::materialNameBuilding;
        }

        virtual ~Building()
        {
        }

        virtual void init() override
        {
            ManualState::init();
        }

        void rebuildMesh() override
        {
            MeshBuild::SpiderNet buildMesh(obj);
            buildMesh.begin(this->material); //

            //Node2D *node2D = Context<Node2D>::get();
            float totalHeight = .35f;
            auto position = [totalHeight](Vector2 &pointOnLayer, int layer2, int totalLayer)
            {
                float scale = 0.0f;//
                float heightPerLayer = totalHeight / (totalLayer - 2);
                int layerFromBottom = totalLayer - layer2 - 1;
                float layerHeight = layerFromBottom * heightPerLayer;

                if (layerFromBottom == totalLayer - 1) // top layer.
                {
                    layerHeight = totalHeight;
                }
                else
                {
                    // scale = node2D->getScale();
                    // TODO scale 
                }

                //Vector2 pointIn2D = pointOnLayer * scale;
                //Vector3 positionIn3D = node2D->plane->to3DInPlane(pointIn2D);
                Point2<float> p2d = pointOnLayer;
                Vector3 positionIn3D = p2d.transform3(*Context<Transform::D2H2D3>::get());
                positionIn3D.y = layerHeight;

                return positionIn3D;
            };

            buildMesh(position, ColourValue::Green);
            buildMesh.end();
        }
    };
}; // end of namespace
