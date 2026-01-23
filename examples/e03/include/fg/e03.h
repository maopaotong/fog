#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
#include "MapGen.h"
#include "DualMap.h"
#include "ElevationGen.h"
#include "ColorMap.h"

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

    struct Pass0302 : public Ogre::CustomCompositionPass
    {
        struct RenderOperation : public Ogre::CompositorInstance::RenderSystemOperation
        {
            Ogre::Entity *entity;
            Ogre::SceneManager *sceMgr;
            Ogre::Viewport *vp;
            RenderOperation(Ogre::SceneManager *sceMgr, Ogre::Viewport *vp, Ogre::Entity *entity)
                : entity(entity), sceMgr(sceMgr)
            {
            }
            void execute(Ogre::SceneManager *sm, Ogre::RenderSystem *rs) override
            {
                rs->_setViewport(vp);
                Ogre::RenderQueue *q = sceMgr->getRenderQueue();
                q->clear();
                entity->_updateRenderQueue(q);
            }
        };
        Ogre::Entity *entity;
        Ogre::SceneManager *sceMgr;
        Pass0302(Ogre::SceneManager *sceMgr, Ogre::Entity *entity) : sceMgr(sceMgr), entity(entity)
        {
        }

        Ogre::CompositorInstance::RenderSystemOperation *createOperation(
            Ogre::CompositorInstance *instance, const Ogre::CompositionPass *pass)
        {
            Ogre::Viewport *vp = instance->getChain()->getViewport();
            return new RenderOperation(sceMgr, vp, entity);
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

            ColorMap::createTexture("tex_cm");

            Args mArgs;
            DualMesh mesh(MapGen::generateDualData(mArgs));
            DualMap map(mesh);
            float island = 0.5;
            ElevationGen::Constraints constraints(128, island);
            ElevationGen::Args eArgs;
            ElevationGen eGen{mesh, map.elevation_t, map.elevation_r};
            eGen.assignElevation(constraints, eArgs);
            setupObj(map);
            //setup 
            setupCompositor();
            core->addFrameListener(this);

            Ogre::SceneNode *cNode = core->getCameraSceneNode();
            cNode->setPosition(0, 0, 500);
            cNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);

            Ogre::GpuProgramManager &gpuMgr = Ogre::GpuProgramManager::getSingleton();

            setProjection("E03Mat00", core, sceNode);
            setProjection("E03Mat01", core, sceNode);
            //setProjection("E03Mat02", core, sceNode);
            
        }

        static void setProjection(std::string matName, CoreMod *core, Ogre::SceneNode * sceNode)
        {

            Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(matName);
            Ogre::Technique *tech = material->getTechnique(0);
            Ogre::Pass *pass = tech->getPass(0);
            Ogre::GpuProgramParametersSharedPtr sParams = pass->getVertexProgramParameters();
            Ogre::Matrix4 proj = core->getCameraWorldViewProj(sceNode, true);
            sParams->setNamedConstant("projection", proj);
        }

        void setupCompositor()
        {
            Ogre::Viewport *vp = core->getViewport();
            Ogre::CompositorInstance *ci = Ogre::CompositorManager::getSingleton().addCompositor(vp, "E03Comp01");
            ci->setEnabled(true);
            // ci->addListener(this);
            //  Ogre::CompositorManager::getSingleton().registerCustomCompositionPass("E03Pass01", new Pass0302());
        }
        void setupObj(DualMap &map)
        {
            std::string meshName = "LandMesh";
            setupMesh(map, meshName);
            Ogre::Entity *entity0 = core->createEntity(meshName);
            entity0->setMaterialName("E03Mat00");
            entity0->setVisibilityFlags(0x1 << 0);
            sceNode->attachObject(entity0);
            //
            Ogre::Entity *entity1 = core->createEntity(meshName);
            entity1->setMaterialName("E03Mat01");
            entity1->setVisibilityFlags(0x1 << 1);
            sceNode->attachObject(entity1);
            //
            Ogre::Entity *entity2 = core->createEntity(meshName);
            entity2->setMaterialName("E03Mat02");
            entity2->setVisibilityFlags(0x1 << 2);
            sceNode->attachObject(entity2);
            
            //
        }

        void setupMesh(DualMap &map, std::string meshName)
        {
            DualMesh &mesh = map.mesh;
            std::vector<float> &elevation_r = map.elevation_r;

            unsigned int vCount = mesh.numRegions;
            unsigned int iCount = mesh.numSolidSides;
            unsigned int vSize = 3 + 2 + 2;               //
            OgreUtil::buildMesh(meshName, vCount, iCount, //
                                [](Ogre::VertexDeclaration *decl)
                                {
                                    decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
                                    decl->addElement(0, 4 * 3, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
                                    decl->addElement(0, 4 * 5, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 1); //
                                },
                                [&mesh, vSize, &elevation_r](float *vData)
                                {
                                    int vIdx = 0;
                                    for (int r = 0; r < mesh.numRegions; r++)
                                    {
                                        vData[vIdx * vSize + 0] = mesh.x_of_r(r);
                                        vData[vIdx * vSize + 1] = mesh.y_of_r(r);
                                        vData[vIdx * vSize + 2] = elevation_r[r];
                                        vData[vIdx * vSize + 3 + 0] = mesh.x_of_r(r);
                                        vData[vIdx * vSize + 3 + 1] = mesh.y_of_r(r);
                                        vData[vIdx * vSize + 5 + 0] = mesh.x_of_r(r);
                                        vData[vIdx * vSize + 5 + 1] = mesh.y_of_r(r);
                                        vIdx++;
                                    } //
                                },
                                [&mesh](unsigned int *iData) { //
                                    int iIdx = 0;
                                    int len = mesh.numSolidSides;
                                    for (int s = 0; s < len; s++)
                                    {
                                        iData[len - iIdx - 1] = mesh._triangles[s];
                                        iIdx++;
                                    }
                                });
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