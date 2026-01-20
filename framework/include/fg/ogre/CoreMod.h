/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "Common.h"
#include "ImGuiApp.h"
#include "fg.h"
#include "fg/util.h"

namespace fog
{

    class CoreMod
    {

    public:
        class Callback
        {
        public:
            virtual void beforeResourceLoad() = 0;
            virtual void afterResourceLoad() = 0;
        };

    public:
        CoreMod()
        {
        }
        virtual ~CoreMod() {}
        // virtual ApplicationContext *getAppContext() = 0;
        virtual ImGuiApp *getImGuiApp() = 0;
        virtual Ogre::SceneNode *getRootSceneNode() = 0;
        virtual Ogre::RaySceneQuery *createRayQuery(Ogre::Ray &ray) = 0;
        virtual void destroyQuery(Ogre::RaySceneQuery *) = 0;

        // virtual Viewport *getViewport() = 0;
        virtual Box2<float> getViewportBox() = 0;
        virtual Box2<float> getActualViewportBox() = 0;
        virtual Ogre::ManualObject *createManualObject() = 0;
        virtual Ogre::Entity *createEntity(std::string mesh) = 0;

        // virtual Camera *getCamera() = 0;
        virtual Ogre::Ray getCameraToViewportRay(float x, float y) = 0;
        virtual Ogre::SceneNode *getCameraSceneNode() = 0;
        virtual Ogre::Radian getCameraFOVy() = 0;
        virtual Ogre::MaterialPtr createMaterial(std::string name, std::string group) = 0;

        // virtual Root *getRoot() = 0;
        // virtual RenderWindow *getWindow() = 0;
        virtual Box2<int> getWindowBox() = 0;
        virtual Ogre::RenderTarget::FrameStats getWindowStatistics() = 0;

        virtual void addStepListener(Stairs *listener) = 0;

        virtual void addInputListener(OgreBites::InputListener *listener) = 0;

        virtual void addFrameListener(Ogre::FrameListener *listener) = 0;
        virtual Ogre::MaterialManager *getMaterialManager() = 0;

        virtual void setUserObject(const std::string key, std::any value) = 0;
        virtual bool getUserObject(const std::string key, std::any &value) = 0;
        virtual Ogre::Light *getLight() = 0;
        virtual Ogre::Viewport *getViewport() = 0;
        virtual Ogre::Affine3 getCamerayViewMatrix(bool bl) = 0;
        virtual Ogre::Matrix4 getCamerayProjMatrix() = 0;
        virtual Ogre::Camera * getCamera() = 0;       

        template <typename T>
        void setUserObject(const std::string key, T *obj)
        {
            this->setUserObject(key, std::any(obj));
        }

        template <typename T>
        T *getUserObject(const std::string key)
        {

            return getUserObject<T>(key, false);
        }

        template <typename T>
        T *getUserObject(const std::string key, bool required)
        {
            std::any value;
            T *rt = nullptr;
            if (this->getUserObject(key, value))
            {
                T **pp = any_cast<T *>(&value);
                if (pp != nullptr)
                {
                    rt = *pp;
                }
            }

            if (rt == nullptr && required)
            {
                throw std::runtime_error("No such user object with key=" + key + " or type of the value mis-match.");
            }
            return rt;
        }
        virtual void addCallback(Callback *callback) = 0;
        virtual void startRendering() = 0;
    };
};
