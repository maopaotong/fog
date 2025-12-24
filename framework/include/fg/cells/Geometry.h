#pragma once
#include "Common.h"
#include "util.h"

namespace fog
{

    struct Geometry
    {
        struct Args
        {
            SELF(Args)             //
            GROUP("geometry#args") //
            MEMBER(float, groundHeight)  //

        };

        Ogre::Plane ground;
        INJECT(Geometry(Args args)) : ground(Ogre::Vector3::UNIT_Y, args.groundHeight)
        {
        }
    };
};