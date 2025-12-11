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
