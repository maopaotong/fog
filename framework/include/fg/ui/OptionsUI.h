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
#include "fg/util/Options.h"
namespace fog
{
    class OptionsUI : public UIState
    {
    protected:
        Options options;

    public:
        OptionsUI() : UIState("Options")
        {
            options.add("Show-plain-cell?", false);
            options.add("Video-Mode", "1024 x 768");
        }

        void doOpen() override
        {
            int id = 0;
            this->options.forEach([&id](const std::string name, const Options::Option *option)
                                  {
                                      ImGui::PushID("OptionsUID" + id++);

                                      if (option->isType<bool>())
                                      {
                                          bool &valuePtr = option->getValueRef<bool>();

                                          if (ImGui::Checkbox(option->name.c_str(), &valuePtr))
                                          {
                                              // ignore bool change.
                                          }
                                      }
                                      else if (option->isType<std::string>())
                                      {

                                          std::string &valuePtr = option->getValueRef<std::string>();

                                          ImGui::Text((valuePtr).c_str());

                                      } //
                                      ImGui::PopID(); //
                                  } //
            );

            //
            if (ImGui::Button("Apply"))
            {
                this->onApply();
            }

            ImGui::SameLine();

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
