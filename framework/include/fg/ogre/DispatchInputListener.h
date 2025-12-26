/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <OgreInput.h>

namespace fog
{
    class DispatchInputListener : public OgreBites::InputListener
    {

        std::vector<OgreBites::InputListener *> list;

    public:
        void addInputListener(OgreBites::InputListener *lis)
        {
            list.push_back(lis);
        }

        void frameRendered(const Ogre::FrameEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                l->frameRendered(evt);
            }
        }
        virtual bool keyPressed(const OgreBites::KeyboardEvent &evt)
        {

            for (OgreBites::InputListener *l : list)
            {
                if (l->keyPressed(evt))
                {
                    return true;
                }
            }
            return false;
        }

        bool keyReleased(const OgreBites::KeyboardEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->keyReleased(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool touchMoved(const OgreBites::TouchFingerEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->touchMoved(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool touchPressed(const OgreBites::TouchFingerEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->touchPressed(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool touchReleased(const OgreBites::TouchFingerEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->touchReleased(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool mouseMoved(const OgreBites::MouseMotionEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->mouseMoved(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool mouseWheelRolled(const OgreBites::MouseWheelEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->mouseWheelRolled(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool mousePressed(const OgreBites::MouseButtonEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->mousePressed(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool mouseReleased(const OgreBites::MouseButtonEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->mouseReleased(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool textInput(const OgreBites::TextInputEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->textInput(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool axisMoved(const OgreBites::AxisEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->axisMoved(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool buttonPressed(const OgreBites::ButtonEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->buttonPressed(evt))
                {
                    return true;
                }
            }
            return false;
        }
        bool buttonReleased(const OgreBites::ButtonEvent &evt) override
        {
            for (OgreBites::InputListener *l : list)
            {
                if (l->buttonReleased(evt))
                {
                    return true;
                }
            }
            return false;
        }
    };

}; // end of namespace
