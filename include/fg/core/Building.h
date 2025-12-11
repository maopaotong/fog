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
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
#include "fg/State.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/util/CollectionUtil.h"
#include "fg/core/ManualState.h"
#include "fg/MeshBuild.h"
#include "fg/util/Point2.h"
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
