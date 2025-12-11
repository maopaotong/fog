/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
#pragma once
#include <OgreInput.h>
#include <OgreApplicationContext.h>
#include <any>
#include "ImGuiApp.h"
#include "State.h"
#include "fg/util.h"
#include "fg/Mod.h"

using namespace Ogre;
using namespace OgreBites;
namespace fog
{

    class CoreMod : public Mod
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
        virtual ApplicationContext *getAppContext() = 0;
        virtual ImGuiApp *getImGuiApp() = 0;
        virtual SceneManager *getSceneManager() = 0;
        virtual Viewport *getViewport() = 0;
        virtual Camera *getCamera() = 0;
        virtual Root *getRoot() = 0;
        virtual RenderWindow *getWindow() = 0;

        virtual void addStepListener(Stairs *listener) = 0;

        virtual void addInputListener(InputListener *listener) = 0;

        virtual void addFrameListener(Ogre::FrameListener *listener) = 0;
        virtual MaterialManager *getMaterialManager() = 0;

        virtual void setUserObject(const std::string key, std::any value) = 0;
        virtual bool getUserObject(const std::string key, std::any &value) = 0;
        virtual Light *getLight() = 0;

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
    };
};
