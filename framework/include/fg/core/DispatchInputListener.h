/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <OgreInput.h>

namespace fog{
using namespace Ogre;
using namespace OgreBites;
class DispatchInputListener : public InputListener
{

    std::vector<InputListener *> list;

public:
    void addInputListener(InputListener *lis)
    {
        list.push_back(lis);
    }

    void frameRendered(const Ogre::FrameEvent &evt) override
    {
        for (InputListener *l : list)
        {
            l->frameRendered(evt);
        }
    }
    virtual bool keyPressed(const KeyboardEvent &evt)
    {

        for (InputListener *l : list)
        {
            if (l->keyPressed(evt))
            {
                return true;
            }
        }
        return false;
    }

    bool keyReleased(const KeyboardEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->keyReleased(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool touchMoved(const TouchFingerEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->touchMoved(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool touchPressed(const TouchFingerEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->touchPressed(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool touchReleased(const TouchFingerEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->touchReleased(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool mouseMoved(const MouseMotionEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->mouseMoved(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool mouseWheelRolled(const MouseWheelEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->mouseWheelRolled(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool mousePressed(const MouseButtonEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->mousePressed(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool mouseReleased(const MouseButtonEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->mouseReleased(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool textInput(const TextInputEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->textInput(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool axisMoved(const AxisEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->axisMoved(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool buttonPressed(const ButtonEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->buttonPressed(evt))
            {
                return true;
            }
        }
        return false;
    }
    bool buttonReleased(const ButtonEvent &evt) override
    {
        for (InputListener *l : list)
        {
            if (l->buttonReleased(evt))
            {
                return true;
            }
        }
        return false;
    }
};

};//end of namespace
