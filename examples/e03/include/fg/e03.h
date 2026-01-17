#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
namespace fog::examples::e03
{
    struct Example : public Ogre::FrameListener
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

        Ogre::ManualObject *obj;
        CoreMod *core;
        Ogre::SceneNode *sceNode;
        SELF(Example)

        INJECT(Example(CoreMod *core, Ogre::SceneNode *sceNode)) : core(core), sceNode(sceNode)
        {
        }
        INIT(setupAll)()
        {
            setupObj();
            setupCompositor();
            core->addFrameListener(this);
        }
        void setupCompositor()
        {
            Ogre::Viewport *vp = core->getViewport();
            Ogre::CompositorManager::getSingleton().addCompositor(vp, "E03Comp01");
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "E03Comp01", true);
        }
        void setupObj()
        {

            Ogre::MeshPtr meshPtr = Ogre::MeshManager::getSingleton().createManual("LandMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            Ogre::SubMesh *subMesh = meshPtr->createSubMesh();
            subMesh->useSharedVertices = false;
            Ogre::VertexDeclaration *decl = Ogre::HardwareBufferManager::getSingleton().createVertexDeclaration();
            decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
            decl->addElement(0, sizeof(float) * 3, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

            subMesh->vertexData = new Ogre::VertexData();
            subMesh->vertexData->vertexDeclaration = decl;
            size_t vCount = 3;
            subMesh->vertexData->vertexCount = vCount;

            // vertex data
            size_t vSize = decl->getVertexSize(0);
            Ogre::HardwareVertexBufferSharedPtr vBufPtr = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vSize, vCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
            float *vData = static_cast<float *>(vBufPtr->lock(Ogre::HardwareBuffer::HBL_DISCARD));
            int idx = 0;

            vData[idx++] = 0; // x
            vData[idx++] = 0; // y
            vData[idx++] = 0; // z
            vData[idx++] = 0; // u
            vData[idx++] = 0; // v

            vData[idx++] = 1; // x
            vData[idx++] = 0; // y
            vData[idx++] = 0; // z
            vData[idx++] = 0; // u
            vData[idx++] = 0; // v

            vData[idx++] = 0;  // x
            vData[idx++] = 0;  // y
            vData[idx++] = -1; // z
            vData[idx++] = 0;  // u
            vData[idx++] = 0;  // v

            vBufPtr->unlock();
            subMesh->vertexData->vertexBufferBinding->setBinding(0, vBufPtr);
            // index data.
            int iCount = 3;
            subMesh->indexData->indexCount = iCount;

            subMesh->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                Ogre::HardwareIndexBuffer::IT_32BIT, // 或 IT_16BIT
                iCount,
                Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            uint32_t *iData = static_cast<uint32_t *>(subMesh->indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
            idx = 0;
            iData[idx++] = 0;
            iData[idx++] = 1;
            iData[idx++] = 2;

            subMesh->indexData->indexBuffer->unlock();

            subMesh->operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

            // mesh bounds
            meshPtr->_setBounds(Ogre::AxisAlignedBox(-1000, -1000, -1, 1000, 1000, 1));
            meshPtr->_setBoundingSphereRadius(2000);

            // entity
            Ogre::Entity *entity = core->createEntity("LandMesh");
            entity->setMaterialName("E03Mat01");
            sceNode->attachObject(entity);
            sceNode->setScale(30, 30, 30);

            //
        }

        bool frameRenderingQueued(const Ogre::FrameEvent &evt) override
        {

            // obj->setMaterialName(0, "E02Material");
            // obj->setVisible(false);

            return true;
        }

        static int run(Options::Groups &ogs);
    };
};