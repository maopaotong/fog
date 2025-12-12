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
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
#include "fg/core/Sinbad.h"
#include "fg/core/Tower.h"

namespace fog
{

    class Inventory : public State
    {
        InventoryType type;
        float amount;
        float capacity;

    public:
        Inventory(InventoryType type) : type(type), amount(0.0f), capacity(1e6f)
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
        float getCapacity() const
        {
            return this->capacity;
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

    class InventoryStateManager : public State
    {

    protected:
    public:
        InventoryStateManager()
        {
        }
        virtual ~InventoryStateManager()
        {
        }
        void init() override
        {
            this->add(InventoryType::BuildingPermit, 100.0f);
            this->add(InventoryType::Population, 10.0f);
        }

        void add(InventoryType type, float amount)
        {
            Inventory *inv = new Inventory(type);
            inv->init();
            inv->add(amount); //
            this->addChild(inv);
        }

        template <typename F>
        void forEachInventory(F &&func)
        {
            this->forEachChild([&func](State *state)
                               {
                                   Inventory *inv = dynamic_cast<Inventory *>(state);
                                   if (inv)
                                   {
                                       return func(inv);
                                   }
                                   return true; //
                               });
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
        float getCapacity(InventoryType type)
        {
            Inventory *inv = this->getInventory(type);
            if (!inv)
            {
                return 0.0f;
            }
            return inv->getCapacity();
        }

        Inventory *getInventory(InventoryType type)
        {
            Inventory *result = nullptr;
            this->forEachInventory([&](Inventory *inv)
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
