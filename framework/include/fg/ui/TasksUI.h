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
#include "fg/util/Options.h"
namespace fog
{
    class TasksUI : public UIState
    {
    protected:
        Options options;
        MovingStateManager * movingStateManager;
    public:
        INJECT(TasksUI(MovingStateManager * movingStateManager)) : UIState("TasksUI"),movingStateManager (movingStateManager)
        {
        }

        void doOpen() override
        {
            int id = 0;
            
            //
            //Context<MovingStateManager>::get()
            movingStateManager->forEachTask([this, &id](MoveToCellTask* task)
            {
                std::string label = fmt::format("Task##{}", id);
                if (ImGui::TreeNode(label.c_str()))
                {
                    ImGui::Text(("Target HexTile::Key: " + CellUtil::toString(task->getTargetCellKey())).c_str());
                    ImGui::Text(("State Active: " + std::to_string(task->getState()->isActive())).c_str());
                    ImGui::TreePop();
                }
                id++;
                return true;
            });

            // Context<State>::get()->forEachChild([](State* state)
            // {
            //     Tasks::Slot * owner = state->tryGetSlot(0);

            //     if(owner){
            //         ImGui::Text(("owner.stackSize:" + std::to_string(owner->stackSize())).c_str()); //
            //         ImGui::SameLine();                                 //
            //         ImGui::Text(("owner.popCounter:" + std::to_string(owner->getPopCounter())).c_str());
                    
            //     }
            //     return true;
            //     //
            // });
            
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
