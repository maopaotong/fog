/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>

#include "fg/ogre.h"
#include "fg/util.h"

#include "MaterialNames.h"
#define DEFAULT_CELL_HIGH_OFFSET .08f

namespace fog
{
    using namespace Ogre;

    //
    class ManualState : public Actor
    {

    protected:
        Ogre::ManualObject *obj;

        std::string material = MaterialNames::materialNameInUse;
        CoreMod * core;
    public:
        ManualState(CoreMod * core):core(core),Actor(core->getRootSceneNode()->createChildSceneNode())
        {

            obj = core->createManualObject();
            obj->setQueryFlags(0x00000001);
            sceNode->attachObject(obj);
        }

        virtual ~ManualState()
        {
            core->getRootSceneNode()->removeAndDestroyChild(sceNode);            
            this->sceNode = nullptr;
        }

        
    };

}; // end of namespace
