/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "imgui.h"
#include <string>
#include "fg/core.h"
#include "fg/State.h"
#include "fg/util.h"
#include <fmt/format.h>
#include "UIState.h"
#include "fg/InventoryStateManager.h"
namespace fog
{
    class TopBarUI : /* public Listener<State *, std::string &>,*/ public UIState
    {
        static float getAmount(InventoryStateManager *inventoryManager, InventoryType type)
        {
            return inventoryManager->getAmount(type);
        }
        static float getCapacity(InventoryStateManager *inventoryManager, InventoryType type)
        {
            return inventoryManager->getCapacity(type);
        }

        InventoryStateManager *inventoryManager;

    public:
        TopBarUI(InventoryStateManager *inventoryManager) : UIState("TopBar"),
                                                            inventoryManager(inventoryManager)
        {
        }
        void init() override
        {
            UIState::init();
        }

        void doOpen() override
        {
            for (auto it = InventoryTypeToString.begin(); it != InventoryTypeToString.end(); ++it)
            {
                InventoryType type = it->first;
                std::string name = it->second;
                ImGui::Text("%s: %.0f/%.0f", name.c_str(), getAmount(inventoryManager, type), getCapacity(inventoryManager, type));
                ImGui::SameLine();
            }
            ImGui::NewLine();
        }
    };

}; // end of namespace
