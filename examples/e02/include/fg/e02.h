#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
namespace fog::examples::e02
{
    struct Example02
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
                auto &rgm = Ogre::ResourceGroupManager::getSingleton();
                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/Main", "FileSystem", "OgreInternal");
                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/RTShaderLib", "FileSystem", "OgreInternal");
                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/Terrain", "FileSystem", "OgreInternal");
                rgm.addResourceLocation("fog/examples/e02/materials", "FileSystem", "App");
            }
        };

        INJECT(Example02(CoreMod *core, Ogre::SceneNode *sceNode))
        {
            Ogre::ManualObject *obj = core->createManualObject();
            sceNode->attachObject(obj);
            obj->clear();
            obj->begin("E02Material", Ogre::RenderOperation::OT_TRIANGLE_LIST);
            int baseIdx = obj->getCurrentVertexCount();
            Vector3 a{0, 0, 0};
            Vector3 b{1, 0, 0};
            Vector3 c{0, 0, -1};

            obj->position(a);
            obj->position(b);
            obj->position(c);

            obj->triangle(baseIdx + 0, baseIdx + 1, baseIdx + 2);

            obj->end();
            sceNode->setScale(30, 30, 30);
        }

        static int run(Options::Groups &ogs);
    };
};