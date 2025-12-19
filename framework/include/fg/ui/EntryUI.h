/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "imgui.h"
#include <string>
#include "fg/ogre.h"

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

    struct EntryUI : public UIState
    {
        struct ChildInfo
        {
            UIState *ui;
            std::string name;
        };
        
    public:
        struct Children
        {
            TopBarUI *ui0;
            InventoryUI *ui1;
            OptionsUI *ui2;
            PropertyRefsUI *ui3;
            ActiveTrayUI *ui4;
            BuildingTrayUI *ui5;
            StatisticTrayUI *ui6;
            TasksUI *ui7;
            QuitUI *ui8;
            INJECT(Children(TopBarUI *ui0,
                            InventoryUI *ui1,
                            OptionsUI *ui2,
                            PropertyRefsUI *ui3,
                            ActiveTrayUI *ui4,
                            BuildingTrayUI *ui5,
                            StatisticTrayUI *ui6,
                            TasksUI *ui7,
                            QuitUI *ui8)) : ui0(ui0), ui1(ui1), ui2(ui2), ui3(ui3), ui4(ui4), ui5(ui5), ui6(ui6), ui7(ui7), ui8(ui8)
            {
            }
        };

    protected:
        std::vector<ChildInfo> childInfos;
        MovingStateManager *movingStateManager;
        CoreMod *core;
        InventoryManager *inventoryManager;
        Event::Bus *eventBus;
        MovableStateManager *msm;
        Children cs;

    public:
        INJECT(EntryUI(MovingStateManager *movingStateManager, CoreMod *core,
                       MovableStateManager *msm,
                       Event::Bus *eventBus,
                       Children *cs,
                       InventoryManager *inventoryManager)) : inventoryManager(inventoryManager),
                                                              eventBus(eventBus),
                                                              cs(*cs), msm(msm),
                                                              movingStateManager(movingStateManager), UIState("EntryUI"), core(core)
        {
            this->active = true;
        }

        void init() override
        {

            this->add(cs.ui0);
            this->add(cs.ui1);
            this->add(cs.ui2);
            this->add(cs.ui3);
            this->add(cs.ui4);
            this->add(cs.ui5);
            this->add(cs.ui6);
            this->add(cs.ui7);
            this->add(cs.ui8);
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
