/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/Config.h"
namespace fog
{

    class CellInstanceState : public State
    {
        std::stack<ColourValue> colours;
        ManualObject *obj;
        HexTile::Key cis;

    public:
        CellInstanceState(HexTile::Key cis) : cis(cis)
        {
            this->sceNode = Context<CoreMod>::get()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
            this->obj = Context<CoreMod>::get()->getSceneManager()->createManualObject();
            this->sceNode->attachObject(this->obj);
        }

        virtual ~CellInstanceState()
        {
        }

        void init() override
        {
            ColourValue bottomColour;
            bool hasBottomColor = getCostColor(cis, bottomColour);
            if (hasBottomColor)
            {
                colours.push(bottomColour);
            }

            buildMesh();
        }

        Vector3 getOrigin3D()
        {
            // return Context<Node2D>::get()->to3D(Cell::getOrigin2D(cis),Config::CELL_SCALE);
            return cis.transform3();
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
                return false;
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

        HexTile::Key getCellKey()
        {
            return this->cis;
        }

        void popColour()
        {
            // if (!this->colours.empty())
            //{
            this->colours.pop();
            //}
            buildMesh();
        }

        void pushColour(ColourValue color)
        {
            this->colours.push(color);
            buildMesh();
        }

    }; // end of class

    class CellInstanceManager : public State
    {
        std::unordered_map<HexTile::Key, CellInstanceState *, HexTile::Key::Hash> cellInstanceStates;

    public:
        CellInstanceManager()
        {
        }
        virtual ~CellInstanceManager()
        {
        }
        void init() override
        {
            this->sceNode = Context<CoreMod>::get()->getSceneManager()->getRootSceneNode()->createChildSceneNode();

            for (int x = 0; x < Config::TILES_RANGE.getWidth(); x++)
            {
                for (int y = 0; y < Config::TILES_RANGE.getHeight(); y++)
                {
                    HexTile::Key cell(x, y);
                    CellInstanceState *state = new CellInstanceState(cell);
                    state->init();
                    this->addChild(state); //
                    this->cellInstanceStates[cell] = state;
                }
            }
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
            HexTile::Key cKey = HexTile::Key::from(pos);
            return getCellInstanceStateByCellKey(cKey);
        }
        CellInstanceState *getCellInstanceStateByCellKey(HexTile::Key cKey)
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
