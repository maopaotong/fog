#pragma once

#include "fg/util.h"
#include "fg/ogre.h"

namespace fog::examples::e03
{

    struct RenderTarget
    {

        Ogre::RenderTexture * rTex;
        RenderTarget(std::string texName, int w, int h, Ogre::SceneManager *sceneMgr, Ogre::uint32 mask)
        {
            Ogre::TexturePtr texElevation = Ogre::TextureManager::getSingleton().createManual(
                texName,
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::TEX_TYPE_2D,
                w, h,
                0, // mipmaps
                Ogre::PF_BYTE_RGB,
                Ogre::TU_RENDERTARGET);
            rTex = texElevation->getBuffer()->getRenderTarget();

            Ogre::Camera *cam = sceneMgr->createCamera(texName + "-Cam");
            cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
            cam->setOrthoWindow(w, h); //
            Ogre::SceneNode *camNode = sceneMgr->createSceneNode();
            camNode->attachObject(cam);
            camNode->setPosition(0, 0, 100);
            camNode->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f), Ogre::Node::TS_PARENT);

            // viewport
            Ogre::Viewport *vp = rTex->addViewport(cam);
            vp->setClearEveryFrame(true);
            vp->setBackgroundColour(Ogre::ColourValue::Black);
            vp->setVisibilityMask(mask); //
        }
        void update(){
            rTex->update();
        }
    };
};