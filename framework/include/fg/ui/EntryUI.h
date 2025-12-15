/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "imgui.h"
#include <string>
#include "fg/core.h"
#include "fg/State.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"
#include "OptionsUI.h"
#include "PropertyRefsUI.h"
#include "StatisticUI.h"
#include "QuitUI.h"
#include "TasksUI.h"
#include "BuildingTrayUI.h"
#include "ActiveTrayUI.h"
#include "InventoryUI.h"
#include "TopBarUI.h"

namespace fog
{

    class EntryUI : public UIState
    {
        struct ChildInfo
        {
            UIState *ui;
            std::string name;
        };

    protected:
        std::vector<ChildInfo> childInfos;
        MovingStateManager *movingStateManager;
        CoreMod *core;
        InventoryStateManager *inventoryManager;
        Event::Bus * eventBus;
    public:
        INJECT(EntryUI(MovingStateManager *movingStateManager, CoreMod *core,
            Event::Bus * eventBus,
            InventoryStateManager *inventoryManager)) : 
        inventoryManager(inventoryManager),
        eventBus(eventBus),
        movingStateManager(movingStateManager), UIState("EntryUI"), core(core)
        {
            this->active = true;
        }

        void init() override
        {
            this->add(new TopBarUI(inventoryManager));
            this->add(new InventoryUI(inventoryManager));
            this->add(new OptionsUI());
            this->add(new PropertyRefsUI());
            this->add(new ActiveTrayUI(eventBus));
            this->add(new BuildingTrayUI(eventBus));
            this->add(new StatisticTrayUI(core));
            this->add(new TasksUI(movingStateManager));

            // Quit:
            this->add(new QuitUI(core));
        }
        void add(UIState *child)
        {
            child->init();
            UIState::add(child);
            ChildInfo ci = {child, child->getName()};
            childInfos.push_back(ci);
        }
        bool open() override
        {
            bool ret = UIState::open();
            if (!ret)
            {
                return false;
            }

            return true;
        }
        void doOpen() override
        {

            for (auto ci : childInfos)
            {
                // if (ImGui::Button(ci.name.c_str()))
                if (ImGuiUtil::MyToggleButton(ci.name.c_str(), ci.ui->activePtr()))
                {
                    ci.ui->changeActive();
                }
            }
        };
    };

}; // end of namespace
