/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Common.h"

#include "fg/ogre.h" //TODO remove this include.
#include "fg/cells.h"
#include "fg/cells/Transforms.h"
namespace fog
{

    class CellInstanceState : public Actor
    {
        std::stack<ColourValue> colours;
        ManualObject *obj;
        CellKey cis;
        CoreMod *core;
        Transforms *tfs;

    public:
        CellInstanceState(CellKey cis, CoreMod *core, Transforms *tfs, SceneNode *sNode)
            : Actor(sNode),
              tfs(tfs),
              cis(cis), core(core)
        {
            this->obj = core->createManualObject();
            sNode->attachObject(this->obj);
            ColourValue bottomColour;
            bool hasBottomColor = getCostColor(cis, bottomColour);
            if (hasBottomColor)
            {
                colours.push(bottomColour);
            }

            buildMesh();
        }

        virtual ~CellInstanceState()
        {
        }

        Vector3 getOrigin3D()
        {
            // return Context<Node2D>::get()->to3D(Cell::getOrigin2D(cis),config->CELL_SCALE);
            return cis.transform3(*tfs->d2td3);
        }

        void buildMesh()
        {
            if (colours.empty())
            {
                obj->clear();
            }
            else
            {
                // MeshBuild::SpiderNet buildMesh(obj);
                // buildMesh.begin(MaterialNames::materialNameInUse);
                // ColourValue currentColour = colours.top();
                // buildMesh(cis, currentColour); //
                // buildMesh.end();
            }
        }

        // Get color based on cost
        bool getCostColor(CellKey &cell, Ogre::ColourValue &color) const
        {

            return false;
        }

        CellKey getCellKey()
        {
            return this->cis;
        }

    }; // end of class

    class CellInstanceStateManager : public Manager<CellInstanceState>
    {
        std::unordered_map<CellKey, CellInstanceState *, CellKey::Hash> cellInstanceStates;

        CoreMod *core;

        Config *config;

        Transforms * tfs;
    public:
        INJECT(CellInstanceStateManager(CoreMod *core, Config *config, Transforms * tfs, CellsDatas::Options& cdos))
            : config(config),
              core(core),
              tfs(tfs)

        {

            for (int x = 0; x < cdos.cellsRange.getWidth(); x++)
            {
                for (int y = 0; y < cdos.cellsRange.getHeight(); y++)
                {
                    CellKey cell{x, y};
                    CellInstanceState *state = new CellInstanceState(cell, core, tfs, core->getRootSceneNode()->createChildSceneNode());
                    this->add(state); //
                    this->cellInstanceStates[cell] = state;
                }
            }
            
        }
        virtual ~CellInstanceStateManager()
        {
        }

        // CellInstanceState *getCellInstanceStateByPosition(Vector2 posIn2D)
        // {
        //     Vector3 pos3D = Context<Node2D>::get()->to3D(posIn2D,1);//
        //     return getCellInstanceStateByPosition(pos3D);
        // }

        // HexTile::Key getCellKeyByPosition(Vector2 pos)
        // {
        //     HexTile::Key cell;
        //     if (Context<Cell::Center>::get()->findCellByPosition(pos, cell))
        //     {
        //         return cell;
        //     }
        //     return HexTile::Key(-1, -1);
        // }

        CellInstanceState *getCellInstanceStateByPosition(Vector2 pos)
        {
            // HexTile::Key cell;
            // if (Context<Cell::Center>::get()->findCellByPosition(pos, cell))
            // {
            //     return this->cellInstanceStates[cell];
            // }
            CellKey cKey = CellKey::from(pos);
            return getCellInstanceStateByCellKey(cKey);
        }
        CellInstanceState *getCellInstanceStateByCellKey(CellKey cKey)
        {
            auto it = this->cellInstanceStates.find(cKey);
            if (it != this->cellInstanceStates.end())
            {
                return it->second;
            }
            return nullptr;
        }

    }; // end of class
}; // end of namespace
