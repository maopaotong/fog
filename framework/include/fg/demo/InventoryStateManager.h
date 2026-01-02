/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/ogre.h"
#include "fg/util.h"
#include "fg/actors.h"
//
#include "Common.h"
namespace fog
{
    struct InventoryTypeInfo
    {
        std::unordered_map<InventoryType, float> material;
        std::unordered_map<InventoryType, float> dailyCost;
        std::unordered_map<InventoryType, float> dailyProfit;

    };
    
    struct Inventory
    {
        InventoryType type;
        float amount;

    public:
        Inventory(InventoryType type) : type(type), amount(0.0f)
        {
        }
        virtual ~Inventory()
        {
        }

        void add(float amount)
        {
            this->amount += amount;
        }

        bool consume(float amount)
        {
            if (this->amount < amount)
            {
                return false;
            }
            this->amount -= amount;

            return true;
        }

        InventoryType getType() const
        {
            return this->type;
        }
        float getAmount() const
        {
            return this->amount;
        }
       
    };

    class InventoryMonitor : public Stairs
    {
    public:
        InventoryMonitor() : Stairs()
        {
        }
        virtual ~InventoryMonitor()
        {
        }

        bool step(float time) override
        {
            

            return true;
        }
    };

    struct InventoryManager : public Manager<Inventory>
    {

    protected:
    public:
        INJECT(InventoryManager())
        {
            this->add(InventoryType::Food, 100.0f);
            this->add(InventoryType::BuildingPermit, 100.0f);
            this->add(InventoryType::Worker, 10.0f);
            
        }
        virtual ~InventoryManager()
        {
        }

        void add(InventoryType type, float amount)
        {
            Inventory *inv = new Inventory(type);
            inv->add(amount); //
            this->Manager::add(inv);
        }

        void returnInventory(InventoryType type, float amount)
        {
            Inventory *inv = this->getInventory(type);
            if (!inv)
            {
                this->add(type, amount);
            }
            inv->add(amount);
        }

        bool consumeInventory(InventoryType type, float amount)
        {
            Inventory *inv = this->getInventory(type);
            if (!inv)
            {
                return false;
            }
            return inv->consume(amount);
        }

        float getAmount(InventoryType type)
        {
            Inventory *inv = this->getInventory(type);
            if (!inv)
            {
                return 0.0f;
            }
            return inv->getAmount();
        }
        
        Inventory *getInventory(InventoryType type)
        {
            Inventory *result = nullptr;
            this->forEach([&](Inventory *inv) -> bool
                          {
                              if (inv->getType() == type)
                              {
                                  result = inv;
                                  return false; // stop
                              }
                              return true; // continue
                          });
            return result;
        }

    }; // end of class
}; // end of namespace
