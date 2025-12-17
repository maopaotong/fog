/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "Common.h"
#include "Mod.h"
#include "util.h"
namespace fog
{
    class App
    {
        struct ModHolder
        {
            Mod *mod;
            Component::Injector &injector;
            ModHolder(Mod *mod, Component::Injector &injector) : mod(mod), injector(injector)
            {
            }
        };

    protected:
        std::vector<ModHolder> mods;

    public:
        template <typename F>
        void add(Component::Injector &injector, F &&setup)
        {
            Mod *mod = setup(injector);
            mods.push_back(ModHolder(mod, injector));
        }

        virtual void setup() = 0;
        virtual void close() = 0;
    };
};
