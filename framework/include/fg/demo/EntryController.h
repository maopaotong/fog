/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once
#include "Common.h"
#include "fg/util.h"
#include "fg/ogre.h"
#include "MoveToCellTask.h"
#include "CellInstanceStateManager.h"
#include "PathingStateManager.h"
#include "InputStateController.h"
#include "MovingStateManager.h"
#include "BuildingStateManager.h"
#include "CameraStateManager.h"
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
        CameraStateManager *cameraState;
        CoreMod * core;
    public:
        INJECT(EntryController(PathingStateManager *pathingStateManager, MovingStateManager *movingStateManager,
                               InputStateController *inputStateController,
                               MovableStateManager * movableStateManager,
                               BuildingStateManager * buildingStateManager,
                               CameraStateManager *cameraState,
                               CoreMod * core
                               ))
            :              
              core(core),
              pathingStateManager(pathingStateManager), 
              buildingStateManager(buildingStateManager),
              movingStateManager(movingStateManager),
              movableStateManager(movableStateManager),
              cameraState (cameraState),
              inputStateController(inputStateController)
        {
            //core->getAppContext()->addInputListener(this);
            core->getImGuiApp()->addInputListener(this);
            // core->getRoot()->addFrameListener(this);
            core->addFrameListener(this);
        
        }
        bool mouseWheelRolled(const MouseWheelEvent &evt)
        {
            cameraState->mouseWheelRolled(evt.y);
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
            Box2<float> viewport = core->getActualViewportBox();

            float ndcX = evt.x / (float)viewport.getWidth();
            float ndcY = evt.y / (float)viewport.getHeight();
            Ogre::Ray ray = core->getCameraToViewportRay(ndcX, ndcY);

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
