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
#include <imgui.h>
// #include "imgui/imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <fmt/format.h>
#include <OgreInput.h>
#include <iostream>

#define MAO_IMGUI_BUTTON_LEFT 0
#define MAO_IMGUI_BUTTON_RIGHT 1
#define MAO_IMGUI_BUTTON_OTHER 2

namespace fog{
using namespace Ogre;
using namespace OgreBites;

class ImGuiInputListener : public InputListener
{

public:
    // ========== 输入事件转发给 ImGui ==========
    bool keyPressed(const KeyboardEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        if (evt.keysym.sym >= 0 && evt.keysym.sym < 256)
            io.AddKeyEvent(ImGuiKey(evt.keysym.sym), true);
        bool ret = io.WantCaptureKeyboard;
        return ret;
    }

    bool keyReleased(const KeyboardEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        if (evt.keysym.sym >= 0 && evt.keysym.sym < 256)
            io.AddKeyEvent(ImGuiKey(evt.keysym.sym), false);
        bool ret = io.WantCaptureKeyboard;

        // log(fmt::format("io.WantCapterKeyboard is {}", ret));
        return ret;
    }

    void log(std::string msg)
    {
        std::cout << msg << std::endl;
    }

    bool mouseMoved(const MouseMotionEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        io.AddMousePosEvent((float)evt.x, (float)evt.y);
        bool ret = io.WantCaptureMouse;
        return ret;
    }

    bool mousePressed(const MouseButtonEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        int button = evt.button == ButtonType::BUTTON_LEFT ? MAO_IMGUI_BUTTON_LEFT : (evt.button == ButtonType::BUTTON_RIGHT ? MAO_IMGUI_BUTTON_RIGHT : MAO_IMGUI_BUTTON_OTHER);
        io.AddMouseButtonEvent(button, true);
        bool ret = io.WantCaptureMouse;
        // log(fmt::format("io.WantCaptureMouse is {}", ret));
        return ret;
    }

    bool mouseReleased(const MouseButtonEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        int button = evt.button == ButtonType::BUTTON_LEFT ? MAO_IMGUI_BUTTON_LEFT : (evt.button == ButtonType::BUTTON_RIGHT ? MAO_IMGUI_BUTTON_RIGHT : MAO_IMGUI_BUTTON_OTHER);
        io.AddMouseButtonEvent(button, false);

        bool ret = io.WantCaptureMouse;
        // log(fmt::format("io.WantCaptureMouse is {}", ret));
        return ret;
    }
};

};//end of namespace
