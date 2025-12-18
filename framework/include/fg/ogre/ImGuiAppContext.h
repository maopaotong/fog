/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <Bites/OgreWindowEventUtilities.h>

#include <fmt/format.h>

#include "DispatchInputListener.h"
#include "ImGuiInputListener.h"
#include "ImGuiFPSCounter.h"
#include "ImGuiAppImpl.h"
#include "fg/util.h"
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
        struct Options
        {
            std::string name;
        };

    public:
        INJECT(ImGuiAppContext(Options opts, ImGuiAppImpl *imGuiApp)) : imGuiApp(imGuiApp), ApplicationContextSDL(opts.name)
        {
            ApplicationContextSDL::initApp();

            if (mWindows.empty())
            {
                throw new std::runtime_error("no window created?");
            }
            NativeWindowPair window = mWindows[0];
            imGuiApp->initApp(window);

            // not this->addInputListener() ,us the super class to add the only one entry point listener.
            ApplicationContextSDL::addInputListener(window.native, imGuiApp);
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
            if (afterResourceLoad)
            {
                afterResourceLoad();
            }
        }
    };

}; // end of namespace
