/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
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
//using namespace Ogre;
//using namespace OgreBites;

class ImGuiInputListener : public OgreBites::InputListener
{

public:
    // ========== 输入事件转发给 ImGui ==========
    bool keyPressed(const OgreBites::KeyboardEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        if (evt.keysym.sym >= 0 && evt.keysym.sym < 256)
            io.AddKeyEvent(ImGuiKey(evt.keysym.sym), true);
        bool ret = io.WantCaptureKeyboard;
        return ret;
    }

    bool keyReleased(const OgreBites::KeyboardEvent &evt) override
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

    bool mouseMoved(const  OgreBites::MouseMotionEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        io.AddMousePosEvent((float)evt.x, (float)evt.y);
        bool ret = io.WantCaptureMouse;
        return ret;
    }

    bool mousePressed(const OgreBites::MouseButtonEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        int button = evt.button == OgreBites::ButtonType::BUTTON_LEFT ? MAO_IMGUI_BUTTON_LEFT : (evt.button == OgreBites::ButtonType::BUTTON_RIGHT ? MAO_IMGUI_BUTTON_RIGHT : MAO_IMGUI_BUTTON_OTHER);
        io.AddMouseButtonEvent(button, true);
        bool ret = io.WantCaptureMouse;
        // log(fmt::format("io.WantCaptureMouse is {}", ret));
        return ret;
    }

    bool mouseReleased(const OgreBites::MouseButtonEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        int button = evt.button == OgreBites::ButtonType::BUTTON_LEFT ? MAO_IMGUI_BUTTON_LEFT : (evt.button == OgreBites::ButtonType::BUTTON_RIGHT ? MAO_IMGUI_BUTTON_RIGHT : MAO_IMGUI_BUTTON_OTHER);
        io.AddMouseButtonEvent(button, false);

        bool ret = io.WantCaptureMouse;
        // log(fmt::format("io.WantCaptureMouse is {}", ret));
        return ret;
    }
};

};//end of namespace
