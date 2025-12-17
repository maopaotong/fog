/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/Common.h"
#include "fg/Actor.h"
#include "fg/core/CoreMod.h" //TODO remove this include.
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/Config.h"
#include "fg/Manager.h"
namespace fog
{

    class CellInstanceState : public Actor
    {
        std::stack<ColourValue> colours;
        ManualObject *obj;
        CellKey cis;
        CoreMod *core;
        SceneManager *sceneManager;
        Config* config;
    public:
        INJECT(CellInstanceState(CellKey cis, CoreMod *core, SceneManager *sceneManager,Config* config))
            : sceneManager(sceneManager),
            config(config),
              cis(cis), core(core)
        {
            this->sceNode = sceneManager->getRootSceneNode()->createChildSceneNode();
            this->obj = sceneManager->createManualObject();
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
            // return Context<Node2D>::get()->to3D(Cell::getOrigin2D(cis),config->CELL_SCALE);
            return cis.transform3(*config->d2h2d3);
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
            // CostMap *costMap = Context<CostMap>::get();
            // const int cost = costMap->getCost(cell);
            // switch (cost)
            // {
            // case CostMap::OBSTACLE:
            //     color = Ogre::ColourValue::Red;
            //     return true;
            // case CostMap::DEFAULT_COST:
            //     color = Ogre::ColourValue(0.8f, 0.6f, 0.2f); // light Sand color
            //     return false;
            // case 2:
            //     color = Ogre::ColourValue(0.6f, 0.4f, 0.1f); // Dark Sand color
            //     return true;
            // case 3:
            //     color = Ogre::ColourValue(0.2f, 0.4f, 0.8f); // Water color
            //     return true;
            // default:
            //     color = Ogre::ColourValue(0.7f, 0.7f, 0.7f); // light gray

            //     return true;
            // }
            return false;
        }

        CellKey getCellKey()
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

    class CellInstanceStateManager : public Manager<CellInstanceState>
    {
        std::unordered_map<CellKey, CellInstanceState *, CellKey::Hash> cellInstanceStates;

        CoreMod *core;
        SceneManager *sceneManager;

        Config *config;

    public:
        INJECT(CellInstanceStateManager(CoreMod *core, SceneManager *sceneManager, Config *config,Component::Injector* injector))
            : config(config),
              sceneManager(sceneManager),
              core(core)

        {

            CellKey cell;
            injector->push<CellKey>(nullptr, [&cell]()
                                   { return cell; });
            for (int x = 0; x < config->TILES_RANGE.getWidth(); x++)
            {
                for (int y = 0; y < config->TILES_RANGE.getHeight(); y++)
                {
                    cell = CellKey(x, y);
                    CellInstanceState *state = injector->getPtr<CellInstanceState>();
                    state->init();

                    this->add(state); //
                    this->cellInstanceStates[cell] = state;
                }
            }
            injector->pop<CellKey>();
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
