/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <OgreFrameListener.h>

namespace fog
{
using namespace Ogre;
using namespace OgreBites;
class ImGuiApp
{
public:
    class FrameListener
    {
    public:
        //use this event to draw UI.
        virtual void onFrame(const FrameEvent &evt) = 0;
    };
    virtual void addInputListener(InputListener *lis) = 0;
    //use this method to add the listener to draw UI.
    virtual void addFrameListener(FrameListener *l) = 0;

    virtual void breakRender() = 0;
};
};
