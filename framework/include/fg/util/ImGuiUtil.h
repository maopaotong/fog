/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <imgui.h>
#include <OgreVector.h>
#include <fmt/format.h>

namespace fog
{
    //using namespace Ogre;

    class ImGuiUtil
    {
    public:
        static void Text(const Vector3 &pos)
        {
            ImGui::Text(fmt::format("({:.1f},{:.1f},{:.1f})", pos.x, pos.y, pos.z).c_str());
        };

        template <typename... Args>
        static void TextFormat(const std::string str, Args... args)
        {
            ImGui::Text(fmt::format(str, args).c_str());
        };

        template <typename... Args>
        static void Button(const std::string &title, void (*clickFunc)(Args...), Args... args)
        {
            if (ImGui::Button(title.c_str()))
            {
                clickFunc(args);
            }
        }

        template <typename... Args>
        static void Checkbox(const std::string &title, bool vPtr, void (*clickFunc)(bool), Args... args)
        {
            if (ImGui::Checkbox(title.c_str()))
            {
                clickFunc(args);
            }
        }

        static bool MyToggleButton(const char *label, bool *v)
        {
            ImVec4 green = ImVec4(0.2f, 0.6f, 0.2f, 1.0f);
            if (*v)
            {

                ImGui::PushStyleColor(ImGuiCol_Button, green);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, green);
            }
            bool clicked = ImGui::Button(label);
            if (*v)
            {

                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            return clicked;
        }

        template<typename F>
        static bool BeginIfEnd(std::string name, bool * open, F&& func){
            bool ret = ImGui::Begin(name.c_str(), open);
            if(ret){
                func();
            }
            ImGui::End();
            return ret;
        }

        template<typename F>
        static bool BeginPopupModalIfEnd(std::string name, F&& func){
            bool ret = ImGui::BeginPopupModal(name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if(ret){
                func();
            }
            ImGui::EndPopup();
            return ret;
        }
    };
}; // end of namespace
