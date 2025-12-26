/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/actors.h"
#include "fg/cells.h"
#include "Common.h"
#include "InventoryStateManager.h"

namespace fog
{

    class BuildingLocator
    {
    protected:
        std::unique_ptr<Actor> building;
        float amount;
        CoreMod *core;
        Transforms *tfs;
        std::unordered_map<CellKey::Offset, std::vector<Actor *>, CellKey::Offset::Hash> &buildingsInCells;

    public:
        BuildingLocator(std::unique_ptr<Actor> building, float amount, CoreMod *core, Transforms *tfs,
                        std::unordered_map<CellKey::Offset, std::vector<Actor *>, CellKey::Offset::Hash> &buildingsInCells) : core(core),
                                                                                                              tfs(tfs),
                                                                                                              building(std::move(building)), amount(amount),
                                                                                                              buildingsInCells(buildingsInCells)
        {
        }

        float getAmount() const
        {
            return this->amount;
        }

        Actor *getBuilding()
        {
            return building.get();
        }

        virtual ~BuildingLocator()
        {
        }

        void moveTo(CellKey::Offset cKey)
        {
            if (this->building)
            {
                this->building->getSceneNode()->setPosition(cKey.transform3(*tfs->d2td3));
            }
        }

        bool tryBuildAt(CellKey::Offset cKey)
        {
            if (this->building)
            {
                auto it = this->buildingsInCells.find(cKey);
                if (it == this->buildingsInCells.end()) // cannot build two at the same cell
                {
                    this->buildingsInCells[cKey].push_back(building.release());
                    return true;
                }
            }
            return false;
        }
    };

    struct BuildingStateManager : public Manager<Actor>
    {
        SELF(BuildingStateManager)
        
        Actor *picked;
        std::unique_ptr<BuildingLocator> locator;
        CoreMod *core;
        std::unordered_map<CellKey::Offset, std::vector<Actor *>, CellKey::Offset::Hash> buildingsInCells;
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
        EventBus *eventBus;
        Injector *injector;
        Transforms *tfs;
        Building::Args args;

    public:
        INJECT(BuildingStateManager(CoreMod *core,
                                    Transforms *tfs,
                                    InventoryManager *inventoryManager,
                                    Building::Args args,
                                    Config *config,
                                    Injector *injector,
                                    EventBus *eventBus))
            : core(core),
              args(args),
              injector(injector),
              tfs(tfs),
              config(config),
              eventBus(eventBus),
              inventoryManager(inventoryManager),
              picked(nullptr), locator(nullptr)
        {
            eventBus-> //
                subscribe<MouseCellEventType, CellKey::Offset>([this](MouseCellEventType type, CellKey::Offset cKey)
                                                       {
                                                           if (type == MouseCellEventType::MouseEnterCell)
                                                           {
                                                               if (this->locator) // move the position if has plan
                                                               {
                                                                   this->locator->moveTo(cKey);
                                                                   std::cout << "Locator move to cell: (" << cKey.x << "," << cKey.y << ")" << std::endl;
                                                               }
                                                           }

                                                           return true; //
                                                       });
            eventBus-> //
                subscribe<CellEventType, CellKey::Offset>([this](CellEventType type, CellKey::Offset cKey)
                                                  {
                                                      if (type == CellEventType::CellAsTarget)
                                                      {
                                                          if (this->locator) // finish the plan by set the position of the building.
                                                          {
                                                              if (this->locator->tryBuildAt(cKey))
                                                              {
                                                                  locator.reset();
                                                              }
                                                          }
                                                      }
                                                      return true; //
                                                  });
        }

        INIT(init)()
        {
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

        bool startLocator(BuildingType type) // plan a building and waiting mouse cell event to choose a position for the building.
        {

            if (this->locator)
            {
                float invAmount = this->locator->getAmount();
                inventoryManager->returnInventory(InventoryType::BuildingPermit, invAmount);
                this->locator.reset();
                return true; //
            }

            float invAmount = 1.0f;
            bool success = inventoryManager->consumeInventory(InventoryType::BuildingPermit, invAmount);

            if (success)
            {
                std::unique_ptr<Building> bd = std::make_unique<Building>(type, tfs, core, core->getRootSceneNode()->createChildSceneNode(), args);
                locator = std::make_unique<BuildingLocator>(std::move(bd), invAmount, core, tfs, buildingsInCells);
            }

            return true;
        }

    }; // end of class
}; // end of namespace
