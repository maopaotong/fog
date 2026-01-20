#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
#include "MapGen.h"
namespace fog::examples::e03
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
            rgm.addResourceLocation("fog/examples/e03/materials", "FileSystem", "App");
        }
    };
    struct TheCore : public SimpleCore
    {

        INJECT(TheCore(ImGuiAppContext *appCtx)) : SimpleCore(appCtx)
        {
        }

        SELF(TheCore)

        INIT(init)()
        {
            SimpleCore::init();
        }
    };

    struct Example : public Ogre::FrameListener
    {

        Ogre::ManualObject *obj;
        CoreMod *core;
        Ogre::SceneNode *sceNode;
        SELF(Example)

        INJECT(Example(CoreMod *core, Ogre::SceneNode *sceNode)) : core(core), sceNode(sceNode)
        {
        }
        INIT(init)()
        {
            Data data = MapGen::setupData();
            setupObj(data);
            // setupObj2(data);
            setupCompositor();
            core->addFrameListener(this);
            Ogre::SceneNode *cNode = core->getCameraSceneNode();
            cNode->setPosition(0, 0, 10000);
            cNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);
        }

        void setupCompositor()
        {
            Ogre::Viewport *vp = core->getViewport();
            Ogre::CompositorManager::getSingleton().addCompositor(vp, "E03Comp01");
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "E03Comp01", true);
        }
        void setupObj(Data &data)
        {
            DualMesh mesh(data);
            unsigned int vCount = mesh.numRegions;
            unsigned int iCount = mesh.numSolidSides;
            unsigned int vSize = 3 + 2 + 2; //
            std::string meshName = "LandMesh";
            OgreUtil::buildMesh(meshName, vCount, iCount, //
                                [](Ogre::VertexDeclaration *decl)
                                {
                                    decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
                                    decl->addElement(0, 4 * 3, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
                                    decl->addElement(0, 4 * 5, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 1); //
                                },
                                [&mesh, vSize](float *vData)
                                {
                                    int vIdx = 0;
                                    for (int r = 0; r < mesh.numRegions; r++)
                                    {
                                        vData[vIdx * vSize + 0] = mesh.x_of_r(r);
                                        vData[vIdx * vSize + 1] = mesh.y_of_r(r);
                                        vData[vIdx * vSize + 2] = 0.0f;
                                        vData[vIdx * vSize + 3 + 0] = mesh.x_of_r(r);
                                        vData[vIdx * vSize + 3 + 1] = mesh.y_of_r(r);
                                        vData[vIdx * vSize + 5 + 0] = mesh.x_of_r(r);
                                        vData[vIdx * vSize + 5 + 1] = mesh.y_of_r(r);
                                        vIdx++;
                                    } //
                                },
                                [&mesh](unsigned int *iData) { //
                                    int iIdx = 0;
                                    for (int s = 0; s < mesh.numSolidSides; s++)
                                    {
                                        iData[iIdx] = mesh._triangles[s];
                                        iIdx++;
                                    }
                                    std::reverse(iData,iData+mesh.numSolidSides);
                                });

            // entity
            Ogre::Entity *entity = core->createEntity(meshName);
            entity->setMaterialName("E03Mat01");
            sceNode->attachObject(entity);
            sceNode->setScale(30, 30, 30);
        }

        bool frameRenderingQueued(const Ogre::FrameEvent &evt) override
        {
            return true;
        }

        static int run(Options::Groups &ogs)
        {
            return SimpleApp::run<TheImGuiAppContext, TheCore, Example>(ogs);
        }
    };
};