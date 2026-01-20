#pragma once
#include "Common.h"

namespace fog
{
    struct OgreUtil
    {
        static Ogre::Camera *setupCamera(Ogre::SceneManager *sceMgr)
        {
            // Create camera
            Ogre::Camera *camera = sceMgr->createCamera("Cam01");
            camera->setNearClipDistance(0.1f);
            camera->setFarClipDistance(0.0f);
            camera->setAutoAspectRatio(true);

            // Create camera node and set position and direction
            Ogre::SceneNode *cameraNode = sceMgr->getRootSceneNode()->createChildSceneNode("CameraNode");
            cameraNode->setPosition(0, 5000, 0); //
            cameraNode->attachObject(camera);
            cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);
            return camera;
        }

        template <typename E, typename V, typename I>
        static void buildMesh(std::string name, unsigned int vCount, unsigned int iCount, E &&declFunc, V &&vDataFunc, I iDataFunc)
        {

            Ogre::MeshPtr meshPtr = Ogre::MeshManager::getSingleton().createManual(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            Ogre::SubMesh *subMesh = meshPtr->createSubMesh();
            subMesh->useSharedVertices = false;
            Ogre::VertexDeclaration *decl = Ogre::HardwareBufferManager::getSingleton().createVertexDeclaration();
            subMesh->vertexData = new Ogre::VertexData();
            subMesh->vertexData->vertexDeclaration = decl;
            subMesh->vertexData->vertexCount = vCount; //
            declFunc(decl);

            // vertex data
            size_t vSize = decl->getVertexSize(0);
            Ogre::HardwareVertexBufferSharedPtr vBufPtr = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vSize, vCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
            float *vData = static_cast<float *>(vBufPtr->lock(Ogre::HardwareBuffer::HBL_DISCARD));
            vDataFunc(vData);

            vBufPtr->unlock();
            subMesh->vertexData->vertexBufferBinding->setBinding(0, vBufPtr);
            // index data.
            subMesh->indexData->indexCount = iCount;

            subMesh->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                Ogre::HardwareIndexBuffer::IT_32BIT, // 或 IT_16BIT
                iCount,
                Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            uint32_t *iData = static_cast<uint32_t *>(subMesh->indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
            iDataFunc(iData);

            subMesh->indexData->indexBuffer->unlock();

            subMesh->operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

            // mesh bounds
            meshPtr->_setBounds(Ogre::AxisAlignedBox(-1000, -1000, -1, 1000, 1000, 1));

            //
        }

        static void buildExampleMesh(std::string name)
        {
            unsigned int vCount = 3;
            OgreUtil::buildMesh(name, vCount, 3, [vCount](Ogre::VertexDeclaration *decl)
                                {
                                    decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
                                    decl->addElement(0, 4 * 3, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
                                    decl->addElement(0, 4 * 5, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 1); //
                                },
                                [](float *vData)
                                {
                                    int idx = 0;
                                    vData[idx++] = 0; // x
                                    vData[idx++] = 0; // y
                                    vData[idx++] = 0; // z
                                    vData[idx++] = 0; // u
                                    vData[idx++] = 0; // v
                                    vData[idx++] = 0; // u
                                    vData[idx++] = 0; // v

                                    vData[idx++] = 1; // x
                                    vData[idx++] = 0; // y
                                    vData[idx++] = 0; // z
                                    vData[idx++] = 1; // u
                                    vData[idx++] = 0; // v
                                    vData[idx++] = 1; // u
                                    vData[idx++] = 0; // v

                                    vData[idx++] = 0;  // x
                                    vData[idx++] = -1; // y
                                    vData[idx++] = 0;  // z
                                    vData[idx++] = 0;  // u
                                    vData[idx++] = -1; // v
                                    vData[idx++] = 0;  // u
                                    vData[idx++] = -2; // v
                                },
                                [](uint32_t *iData)
                                {
                                    int idx = 0;
                                    iData[idx++] = 0;
                                    iData[idx++] = 1;
                                    iData[idx++] = 2; //
                                });
        }
        struct SimpleApp{

        };
    };
};