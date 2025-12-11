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
#include "fg/defines.h"
#include <string>
#include "fg/CoreMod.h"
#include "fg/State.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"
#include "fg/util/Options.h"
#include "fg/InventoryStateManager.h"
namespace fog
{
   
    static std::string to_string(InventoryType level)
    {
        auto it = InventoryTypeToString.find(level);
        if (it != InventoryTypeToString.end())
        {
            return it->second;
        }
        return "Unknown";
    }

    class InventoryUI : public UIState
    {
    protected:
        Options options;

    public:
        InventoryUI() : UIState("InventoryUI")
        {
        }

        void doOpen() override
        {
            int id = 0;

            //
            Context<InventoryStateManager>::get()-> //
                forEachInventory([this, &id](Inventory *inv)
                                 {
                                     std::string label = fmt::format("Inventory##{}", id);
                                     if (ImGui::TreeNode(label.c_str()))
                                     {
                                         ImGui::Text(to_string(inv->getType()).c_str());
                                         ImGui::SameLine();
                                         ImGui::Text(std::to_string(inv->getAmount()).c_str());
                                         ImGui::TreePop();
                                     }
                                     id++;
                                     return true; //
                                 });

            if (ImGui::Button("Return"))
            {
                this->active = false;
            }
        }

        void onApply()
        {
            //
            // core->getGame()->apply(&this->options);
        }
    };
}; // end of namespace
