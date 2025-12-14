/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "util.h"
namespace fog
{

    using namespace Ogre;
    using namespace OgreBites;
    class Mod
    {
    public:
        class Active
        {
        };
        class Deactive
        {
        };

    protected:
    public:
        virtual std::string getName() = 0;
        // befor active, this method allow a mod to register callback into the underlying mod.
        virtual void setup(Component::Injector & injector) = 0;
        virtual void active() = 0;
        virtual void deactive() = 0;
    };
};
