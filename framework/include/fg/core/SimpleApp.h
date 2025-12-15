/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>

#include "fg/util/HexGridPrinter.h"
#include "fg/Mod.h"
#include "fg/App.h"
#include "fg/core/SimpleCore.h"
namespace fog
{

    using namespace OgreBites;
    using namespace Ogre;
    class SimpleApp : public App
    {
    private:
    public: 
        SimpleApp()
        {
            
        }
        virtual ~SimpleApp()
        {
        }

        void setup() override
        {

            
        }

        void startRendering() override
        {

            Ogre::Root *root = injector.get<CoreMod>()->getRoot();
            root->startRendering(); //
        }

        void close() override
        {
            std::cout << "Closing application.\n";
            for (auto it = mods.rbegin(); it != mods.rend(); it++)
            {
                Mod *mod = it->mod;
                std::cout << "Disactive module:" << mod->getName() << "" << std::endl;
                mod->deactive();
                std::cout << "Done of disactive module." << std::endl;
            }

            ////TODO: context->close() at present time will crash the process.
            // Cause:
            //  After add the terrain code by module of GameTerrain.h.
            //
            // Root cause:
            //  Unknown.
            //
            // Research:
            //----------------------------------------
            // 1. The first error is an Ogre defined exception:
            // 1. Since SharderGenerator maintaned types of SubSharderFactory,
            // 1. One of the factory is FFPTransform factory, this factory will raise a exception saying the RenderState
            // 1. instance is not empty. It must be empty before destroy the factory.
            // 2. The second exception will raise if you fix the first one by destroying this factory's all instance
            // 2. before calling OgreBites::Application context::cose();
            // so we give up closing the context of ogre.
            // core->getAppContext()->closeApp();
            //----------------------------------------
        }
    };
}; // end of namespace
