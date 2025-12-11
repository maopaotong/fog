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
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/util/CostMap.h"
#include "fg/Cell.h"
#include "fg/util/CostMap.h"
#include "fg/MeshBuild.h"

#include "fg/util/Property.h"
#include "fg/core/ManualState.h"

namespace fog
{
    using namespace Ogre;

    typedef Property::Ref<bool> boolRef;

    //
    class CellStateControl : public ManualState
    {
    public:
        // boolRef showCost0;
        // boolRef showCost1;
        // boolRef showCost2;
        // boolRef showCost3;
        // boolRef showOther;

    public:
        CellStateControl() : ManualState()
        {
        }
        void init() override
        {
            // this->showCost0 = createProperty("showCost0", false);
            // this->showCost1 = createProperty("showCost1", false);
            // this->showCost2 = createProperty("showCost2", false);
            // this->showCost3 = createProperty("showCost3", false);
            ManualState::init();
        }
        void rebuildMesh() override
        {

            Cell::Center *cc = Context<Cell::Center>::get();
            // MeshBuild::PointOnCircle buildMesh(obj);
            MeshBuild::SpiderNet buildMesh(obj);
            buildMesh.begin(this->material);
            // cc->forEachCell([this, &buildMesh](HexTile::Key &cell)
            //                 {
            //                     ColourValue color;
            //                     bool build = this->getCostColor(cell, color);
            //                     if (build)
            //                     {
            //                         buildMesh([&cell, this](Vector2 &pointOnCircle, int layer, int totalLayer)
            //                                   {
            //                                       Vector2 pointOnLayer = pointOnCircle * (float)layer / ((float)totalLayer - 1);
            //                                       //Vector3 pos = Context<Node2D>::get()->to3D(Cell::getOrigin2D(cell), pointOnLayer, nullptr); //
            //                                       Vector3 pos = cell.transform3(pointOnLayer);
                                                    
            //                                       pos.y += 1.f;
            //                                       return pos;
            //                                   },
            //                                   color); //
            //                     } // end if
            //                 });

            buildMesh.end();
        }

        // Get color based on cost
        bool getCostColor(HexTile::Key &cell, Ogre::ColourValue &color) const
        {
            CostMap *costMap = Context<CostMap>::get();
            const int cost = costMap->getCost(cell.x, cell.y);
            switch (cost)
            {
            case CostMap::OBSTACLE:
                color = Ogre::ColourValue::Red;
                return true;
            case CostMap::DEFAULT_COST:
                color = Ogre::ColourValue(0.8f, 0.6f, 0.2f); // light Sand color
                return true;
            case 2:
                color = Ogre::ColourValue(0.6f, 0.4f, 0.1f); // Dark Sand color
                return true;
            case 3:
                color = Ogre::ColourValue(0.2f, 0.4f, 0.8f); // Water color
                return true;
            default:
                color = Ogre::ColourValue(0.7f, 0.7f, 0.7f); // light gray

                return true;
            }
        }
    };

}; // end of namespace
