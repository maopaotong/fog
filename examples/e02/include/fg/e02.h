#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
namespace fog::examples::e02
{
    struct Example02 : public Ogre::FrameListener
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

        Ogre::ManualObject *obj;
        CoreMod *core;
        Ogre::SceneNode *sceNode;
        SELF(Example02)

        INJECT(Example02(CoreMod *core, Ogre::SceneNode *sceNode)) : core(core), sceNode(sceNode)
        {
        }
        INIT(setupAll)()
        {
            setupObj();
            setupCompositor();
            core->addFrameListener(this);
        }
        void setupCompositor(){
            Ogre::Viewport * vp = core->getViewport();
            Ogre::CompositorManager::getSingleton().addCompositor(vp, "E02Comp01");
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "E02Comp01", true);
        }
        void setupObj(){
            obj = core->createManualObject();
            sceNode->attachObject(obj);
            obj->clear();
            std::string mName;
            mName = "E02Mat01";
            obj->begin(mName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
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
            obj->setBoundingBox(Ogre::AxisAlignedBox(-50, -50, -50, 50, 50, 50));

            //
        }

        bool frameRenderingQueued(const Ogre::FrameEvent &evt) override
        {

            //obj->setMaterialName(0, "E02Material");
            //obj->setVisible(false);

            return true;
        }

        static int run(Options::Groups &ogs);
    };
};