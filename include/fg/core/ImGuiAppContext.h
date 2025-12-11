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
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <Bites/OgreWindowEventUtilities.h>

#include <fmt/format.h>
#include "DispatchInputListener.h"
#include "ImGuiInputListener.h"
#include "ImGuiFPSCounter.h"
#include "fg/ImGuiApp.h"
#include "ImGuiAppImpl.h"
#include "fg/Video.h"
#include <functional>

#define MAO_IMGUI_BUTTON_LEFT 0
#define MAO_IMGUI_BUTTON_RIGHT 1
#define MAO_IMGUI_BUTTON_OTHER 2

namespace fog
{
    using namespace Ogre;
    using namespace OgreBites;

    class ImGuiAppContext : public ApplicationContextSDL
    {
    protected:
        ImGuiAppImpl *imGuiApp = nullptr;

        Video::Mode videoMode{1920, 1080, 32};

    public:
        std::function<void()> beforeResourceLoad;
        std::function<void()> afterResourceLoad;

    public:
        ImGuiAppContext(std::string name) : ApplicationContextSDL(name)
        {
            //
        }
        virtual ~ImGuiAppContext() override
        {
            if (this->imGuiApp)
                delete this->imGuiApp;
        }

        void createRoot() override
        {
            ApplicationContextSDL::createRoot();
        }

        void initApp()
        {
            ApplicationContextSDL::initApp();

            if (mWindows.empty())
            {
                throw new std::runtime_error("no window created?");
            }
            NativeWindowPair window = mWindows[0];
            this->imGuiApp = new ImGuiAppImpl(window);
            imGuiApp->initApp();

            // not this->addInputListener() ,us the super class to add the only one entry point listener.
            ApplicationContextSDL::addInputListener(window.native, imGuiApp);
        }
        ImGuiApp *getImGuiApp()
        {
            return this->imGuiApp;
        }
        bool frameRenderingQueued(const FrameEvent &evt) override
        {

            if (!ApplicationContextBase::frameRenderingQueued(evt))
            {
                return false; // false:break,true:continue
            } // call other listener if any.

            return imGuiApp->frameRenderingQueued(evt);
        }

        // To listen a event, use the ImGuiApp for adding listener. We deliver all input event to the imGuiApp.
        void addInputListener(NativeWindowType *window, InputListener *listener) override
        {
            // to take over the dispathing task,so do not add to the entry listener list,
            // only the dispatcher listener can be there.
            this->imGuiApp->addInputListener(listener);
        }
        void windowResized(Ogre::RenderWindow *rw)
        {
            ApplicationContextSDL::windowResized(rw);
            this->imGuiApp->windowResized(rw);
        }

        void loadResources() override
        {

            if (beforeResourceLoad)
            {
                beforeResourceLoad();
            }
            ApplicationContextBase::loadResources();
            if(afterResourceLoad){
                afterResourceLoad();
            }
        }
    };

}; // end of namespace
