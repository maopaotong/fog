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

    public:
        INJECT(EntryUI(MovingStateManager *movingStateManager)) : movingStateManager(movingStateManager), UIState("EntryUI")
        {
            this->active = true;
        }

        void init() override
        {
            this->add(new TopBarUI());
            this->add(new InventoryUI());
            this->add(new OptionsUI());
            this->add(new PropertyRefsUI());
            this->add(new ActiveTrayUI());
            this->add(new BuildingTrayUI());
            this->add(new StatisticTrayUI());
            this->add(new TasksUI(movingStateManager));

            // Quit:
            this->add(new QuitUI());
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
