#pragma once

#include "Game01.h"
namespace fog
{

    struct Example01 : public App
    {
        INJECT(Example01(Injector *injector))
        {
            this->add(*injector, SimpleCore::Setup());
            this->add(*injector, Game01::Setup()); //
            injector->get<CoreMod>()->startRendering();
        }

        static int run(Options::Groups &ogs);
    };
};