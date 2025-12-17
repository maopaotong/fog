/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <imgui.h>
// #include "imgui/imgui_impl_sdl2.h"
// #include "imgui_impl_opengl3.h"
#include "ImGuiAppContext.h"
#include "fg/util/HexGridPrinter.h"
#include "fg/Mod.h"
#include <unordered_map>
#define FG_LIGHT_DIRECTION_X 300
#define FG_LIGHT_DIRECTION_Y 500
#define FG_LIGHT_DIRECTION_Z 0

namespace fog
{
    using namespace OgreBites;
    using namespace Ogre;

    class SimpleCore : public CoreMod, public FrameListener
    {
    private:
        Ogre::Camera *camera;
        Ogre::SceneNode *cameraNode;
        Ogre::Viewport *vp;
        ImGuiAppContext *appCtx;
        Ogre::SceneManager *sceMgr;
        Ogre::Root *root;
        std::unordered_map<std::string, std::any> userObjs;
        MaterialManager *matMgr;
        Ogre::Light *light;
        std::vector<Stairs *> stepListeners;

    public:
        INJECT(SimpleCore(ImGuiAppContext *igac)) : appCtx(igac), CoreMod()
        {
            appCtx->initApp();
            this->matMgr = MaterialManager::getSingletonPtr();
            this->root = appCtx->getRoot();

            // log level
            LogManager *lm = LogManager::getSingletonPtr();
            Log *log = lm->getDefaultLog();
            log->setDebugOutputEnabled(false);
            log->setLogDetail(Ogre::LL_LOW);
            //
            RenderWindow *window = appCtx->getRenderWindow();

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
            Mod *operator()(Component::Injector &injector)
            {
                injector.bindImpl<CoreMod, SimpleCore>([](CoreMod &core)
                                                       {
                                                           // core.init();
                                                       });
                injector.bindAllImplAsVal<>();
                injector.bindAllImplAsPtr<ImGuiAppContext,
                                     ImGuiAppImpl>();

                return injector.getPtr<CoreMod>();
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

        ApplicationContext *getAppContext() override { return this->appCtx; }
        SceneManager *getSceneManager() override { return this->sceMgr; }
        Viewport *getViewport() override { return this->vp; }
        Camera *getCamera() override { return this->camera; }
        Root *getRoot() override { return this->root; };

        Light *getLight()
        {
            return this->light;
        }
        ImGuiApp *getImGuiApp() override
        {
            return this->appCtx->getImGuiApp();
        }
        RenderWindow *getWindow()
        {
            return this->appCtx->getRenderWindow();
        }
        MaterialManager *getMaterialManager() override
        {
            return this->matMgr;
        }
        void addStepListener(Stairs *listener) override
        {
            this->stepListeners.push_back(listener);
        }
        void addInputListener(InputListener *listener) override
        {
            this->appCtx->getImGuiApp()->addInputListener(listener);
        }

        void addFrameListener(FrameListener *listener) override
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

        std::string getName()
        {
            return "core";
        }

        void deactive() override
        {
            // Context<CoreMod>::set(nullptr);
        }

        bool frameStarted(const FrameEvent &evt)
        {
            for (Stairs *listener : this->stepListeners)
            {
                listener->step(evt.timeSinceLastFrame);
            }
            return true;
        }
    };
}; // end of namespace
