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
#include "fg/util/CellUtil.h"
#include "fg/core/CoreMod.h"
#include "fg/core/MoveToCellTask.h"
#include "fg/CellInstanceManager.h"
#include "fg/PathingStateManager.h"
#include "fg/demo/InputStateController.h"
#include "fg/MovingStateManager.h"
#include "fg/BuildingStateManager.h"
namespace fog
{
    using namespace OgreBites;
    using namespace Ogre;
    // === Custom hash function ===
    //
    // === Input handler for closing application ===

    class EntryController : public OgreBites::InputListener, public FrameListener
    {

    public:
        INJECT(EntryController())
        {
        }
        bool mouseWheelRolled(const MouseWheelEvent &evt)
        {
            Context<CameraState>::get()->mouseWheelRolled(evt);
            return false;
        }
        bool mousePressed(const MouseButtonEvent &evt) override
        {
            if (evt.button == ButtonType::BUTTON_LEFT)
            {
                return mouseButtonLeftPressed(evt);
            }
            else if (evt.button == ButtonType::BUTTON_RIGHT)
            {
                Context<MovingStateManager>::get()->movingActiveStateToCellByMousePosition(evt.x, evt.y);
            }
            return false;
        }

        bool mouseButtonLeftPressed(const MouseButtonEvent &evt)
        {
            // normalized (0,1)
            Viewport *viewport = Context<CoreMod>::get()->getViewport();
            Camera *camera = Context<CoreMod>::get()->getCamera();
            float ndcX = evt.x / (float)viewport->getActualWidth();
            float ndcY = evt.y / (float)viewport->getActualHeight();
            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            if (Context<MovableStateManager>::get()->pick(ray))
            {
                return true;
            }
            if (Context<BuildingStateManager>::get()->pick(ray))
            {
                return true;
            }

            return true;
        }

        CONSUMED mouseMoved(const MouseMotionEvent &evt) override
        {
            Context<PathingStateManager>::get()->onMouseMoved(evt.x, evt.y);
            Context<InputStateController>::get()->mouseMoved(evt);
            return false;
        }

        bool keyPressed(const OgreBites::KeyboardEvent &evt) override
        {
            Context<InputStateController>::get()->keyPressed(evt);
            return true;
        }
        bool keyReleased(const OgreBites::KeyboardEvent &evt) override
        {
            Context<InputStateController>::get()->keyReleased(evt);
            return true;
        }
        GOON frameStarted(const FrameEvent &evt) override
        {
            Context<PathingStateManager>::get()->step(evt.timeSinceLastFrame);
            Context<MovableStateManager>::get()->step(evt.timeSinceLastFrame);
            Context<CameraState>::get()->step(evt.timeSinceLastFrame);
            return true;
        }
    };
}; // end of namespace
