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

#include "fg/util.h"
#include "fg/ogre.h"
#include "InputState.h"
#include "InputStateController.h"
#include "ShaderManager.h"

namespace fog
{

    // === Frame Listener class for main loop ===
    class CameraStateManager : public Stairs
    {

    private:
        bool quit;
        CoreMod *core;
        InputStateController *inputState;
        Config *config;
        Transforms *tfs;
        Geometry *geo;
        ShaderManager *shaderManager;

        float speed = 0;
        float maxAcc = 3000;

    public:
        INJECT(CameraStateManager(CoreMod *core, InputStateController *inputState, Transforms *tfs, Geometry *geo, ShaderManager *shaderManager)) : geo(geo), shaderManager(shaderManager), quit(false), core(core), inputState(inputState),
                                                                                                                      tfs(tfs)
        {
        }

        // position and orientation of the camera
        bool isViewportInsideGround(Vector3 &position, Ogre::Quaternion &orientation)
        {

            // Ray ray = state->camera->getCameraToViewportRay(0.5f, 0.5f);

            Ogre::Vector3 direction = orientation * Ogre::Vector3::NEGATIVE_UNIT_Z;
            Ogre::Ray ray(position, direction);
            // TODO intersects with ground in one piece of code.
            auto hitGrd = ray.intersects(geo->ground);
            if (!hitGrd.first)
            {
                return false;
            }
            Vector3 viewCenterOnGround = ray.getPoint(hitGrd.second);

            auto p2 = Point2<float>::from(viewCenterOnGround, *tfs->d2td3);
            CellKey::OffsetPointy cell = CellsTransform::transform<CellsTransform::CO>(p2);
            // if (Context<Cell::Center>::get()->findCellByPosition(p2, cell))
            // {
            //     return true;
            // }

            return true; // TODO false?
        }

        static float map(float fValue, float minValue, float maxValue, float outMin, float outMax)
        {

            return (fValue - minValue) / (maxValue - minValue) * (outMax - outMin) + outMin;
        }

        bool step(float timeSinceLastFrame) override
        {
            // std::cout << "Frame started!\n";
            if (!inputState->isMoving())
            {
                this->speed = 0;
                return true;
            }

            // Camera *camera = core->getCamera();
            // Move camera
            Ogre::SceneNode *node = core->getCameraSceneNode();
            // 获取当前朝向（四元数）
            // Ogre::Quaternion orientation = node->getOrientation();

            // 计算右向量（X轴）
            Vector3 right = Ogre::Vector3::UNIT_X;
            Vector3 back = Ogre::Vector3::UNIT_Z;
            float height = node->getPosition().y;

            float speed = map(height, DEFAULT_CAMERA_HEIGHT_MIN, DEFAULT_CAMERA_HEIGHT_MAX, FOG_CAM_SPEED_MIN, FOG_CAM_SPEED_MAX);
            float deltaSpeed = speed - this->speed;
            float maxDeltaSpeed = maxAcc * timeSinceLastFrame;
            if (deltaSpeed > maxDeltaSpeed)
            {
                deltaSpeed = maxDeltaSpeed;
            }
            else if (deltaSpeed < -maxDeltaSpeed)
            {
                deltaSpeed = -maxDeltaSpeed;
            }
            speed = this->speed + deltaSpeed;
            this->speed = speed;
            //

            Vector3 position = node->getPosition();
            Vector3 step = Ogre::Vector3::ZERO;

            if (inputState->isFront())
            {
                // node->translate(-back * speed * evt.timeSinceLastFrame);
                step += -back * speed * timeSinceLastFrame;
            }
            if (inputState->isBack())
            {
                step += (back * speed * timeSinceLastFrame);
            }
            if (inputState->isLeft())
            {
                step += (-right * speed * timeSinceLastFrame);
            }
            if (inputState->isRight())
            {
                step += (right * speed * timeSinceLastFrame);
            }

            Vector3 position2 = position + step;
            bool validTranlate = true;

            Ogre::Quaternion orientation = node->getOrientation();
            validTranlate = this->isViewportInsideGround(position2, orientation);

            if (validTranlate)
            {
                node->translate(step);

                shaderManager->setCameraPos(node->getPosition());
            }

            return true; // Continue rendering
        }
        /**
         * Move and rotate the camera according to the height of the camera.
         * Lower position with a heigher pitch : look forward.
         * Heighest position let camera to look more like downward.
         */
        bool mouseWheelRolled(int evtY)
        {
            // Camera *cam = core->getCamera();
            Ogre::SceneNode *node = core->getCameraSceneNode(); // cam->getParentSceneNode();

            float height = node->getPosition().y;

            float cameraRollSpeed = map(height, DEFAULT_CAMERA_HEIGHT_MIN, DEFAULT_CAMERA_HEIGHT_MAX, DEFAULT_CAMERA_ROLL_SPEED_MIN, DEFAULT_CAMERA_ROLL_SPEED_MAX);

            Vector3 translate = Ogre::Vector3::NEGATIVE_UNIT_Y * evtY * cameraRollSpeed;

            Vector3 posTarget = node->getPosition() + translate;
            if (posTarget.y < DEFAULT_CAMERA_HEIGHT_MIN)
            {
                posTarget.y = DEFAULT_CAMERA_HEIGHT_MIN;
            }

            node->setPosition(posTarget);

            // Context<Var<Vector3>::Bag>::get()->setVar(".camera.position", posTarget);

            float distance = map(height, DEFAULT_CAMERA_HEIGHT_MIN, DEFAULT_CAMERA_HEIGHT_MAX, CAMERA_FAR_DISTANCE_MIN, CAMERA_FAR_DISTANCE_MAX);
            // if (distance < posTarget.y)
            // {
            //     distance = posTarget.y + 10;
            // }
            alignHorizonToTop(node, distance);
            shaderManager->setCameraPos(node->getPosition());
            return false;
        }
        /**
         * Adjust the pitch according to the new height of camera.
         */
        void alignHorizonToTop(Ogre::SceneNode *camNode, Ogre::Real distance)
        {
            Ogre::Radian fovY = core->getCameraFOVy(); // cam->getFOVy();

            Ogre::Real camHeight = camNode->getPosition().y; // 假设地面 Y=0

            // 防止高度 <= 0
            if (camHeight <= 0.1f)
                camHeight = 0.1f;

            // 计算相机到目标点的俯角（从水平线向下）
            Ogre::Radian depressionAngle = Ogre::Math::ATan(camHeight / distance);
            // 目标 pitch：向下转 (depressionAngle + fovY/2)
            Ogre::Radian targetPitch = -(depressionAngle + fovY / 2);

            const Ogre::Radian PITCH_LIMIT = Ogre::Degree(89.9);
            targetPitch = Ogre::Math::Clamp(targetPitch, -PITCH_LIMIT, PITCH_LIMIT);

            // 保持当前 yaw
            Ogre::Radian currentYaw = camNode->getOrientation().getYaw();

            // 设置新朝向
            Ogre::Quaternion newOri =
                Ogre::Quaternion(currentYaw, Ogre::Vector3::UNIT_Y) *
                Ogre::Quaternion(targetPitch, Ogre::Vector3::UNIT_X);

            camNode->setOrientation(newOri);            
        }
    };
}; // end of namespace
