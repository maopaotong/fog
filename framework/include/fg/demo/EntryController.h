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
#include "fg/ogre.h"
#include "MoveToCellTask.h"
#include "CellInstanceStateManager.h"
#include "PathingStateManager.h"
#include "InputStateController.h"
#include "MovingStateManager.h"
#include "BuildingStateManager.h"
#include "CameraState.h"
namespace fog
{
    using namespace OgreBites;
    using namespace Ogre;
    // === Custom hash function ===
    //
    // === Input handler for closing application ===

    class EntryController : public OgreBites::InputListener, public FrameListener
    {
        PathingStateManager *pathingStateManager;
        MovingStateManager *movingStateManager;
        InputStateController *inputStateController;
        MovableStateManager * movableStateManager;
        BuildingStateManager * buildingStateManager;
        Viewport *viewport;
        Camera *camera;
        CameraState *cameraState;

    public:
        INJECT(EntryController(PathingStateManager *pathingStateManager, MovingStateManager *movingStateManager,
                               InputStateController *inputStateController,
                               MovableStateManager * movableStateManager,
                               BuildingStateManager * buildingStateManager,
                               CameraState *cameraState,
                               Viewport *viewport, Camera *camera))
            : viewport(viewport),
              camera(camera),
              pathingStateManager(pathingStateManager), 
              buildingStateManager(buildingStateManager),
              movingStateManager(movingStateManager),
              movableStateManager(movableStateManager),
              cameraState (cameraState),
              inputStateController(inputStateController)
        {
        }
        bool mouseWheelRolled(const MouseWheelEvent &evt)
        {
            cameraState->mouseWheelRolled(evt);
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
                // Context<MovingStateManager>::get()
                movingStateManager->movingActiveStateToCellByMousePosition(evt.x, evt.y);
            }
            return false;
        }

        bool mouseButtonLeftPressed(const MouseButtonEvent &evt)
        {
            // normalized (0,1)
            // Viewport *viewport = Context<CoreMod>::get()->getViewport();
            // Camera *camera = Context<CoreMod>::get()->getCamera();
            float ndcX = evt.x / (float)viewport->getActualWidth();
            float ndcY = evt.y / (float)viewport->getActualHeight();
            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            if (movableStateManager->pick(ray))
            {
                return true;
            }
            if (buildingStateManager->pick(ray))
            {
                return true;
            }

            return true;
        }

        CONSUMED mouseMoved(const MouseMotionEvent &evt) override
        {
            pathingStateManager->onMouseMoved(evt.x, evt.y);
            inputStateController->mouseMoved(evt);
            return false;
        }

        bool keyPressed(const OgreBites::KeyboardEvent &evt) override
        {
            inputStateController->keyPressed(evt);
            return true;
        }
        bool keyReleased(const OgreBites::KeyboardEvent &evt) override
        {
            inputStateController->keyReleased(evt);
            return true;
        }
        GOON frameStarted(const FrameEvent &evt) override
        {
            pathingStateManager->step(evt.timeSinceLastFrame);
            movableStateManager->step(evt.timeSinceLastFrame);
            cameraState->step(evt.timeSinceLastFrame);
            return true;
        }
    };
}; // end of namespace
