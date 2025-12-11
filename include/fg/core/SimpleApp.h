/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
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
        std::vector<Mod *> list;
        std::unordered_map<std::string, Mod *> map;

    public:
        SimpleApp()
        {
            this->add(new SimpleCore());
        }
        virtual ~SimpleApp()
        {
        }

        void add(Mod *mod) override
        {

            std::string name = mod->getName();
            if (map.find(name) != map.end())
            {
                throw std::runtime_error("Module already exists:" + name);
            }
            map[name] = mod;
            list.push_back(mod);
        }

        void setup() override
        {
            for (auto it = list.begin(); it != list.end(); it++)
            {
                Mod *mod = *it;
                mod->setup();
            } //
            for (auto it = list.begin(); it != list.end(); it++)
            {
                Mod *mod = *it;
                mod->active();
            } //
            
        }

        void startRendering() override
        {

            Ogre::Root *root = Context<CoreMod>::get()->getRoot();
            root->startRendering(); //
        }

        void close() override
        {
            std::cout << "Closing application.\n";
            for (auto it = list.rbegin(); it != list.rend(); it++)
            {
                Mod *mod = *it;
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
