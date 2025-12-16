/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Mod.h"
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
            mod->active();
            mods.push_back(ModHolder(mod, injector));
        }

        virtual void setup() = 0;
        virtual void close() = 0;
    };
};
