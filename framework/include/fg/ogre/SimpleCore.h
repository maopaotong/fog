/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <imgui.h>
// #include "imgui/imgui_impl_sdl2.h"
// #include "imgui_impl_opengl3.h"
#include "Common.h"
#include "ImGuiAppContext.h"

#define FG_LIGHT_DIRECTION_X 300
#define FG_LIGHT_DIRECTION_Y 500
#define FG_LIGHT_DIRECTION_Z 0

namespace fog
{
    class SimpleCore : public CoreMod, public Ogre::FrameListener
    {
    private:
        Ogre::Camera *camera;
        Ogre::SceneNode *cameraNode;
        Ogre::Viewport *vp;
        ImGuiAppContext *appCtx;
        Ogre::SceneManager *sceMgr;
        Ogre::Root *root;
        std::unordered_map<std::string, std::any> userObjs;
        Ogre::MaterialManager *matMgr;
        Ogre::Light *light;
        std::vector<Stairs *> stepListeners;

    public:
        INJECT(SimpleCore(ImGuiAppContext *appCtx)) : appCtx(appCtx), CoreMod()
        {
            this->matMgr = Ogre::MaterialManager::getSingletonPtr();
            this->root = appCtx->getRoot();

            // log level
            Ogre::LogManager *lm = Ogre::LogManager::getSingletonPtr();
            Ogre::Log *log = lm->getDefaultLog();
            log->setDebugOutputEnabled(false);
            log->setLogDetail(Ogre::LL_LOW);
            //
            Ogre::RenderWindow *window = appCtx->getRenderWindow();

            sceMgr = appCtx->getRoot()->createSceneManager();

            // Register our scene with the RTSS (Required for proper lighting/shaders)
            Ogre::RTShader::ShaderGenerator *shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
            shadergen->addSceneManager(sceMgr);

            // Ogre::RTShader::RenderState* renderState = shadergen->getRenderState(Ogre::RTShader::RS_DEFAULT);
            // std::string techName = "VertexColourTech";
            // Ogre::Pass *pass=nullptr;

            // Create visualizer

            // Create navigation grid and set up example terrain

            // Sand: cost 2

            // 假设你已经有 sceneMgr 和 camera
            light = sceMgr->createLight("MyLight");
            // light->setType(Ogre::Light::LT_POINT);
            light->setType(Ogre::Light::LT_DIRECTIONAL);
            light->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0)); // 白色漫反射
            light->setSpecularColour(Ogre::ColourValue(.0, .0, .0));   // 白色镜面光
            Ogre::SceneNode *lightNode = sceMgr->getRootSceneNode()->createChildSceneNode("LightNode");
            lightNode->setPosition(0, 500, 0);
            Vector3 lightDirection = Vector3(0, 0, 0) - Vector3(FG_LIGHT_DIRECTION_X, FG_LIGHT_DIRECTION_Y, FG_LIGHT_DIRECTION_Z);
            lightDirection.normalise();
            lightNode->setDirection(lightDirection);

            lightNode->attachObject(light);
            // Create camera
            camera = sceMgr->createCamera("HexMapCamera");
            camera->setNearClipDistance(0.1f);
            camera->setFarClipDistance(0.0f);
            camera->setAutoAspectRatio(true);

            // Create camera node and set position and direction
            cameraNode = sceMgr->getRootSceneNode()->createChildSceneNode("CameraNode");
            cameraNode->setPosition(0, 500, 500); //
            cameraNode->attachObject(camera);
            cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);

            // Create viewport
            vp = window->addViewport(camera);
            vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
            this->root->addFrameListener(this);
        }
        virtual ~SimpleCore()
        {
            delete appCtx;
        }
        // HexagonalGridVisualizer

        struct Setup
        {
            Mod *operator()(Injector &injector)
            {
                injector.bindImpl<CoreMod, SimpleCore>();                
                injector.bindAllImpl<ImGuiAppContext,
                                          ImGuiAppImpl>();

                return injector.get<CoreMod>();
            };
        };

        void addCallback(Callback *callback)
        {
            {

                std::function<void()> func = appCtx->beforeResourceLoad;

                appCtx->beforeResourceLoad = [func, callback]()
                {
                    if (func)
                    {
                        func();
                    }
                    callback->beforeResourceLoad();
                };
            }
            {

                std::function<void()> func = appCtx->afterResourceLoad;

                appCtx->afterResourceLoad = [func, callback]()
                {
                    if (func)
                    {
                        func();
                    }
                    callback->afterResourceLoad();
                };
            }
        }

        // ApplicationContext *getAppContext() override { return this->appCtx; }
        Ogre::SceneNode *getRootSceneNode()
        {
            return this->sceMgr->getRootSceneNode();
        }

        Ogre::RaySceneQuery *createRayQuery(Ogre::Ray &ray)
        {
            return this->sceMgr->createRayQuery(ray);
        }
        void destroyQuery(Ogre::RaySceneQuery *q)
        {
            this->sceMgr->destroyQuery(q);
        }
        Ogre::MaterialPtr createMaterial(std::string name, std::string group) override
        {
            return this->matMgr->create(name, group);
        }
        Ogre::ManualObject *createManualObject()
        {
            return sceMgr->createManualObject();
        }

        Ogre::Entity *createEntity(std::string mesh)
        {
            return sceMgr->createEntity(mesh);
        }

        // Viewport *getViewport() override { return this->vp; }
        // Camera *getCamera() override { return this->camera; }
        Ogre::Radian getCameraFOVy() override
        {
            return camera->getFOVy();
        }
        Ogre::SceneNode *getCameraSceneNode()
        {
            return camera->getParentSceneNode();
        }
        Ogre::Ray getCameraToViewportRay(float x, float y)
        {
            return this->camera->getCameraToViewportRay(x, y);
        }

        // Root *getRoot() override { return this->root; };

        Ogre::Light *getLight()
        {
            return this->light;
        }
        ImGuiApp *getImGuiApp() override
        {
            return this->appCtx->getImGuiApp();
        }
        // RenderWindow *getWindow()
        // {
        //     return this->appCtx->getRenderWindow();
        // }
        Box2<int> getWindowBox()
        {
            return Box2<int>(this->appCtx->getRenderWindow()->getWidth(), this->appCtx->getRenderWindow()->getHeight());
        }
        Ogre::MaterialManager *getMaterialManager() override
        {
            return this->matMgr;
        }
        Ogre::RenderTarget::FrameStats getWindowStatistics() override
        {
            return this->appCtx->getRenderWindow()->getStatistics();
        }
        void addStepListener(Stairs *listener) override
        {
            this->stepListeners.push_back(listener);
        }
        void addInputListener(OgreBites::InputListener *listener) override
        {
            this->appCtx->getImGuiApp()->addInputListener(listener);
        }

        void addFrameListener(Ogre::FrameListener *listener) override
        {

            this->root->addFrameListener(listener);
        }

        void setUserObject(const std::string key, std::any value) override
        {
            this->userObjs[key] = value;
        }

        bool getUserObject(const std::string key, std::any &value) override
        {
            std::unordered_map<std::string, std::any>::iterator it = userObjs.find(key);
            if (it != userObjs.end())
            {
                value = it->second;
                return true;
            }
            return false;
        }

        Box2<float> getViewportBox() override
        {
            return Box2<float>(this->vp->getLeft(), this->vp->getTop(), this->vp->getLeft() + vp->getWidth(), this->vp->getTop() + vp->getHeight());
        }

        Box2<float> getActualViewportBox() override
        {
            return Box2<float>(this->vp->getActualLeft(), this->vp->getActualTop(), this->vp->getActualLeft() + vp->getActualWidth(), this->vp->getActualTop() + vp->getActualHeight());
        }

        bool frameStarted(const Ogre::FrameEvent &evt)
        {
            for (Stairs *listener : this->stepListeners)
            {
                listener->step(evt.timeSinceLastFrame);
            }
            return true;
        }
        void startRendering() override
        {
            this->root->startRendering();
        }
    };
}; // end of namespace
