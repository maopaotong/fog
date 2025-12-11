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
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"
#include "fg/util/Options.h"
#include "fg/util/Property.h"
namespace fog
{
    class PropertyRefsUI : public UIState
    {
    protected:

    public:
        PropertyRefsUI() : UIState("PropertyRefs")
        {
        }

        void doOpen() override
        {
            int id = 0;

            Context<Property::Bag>::get()->forEach([&id](const std::string &name, const Options::Option *option)
                                                     {
                                                         ImGui::PushID("PropertyRefUIID" + id++);
                                                         ImGui::Text(option->name.c_str());
                                                         ImGui::SameLine();
                                                         if (option->isType<bool>())
                                                         {

                                                             bool &valuePtr = option->getValueRef<bool>();

                                                             if (ImGui::Checkbox("V", &valuePtr))
                                                             {
                                                                 // ignore bool change.
                                                             }
                                                         }
                                                         else if (option->isType<std::string>())
                                                         {

                                                             std::string &valuePtr = option->getValueRef<std::string>();

                                                             ImGui::Text((valuePtr).c_str());
                                                         }
                                                         else if (option->isType<Vector3>())
                                                         {

                                                             Vector3 &valuePtr = option->getValueRef<Vector3>();

                                                             ImGuiUtil::Text(valuePtr);
                                                         }
                                                         else
                                                         {
                                                             ImGui::Text("TODO");
                                                         }
                                                         ImGui::PopID();
                                                         //
                                                     });

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
           //core->getGame()->apply(nullptr);
        }
    };

}; // end of namespace
