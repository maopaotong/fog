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
#include "imgui.h"
#include <string>
#include "fg/CoreMod.h"
#include "fg/State.h"
#include "fg/util.h"
#include <fmt/format.h>
#include "UIState.h"
#include "fg/InventoryStateManager.h"
namespace fog
{
    class TopBarUI : /* public Listener<State *, std::string &>,*/ public UIState
    {
        static float getAmount(InventoryType type)
        {
            return Context<InventoryStateManager>::get()->getAmount(type);
        }
        static float getCapacity(InventoryType type)
        {
            return Context<InventoryStateManager>::get()->getCapacity(type);
        }

    public:
        TopBarUI() : UIState("TopBar")
        {
        }
        void init() override
        {
            UIState::init();
        }

        void doOpen() override
        {
            for(auto it = InventoryTypeToString.begin(); it != InventoryTypeToString.end(); ++it){
                InventoryType type = it->first;
                std::string name = it->second;
                ImGui::Text("%s: %.0f/%.0f", name.c_str(), getAmount(type), getCapacity(type));
                ImGui::SameLine();
            }
            ImGui::NewLine();            

        }
    };

}; // end of namespace
