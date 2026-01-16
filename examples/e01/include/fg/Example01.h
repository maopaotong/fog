#pragma once

#include "Game01.h"
namespace fog
{

    struct Example01 : public App
    {
        struct TheImGuiAppContext : public ImGuiAppContext
        {
            INJECT(TheImGuiAppContext(Args opts, ImGuiAppImpl *imGuiApp)) : ImGuiAppContext(opts, imGuiApp)
            {
            }

            SELF(TheImGuiAppContext)
            INIT(init)()
            {
                ImGuiAppContext::init();
            }

            void locateResources() override
            {
                // ApplicationContextBase::locateResources();
                auto &rgm = Ogre::ResourceGroupManager::getSingleton();

                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/Main", "FileSystem", "OgreInternal");
                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/RTShaderLib", "FileSystem", "OgreInternal");
                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/Terrain", "FileSystem", "OgreInternal");

                /*
                rgm.addResourceLocation("fog/examples/e00/materials", "FileSystem", "App");
                */
                rgm.addResourceLocation("doc/material", "FileSystem", "App");
                rgm.addResourceLocation("doc/textures", "FileSystem", "General");
                rgm.addResourceLocation("doc/sinbad", "FileSystem", "App");
            }
        };

        INJECT(Example01(Injector *injector))
        {
            this->add(*injector, SimpleCore::Setup());
            this->add(*injector, Game01::Setup()); //
            injector->get<CoreMod>()->startRendering();
        }

        static int run(Options::Groups &ogs);
    };
};