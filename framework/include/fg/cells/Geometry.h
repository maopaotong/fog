#pragma once
#include "Common.h"
#include "util.h"

namespace fog
{

    struct Geometry
    {
        struct Args
        {
            float groundHeight;
            SELF(Args)                            //
            GROUP("geometry#args")                //
            MEMBERK(groundHeight, "groundHeight") //
        };

        Ogre::Plane ground;
        INJECT(Geometry(Args args)) : ground(Ogre::Vector3::UNIT_Y, args.groundHeight)
        {
        }
    };
};