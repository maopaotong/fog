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
#include <OgreRenderWindow.h>
#include <iostream>
#include "fg/util.h"
#include "fg/InputState.h"
#include "fg/core/CoreMod.h"
namespace fog
{
    using namespace OgreBites;
    using namespace Ogre;
    // === Custom hash function ===
    //
    // === Input handler for closing application ===
    class InputStateController : public InputState
    {
    private:
        bool left = false;
        bool right = false;
        bool front = false;
        bool back = false;
        CoreMod *core;

    public:
        INJECT(InputStateController(CoreMod *core)) : core(core)
        {
        }

        bool mouseMoved(const MouseMotionEvent &evt)
        {
            RenderWindow *window = core->getWindow();

            int width = window->getWidth();
            int height = window->getHeight();
            // 定义边缘区域（例如：10 像素）
            int edgeSize = 10;
            int x = evt.x;
            int y = evt.y;
            this->left = (x >= 0 && x <= edgeSize);
            this->right = (x >= width - edgeSize && x <= width);
            this->front = (y >= 0 && y <= edgeSize);
            this->back = (y >= height - edgeSize && y <= height);
            if (DEBUG_COUT)
            {

                if (this->isMoving())
                {
                    std::cout << "Moving:(" << x << "," << y << "),(" << width << "," << height << ")" << std::endl;
                    //  try pick.
                }
                else
                {
                    std::cout << "Not Moving:(" << x << "," << y << "),(" << width << "," << height << ")" << std::endl;
                }
            }
            return false;
        }

        bool keyPressed(const OgreBites::KeyboardEvent &evt)
        {
            if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
            {
                Ogre::Root::getSingleton().queueEndRendering();
            }
            if (evt.keysym.sym == OgreBites::SDLK_LEFT)
            {
                this->left = true;
            }
            if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
            {
                this->right = true;
            }
            if (evt.keysym.sym == OgreBites::SDLK_UP)
            {
                this->front = true;
            }
            if (evt.keysym.sym == OgreBites::SDLK_DOWN)
            {
                this->back = true;
            }
            return true;
        }
        bool keyReleased(const OgreBites::KeyboardEvent &evt)
        {

            if (evt.keysym.sym == OgreBites::SDLK_LEFT)
            {
                this->left = false;
            }
            if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
            {
                this->right = false;
            }
            if (evt.keysym.sym == OgreBites::SDLK_UP)
            {
                this->front = false;
            }
            if (evt.keysym.sym == OgreBites::SDLK_DOWN)
            {
                this->back = false;
            }
            return true;
        }

        bool isLeft() override
        {
            return this->left;
        }
        bool isRight() override
        {
            return this->right;
        }
        bool isFront() override
        {
            return this->front;
        }
        bool isBack() override
        {
            return this->back;
        }
    };

}; // end of namespace
