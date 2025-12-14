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
    protected:
        Component::Injector injector; //
        std::vector<std::type_index> modIds;

    public:
        template <typename T>
        void add()
        {
            static_assert(std::is_base_of<Mod, T>::value, "cannot add a type of non-Mod");
            add<T, T>();
        }

        template <typename T, typename M>
        void add()
        {
            static_assert(std::is_base_of<Mod, T>::value, "cannot add a type of non-Mod");
            injector.bindImpl<T, M, Mod>();
            modIds.push_back(typeid(T));//remember the mod type as id.
        }

        virtual void setup() = 0;
        virtual void startRendering() = 0;
        virtual void close() = 0;
    };
};
