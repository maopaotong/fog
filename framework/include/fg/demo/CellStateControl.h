/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <fg/ogre.h>
#include "fg/core.h"
#include "fg/ogre.h"
#include "fg/util.h"
#include "ManualState.h"

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
        CostMap * costMap;
    public:
        INJECT(CellStateControl(CostMap* costMap,CoreMod * core)) : ManualState(core),costMap(costMap)
        {
            this->rebuildMesh();
        }
       
        void rebuildMesh() 
        {

            // Cell::Center *cc = Context<Cell::Center>::get();
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
        bool getCostColor(CellKey &cell, Ogre::ColourValue &color) const
        {
            // CostMap *costMap = Context<CostMap>::get();
            const int cost = 0;// costMap->getCost(cell, 0);
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
