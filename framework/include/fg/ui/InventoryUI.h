/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "imgui.h"
#include "Common.h"
#include <string>
#include "fg/ogre.h"

#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"
#include "fg/util/Options.h"
#include "fg/demo.h"
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
        InventoryManager *inventoryStateMgr;

    public:
        INJECT(InventoryUI(InventoryManager *inventoryStateMgr)) : UIState("InventoryUI"),
                                                                inventoryStateMgr(inventoryStateMgr)
        {
        }

        void doOpen() override
        {
            int id = 0;

            //
            inventoryStateMgr-> //
                forEach([this, &id](Inventory *inv) -> bool
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
