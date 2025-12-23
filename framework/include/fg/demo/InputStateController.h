/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "InputState.h"
#include "Common.h"
#include <chrono>
namespace fog
{
    // === Custom hash function ===
    //
    // === Input handler for closing application ===
    class InputStateController : public InputState
    {
        using Clock = std::chrono::steady_clock;
        using TimePoint = std::chrono::time_point<Clock>;

    private:
        bool left = false;
        bool right = false;
        bool front = false;
        bool back = false;
        TimePoint lastMouseMoveEventTime;
        CoreMod *core;
        Config *config;
        int mouseX;
        int mouseY;

        int windowWidth;
        int windowHeight;

    public:
        INJECT(InputStateController(CoreMod *core, Config *config)) : core(core), config(config)
        {
        }

        bool mouseMoved(const OgreBites::MouseMotionEvent &evt)
        {
            this->lastMouseMoveEventTime = Clock::now();
            Box2<int> window = core->getWindowBox();

            windowWidth = window.getWidth();
            windowHeight = window.getHeight();

            mouseX = evt.x;
            mouseY = evt.y;

            if (config->debugCout)
            {

                // if (this->isMoving())
                // {
                //     std::cout << "Moving:(" << x << "," << y << "),(" << width << "," << height << ")" << std::endl;
                //     //  try pick.
                // }
                // else
                // {
                //     std::cout << "Not Moving:(" << x << "," << y << "),(" << width << "," << height << ")" << std::endl;
                // }
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

        bool isMoving() override
        {
            auto now = Clock::now();
            auto elapsed = std::chrono::duration<float>(now - lastMouseMoveEventTime).count();
            if (elapsed > 3)
            {
                return false;
            }
            return InputState::isMoving();
        }

        bool isLeft() override
        {
            if (this->left)
            {
                return true;
            }
            if (mouseX > 0 && mouseX < 10)
            {
                return true;
            }
            if (mouseX == 0)
            {
                return true;
            }

            return false;
        }
        bool isRight() override
        {
            if (this->right)
            {
                return this->right;
            }
            if (mouseX > windowWidth - 10 && mouseX < windowWidth - 1)
            {
                return true;
            }
            if (mouseX == windowWidth - 1)
            {
                return true;
            }
            return false;
        }

        bool isFront() override
        {
            if (this->front)
            {
                return true;
            }
            if (mouseY > 0 && mouseY < 10)
            {
                return true;
            }
            if (mouseY == 0)
            {
                return true;
            }
            return false;
        }
        bool isBack() override
        {
            if (this->back)
            {
                return true;
            }
            if (mouseY > windowHeight - 10 && mouseY < windowHeight - 1)
            {
                return true;
            }
            if (mouseY == windowHeight - 1)
            {
                return true;
            }

            return false;
        }
    };

}; // end of namespace
