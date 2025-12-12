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
#include "fg/ImGuiApp.h"
#include <OgreInput.h>

#include <imgui.h>
// #include "imgui/imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

namespace fog{
using namespace Ogre;
using namespace OgreBites;
class ImGuiAppImpl : public ImGuiApp, public DispatchInputListener
{
public:
    class FrameDispatch : public ImGuiApp::FrameListener
    {

        std::vector<ImGuiApp::FrameListener *> list;

    public:
        void add(ImGuiApp::FrameListener *lis)
        {
            list.push_back(lis);
        }

        void onFrame(const Ogre::FrameEvent &evt) override
        {
            for (ImGuiApp::FrameListener *l : list)
            {
                l->onFrame(evt);
            }
        }
    };
    ImGuiInputListener guiListener;
    // The dispatch is the entry listener,key point is: the first child is/must be the UI listener.
    // If a event is consumed by the first listener, it will not be delivered to the next one.
    FrameDispatch frameDispatch;
    NativeWindowPair window;
    bool glInited = false;
    ImGuiContext *igc = nullptr;
    bool breakRenderRequested = false;
public:
    ImGuiAppImpl(NativeWindowPair &window)
    {
        this->window = window;
        DispatchInputListener::addInputListener(&this->guiListener);
    }
    virtual ~ImGuiAppImpl()
    {
        if (glInited)
        {
            ImGui_ImplOpenGL3_Shutdown();
        }
        if (igc)
        {
            ImGui::DestroyContext(igc);
        }
    }

    void initApp()
    {
        // Create world state and controls.
        IMGUI_CHECKVERSION();
        igc = ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        syncWindowSize(io, window.render);
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // NativeWindowPair wp = mWindows[0];//what about other window?
        // SDL_Window * window = wp.native;
        // // // Setup Platform/Renderer backends
        // 初始化 OpenGL3 后端（OGRE 已激活 OpenGL context）
        if (ImGui_ImplOpenGL3_Init("#version 130"))
        {
            glInited = true;
        }

        // 注册自己为输入监听器
    }
    void addInputListener(InputListener *lis) override
    {
        DispatchInputListener::addInputListener(lis);
    }
    void addFrameListener(ImGuiApp::FrameListener *lis) override
    {
        frameDispatch.add(lis);
    }

    // ========== 渲染循环 ==========
    bool frameRenderingQueued(const FrameEvent &evt)
    {

        // 开始 ImGui 帧
        /*
         */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        this->frameDispatch.onFrame(evt);
        // 渲染 ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        return !breakRenderRequested;
    }

    //
    void windowResized(Ogre::RenderWindow *rw)
    {

        if (rw != window.render)
        {
            return;
        }
        ImGuiIO &io = ImGui::GetIO();
        syncWindowSize(io, rw);
    }
    void syncWindowSize(ImGuiIO &io, Ogre::RenderWindow *window)
    {

        unsigned int width, height;
        int left, top;
        window->getMetrics(width, height, left, top); // 👈 获取当前窗口尺寸

        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
    }
    void breakRender() override
    {
        breakRenderRequested = true;
    }   
};

};//end of namespace
