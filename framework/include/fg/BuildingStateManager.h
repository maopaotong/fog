/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/Common.h"
#include "fg/State.h"
#include "fg/core/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
#include "fg/core/Sinbad.h"
#include "fg/core/Tower.h"
#include "fg/InventoryStateManager.h"
#include "fg/core/Building.h"
#include "fg/core/H0085.h"
#include "fg/Config.h"
namespace fog
{

    class BuildingPlan
    {
    protected:
        CellKey cKey;
        State *building;
        float amount;
        CoreMod *core;

    public:
        BuildingPlan(State *building, float amount, CoreMod *core) : core(core),
                                                                     building(building), amount(amount)
        {
        }

        State *exchangeBuilding(State *state)
        {

            State *ret = std::exchange(this->building, state);

            return ret;
        }
        float getAmount() const
        {
            return this->amount;
        }

        State *getBuilding()
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

            // Vector3 pos3 = Context<Node2D>::get()->to3D(Cell::getOrigin2D(cKey),Config::CELL_SCALE);
            // Vector3 pos3 = cKey.cast<float>().transform(Transform::CellCentreByKey()).transform3(Config::D2H2D3);
            // this->building->findSceneNode()->setPosition(pos3);
            this->building->findSceneNode()->setPosition(cKey.transform3());
        }
    };

    class BuildingStateManager : public State
    {
        State *picked;
        BuildingPlan *plan;
        CoreMod *core;

        std::unordered_map<CellKey, std::vector<State *>, CellKey::Hash> buildingsInCells;

    protected:
        void setPicked(State *picked)
        {
            if (this->picked == picked)
            {
                return;
            }
            if (this->picked)
            {
                // unpick previous
                Context<Event::Bus>::get()-> //
                    emit<BuildingEventType, State *>(BuildingEventType::StateUnpicked, this->picked);
            }
            this->picked = picked;
            if (this->picked)
            {
                // pick new
                Context<Event::Bus>::get()-> //
                    emit<BuildingEventType, State *>(BuildingEventType::StatePicked, this->picked);
            }
        }

    public:
        INJECT(BuildingStateManager(CoreMod *core))
            : core(core),
              picked(nullptr), plan(nullptr)
        {
        }
        virtual ~BuildingStateManager()
        {
        }
        void init() override
        {
            Context<Event::Bus>::get()-> //
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
            Context<Event::Bus>::get()-> //
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

        bool pick(Ray &ray) // pick a building.
        {

            // 创建射线查询对象
            Ogre::RaySceneQuery *rayQuery = core->getSceneManager()->createRayQuery(ray);
            rayQuery->setSortByDistance(true);  // 按距离排序（最近的优先）
            rayQuery->setQueryMask(0x00000001); // 与 Entity 的查询掩码匹配

            // 执行查询
            Ogre::RaySceneQueryResult &result = rayQuery->execute();

            State *picked = nullptr;
            // 遍历结果
            for (auto &it : result)
            {
                Node *node = it.movable->getParentNode();
                State *s = State::get(node);
                if (s && s->pickable() && s->getParent() == this)
                {

                    picked = s;
                    break;
                }
            }
            // Context<CoreMod>::get()
            core->getSceneManager()->destroyQuery(rayQuery);
            this->setPicked(picked);
            return picked != nullptr;
        }

        bool planToBuild(BuildingType type) // plan a building and waiting mouse cell event to choose a position for the building.
        {
            if (this->plan)
            {
                float invAmount = this->plan->getAmount();
                Context<InventoryStateManager>::get()->returnInventory(InventoryType::BuildingPermit, invAmount);
                delete this->plan;
                this->plan = nullptr;
            }

            float invAmount = 1.0f;
            bool success = Context<InventoryStateManager>::get()->consumeInventory(InventoryType::BuildingPermit, invAmount);

            if (success)
            {
                State *building = nullptr;
                if (type == BuildingType::Tower)
                {
                    building = new Tower(core);
                }
                else if (type == BuildingType::H0085)
                {
                    building = new H0085(core);
                }
                else
                {
                    building = new Building(type, core);
                }
                building->init();

                this->plan = new BuildingPlan(building, invAmount, core);
            }

            return true;
        }

        void finishPlan(CellKey cKey)
        {

            auto it = this->buildingsInCells.find(cKey);
            if (it == this->buildingsInCells.end()) // cannot build two at the same cell
            {

                State *building = this->plan->exchangeBuilding(nullptr);
                this->addChild(building);
                this->buildingsInCells[cKey].push_back(building);
            }
            else
            { // failed to build on the target cell.
                float invAmount = this->plan->getAmount();
                Context<InventoryStateManager>::get()->returnInventory(InventoryType::BuildingPermit, invAmount);
            }

            delete this->plan;
            this->plan = nullptr;
        }
    }; // end of class
}; // end of namespace
