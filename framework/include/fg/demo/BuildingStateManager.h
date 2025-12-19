/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util.h"

#include "fg/ogre.h"
#include "Common.h"
#include "fg/cells.h"
#include "Sinbad.h"
#include "Tower.h"
#include "InventoryStateManager.h"
#include "Building.h"
#include "H0085.h"
namespace fog
{

    class BuildingPlan
    {
    protected:
        CellKey cKey;
        Actor *building;
        float amount;
        CoreMod *core;
        Transforms * tfs;
    public:
        BuildingPlan(Actor *building, float amount, CoreMod *core, Transforms* tfs) : core(core),
                                                                                    tfs(tfs),
                                                                                     building(building), amount(amount)
        {
        }

        Actor *exchangeBuilding(Actor *state)
        {

            Actor *ret = std::exchange(this->building, state);

            return ret;
        }
        float getAmount() const
        {
            return this->amount;
        }

        Actor *getBuilding()
        {
            return this->building;
        }
        virtual ~BuildingPlan()
        {
            if (this->building)
            {
                delete this->building;
                this->building = nullptr;
            }
        }

        void moveToCell(CellKey cKey)
        {
            this->cKey = cKey;

            // Vector3 pos3 = Context<Node2D>::get()->to3D(Cell::getOrigin2D(cKey),config->CELL_SCALE);
            // Vector3 pos3 = cKey.cast<float>().transform(Transform::CellCentreByKey()).transform3(config->D2H2D3);
            // this->building->findSceneNode()->setPosition(pos3);
            this->building->getSceneNode()->setPosition(cKey.transform3(*tfs->d2td3));
        }
    };

    class BuildingStateManager : public Manager<Actor>
    {
        Actor *picked;
        BuildingPlan *plan;
        CoreMod *core;
        std::unordered_map<CellKey, std::vector<Actor *>, CellKey::Hash> buildingsInCells;
        InventoryManager *inventoryManager;
        Config *config;

    protected:
        void setPicked(Actor *picked)
        {
            if (this->picked == picked)
            {
                return;
            }
            if (this->picked)
            {
                // unpick previous
                eventBus-> //
                    emit<BuildingEventType, Actor *>(BuildingEventType::StateUnpicked, this->picked);
            }
            this->picked = picked;
            if (this->picked)
            {
                // pick new
                eventBus-> //
                    emit<BuildingEventType, Actor *>(BuildingEventType::StatePicked, this->picked);
            }
        }
        Event::Bus *eventBus;
        Component::Injector *injector;
        Transforms * tfs;

    public:
        INJECT(BuildingStateManager(CoreMod *core, 
            Transforms * tfs, 
            InventoryManager *inventoryManager,
                                    Config *config,
                                    Component::Injector *injector,
                                    Event::Bus *eventBus))
            : core(core),
              injector(injector),
              tfs(tfs),
              config(config),
              eventBus(eventBus),
              inventoryManager(inventoryManager),
              picked(nullptr), plan(nullptr)
        {
            eventBus-> //
                subscribe<MouseCellEventType, CellKey>([this](MouseCellEventType type, CellKey cKey)
                                                       {
                                                           if (type == MouseCellEventType::MouseEnterCell)
                                                           {
                                                               if (this->plan) // move the position if has plan
                                                               {
                                                                   this->plan->moveToCell(cKey);
                                                               }
                                                           }

                                                           return true; //
                                                       });
            eventBus-> //
                subscribe<CellEventType, CellKey>([this](CellEventType type, CellKey cKey)
                                                  {
                                                      if (type == CellEventType::CellAsTarget)
                                                      {
                                                          if (this->plan) // finish the plan by set the position of the building.
                                                          {
                                                              this->finishPlan(cKey);
                                                          }
                                                      }
                                                      return true; //
                                                  });
        }
        virtual ~BuildingStateManager()
        {
        }

        bool pick(Ray &ray) // pick a building.
        {

            //
            Ogre::RaySceneQuery *rayQuery = core->createRayQuery(ray);
            rayQuery->setSortByDistance(true);  //
            rayQuery->setQueryMask(0x00000001); //

            //
            Ogre::RaySceneQueryResult &result = rayQuery->execute();

            Actor *picked = nullptr;
            //
            for (auto &it : result)
            {
                Node *node = it.movable->getParentNode();
                Actor *s = Actor::get(node);
                if (s && s->pickable()) // TODO
                {

                    picked = s;
                    break;
                }
            }
            // Context<CoreMod>::get()
            core->destroyQuery(rayQuery);
            this->setPicked(picked);
            return picked != nullptr;
        }

        bool planToBuild(BuildingType type) // plan a building and waiting mouse cell event to choose a position for the building.
        {
            if (this->plan)
            {
                float invAmount = this->plan->getAmount();
                inventoryManager->returnInventory(InventoryType::BuildingPermit, invAmount);
                delete this->plan;
                this->plan = nullptr;
            }

            float invAmount = 1.0f;
            bool success = inventoryManager->consumeInventory(InventoryType::BuildingPermit, invAmount);

            if (success)
            {
                Actor *building = nullptr;
                if (type == BuildingType::Tower)
                {
                    building = injector->getPtr<Tower>();
                }
                else if (type == BuildingType::H0085)
                {
                    building = injector->getPtr<H0085>();
                }
                else
                {
                    building = new Building(type, tfs, core);
                }
                // building->init();

                this->plan = new BuildingPlan(building, invAmount, core, tfs);
            }

            return true;
        }

        void finishPlan(CellKey cKey)
        {

            auto it = this->buildingsInCells.find(cKey);
            if (it == this->buildingsInCells.end()) // cannot build two at the same cell
            {

                Actor *building = this->plan->exchangeBuilding(nullptr);
                this->add(building);
                this->buildingsInCells[cKey].push_back(building);
            }
            else
            { // failed to build on the target cell.
                float invAmount = this->plan->getAmount();
                inventoryManager->returnInventory(InventoryType::BuildingPermit, invAmount);
            }

            delete this->plan;
            this->plan = nullptr;
        }
    }; // end of class
}; // end of namespace
