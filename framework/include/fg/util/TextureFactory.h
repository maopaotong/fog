/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>
#include "stb_truetype.h"
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>


namespace fog
{
    // === Custom hash function ===
    //
    // === Hexagonal Map Visualizer class ===
    //using namespace Ogre;
    //using namespace OgreBites;

    class TextureFactory
    {
    private:
    public:
        static Ogre::TexturePtr createTexture(std::string name, int width, int height, Ogre::PixelFormat format = Ogre::PF_BYTE_RGBA)
        {

            // 上传到 GPU 纹理

           Ogre::TexturePtr tex = getTexture(name);
            if (tex)
            {
                std::cout << "Bug? any way remove texture:" << name << std::endl;
                Ogre::TextureManager::getSingleton().remove(name);
            }
            std::cout << "Create texture:" << name << std::endl;

            tex = Ogre::TextureManager::getSingleton().createManual(
                name,
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::TEX_TYPE_2D,
                width, height, 0,
                format,
                Ogre::TU_DEFAULT // only this option works fine; other option does not work(cannot read texture from fragment shader);
                // Ogre::TU_DYNAMIC_WRITE_ONLY
                // Ogre::TU_STATIC_WRITE_ONLY
            );

            // updateTexture(tex, width, height, data, format);
            return tex;
        }

        static Ogre::TexturePtr createTexture2(std::string name, int width, int height, Ogre::PixelFormat format = Ogre::PF_BYTE_RGBA)
        {

            // 上传到 GPU 纹理

           Ogre::TexturePtr tex = getTexture(name);
            if (tex)
            {
                std::cout << "Bug? any way remove texture:" << name << std::endl;
                Ogre::TextureManager::getSingleton().remove(name);
            }
            std::cout << "Create texture:" << name << std::endl;

            tex = Ogre::TextureManager::getSingleton().createManual(
                name,
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::TEX_TYPE_2D,
                width, height, 0,
                format,
                //Ogre::TU_DEFAULT // only this option works fine; other option does not work(cannot read texture from fragment shader);
                //Ogre::TU_DYNAMIC_WRITE_ONLY
                Ogre::TU_STATIC_WRITE_ONLY
            );

            // updateTexture(tex, width, height, data, format);
            return tex;
        }

        static Ogre::TexturePtr getTexture(std::string name)
        {
            return Ogre::TextureManager::getSingleton().getByName(name);
        }

        static void updateTexture(Ogre::TexturePtr tex, int width, int height, unsigned char *data, Ogre::PixelFormat format = Ogre::PF_BYTE_RGBA)
        {
            Ogre::HardwarePixelBufferSharedPtr buffer = tex->getBuffer();
            Ogre::PixelBox pBox(width, height, 1, format, data);
            buffer->lock(
                Ogre::HardwareBuffer::HBL_READ_ONLY
                //Ogre::HardwareBuffer::HBL_DISCARD //memory access error when set to any other option.
                //Ogre::HardwareBuffer::HBL_NORMAL//
            );
            buffer->blitFromMemory(pBox);
            buffer->unlock();
        }

        static void updateTexture2(Ogre::TexturePtr tex, int width, int height, unsigned char *data, Ogre::PixelFormat format = Ogre::PF_BYTE_RGBA)
        {
            Ogre::HardwarePixelBufferSharedPtr buffer = tex->getBuffer();
            Ogre::PixelBox pBox(width, height, 1, format, data);
            buffer->lock(
                Ogre::HardwareBuffer::HBL_READ_ONLY
                //Ogre::HardwareBuffer::HBL_DISCARD // case 1(example01).memory access error when set to any other option.,cast 2(example02): texture is all gray when set to this option.
                //Ogre::HardwareBuffer::HBL_NORMAL //  case 1(example01).memory access error when set to any other option.,cast 2(example02): texture is all gray when set to this option.
            );
            buffer->blitFromMemory(pBox);
            buffer->unlock();
        }

        static void updateTexture(Ogre::TexturePtr tex, int width, int height, unsigned char *data, Box2<int> box2, Ogre::PixelFormat format = Ogre::PF_BYTE_RGBA)
        {
            Ogre::HardwarePixelBufferSharedPtr buffer = tex->getBuffer();
            Ogre::PixelBox pBox(width, height, 1, format, data);
            buffer->lock(
                Ogre::HardwareBuffer::HBL_READ_ONLY
                // Ogre::HardwareBuffer::HBL_DISCARD memory access error when set to any other option.
            );
            buffer->blitFromMemory(pBox, Ogre::Box(box2.p1.x, box2.p1.y, box2.p2.x, box2.p2.y));
            buffer->unlock();
        }
    };

}; // end of namespace
