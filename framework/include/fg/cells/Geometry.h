#pragma once
#include "Common.h"
#include "util.h"

namespace fog
{

    struct Geometry
    {
        Config *config;
        Ogre::Plane ground;
        INJECT(Geometry(Config *config)) : config(config), ground(Ogre::Vector3::UNIT_Y, config->heightScale / 2.0)
        {
        }
    };
};