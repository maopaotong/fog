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

namespace fog
{

    class UIState
    {
    protected:
        UIState *pState = nullptr;
        std::vector<UIState *> children;
        bool active = false;
        std::string name;

        void openChildren()
        {
            for (auto cState : children)
            {

                if (cState->active)
                {
                    cState->open();
                }
            }
        };
        template <typename T>
        Property::Ref<T> getProperty(std::string name, bool now)
        {
            return Context<Property::Bag>::get()->getProperty<T>(name, now);
        }

    public:
        UIState(std::string name) : name(name)
        {
        }
        virtual ~UIState()
        {
        }
        UIState(UIState &&) = delete;
        UIState(UIState &) = delete;
        UIState &operator=(UIState &&) = delete;
        UIState &operator=(UIState &) = delete;

        virtual void init() {}
        std::string getName()
        {
            return name;
        }
        void changeActive()
        {
            this->setActive(!this->active);
        }
        void setActive(bool active)
        {
            this->active = active;
        }
        bool *activePtr()
        {
            return &this->active;
        }
        bool isActive()
        {
            return active;
        }
        void add(UIState *child)
        {
            child->pState = this;
            children.push_back(child);
        }

        std::string getFullName()
        {
            if (this->pState)
            {
                return this->pState->getFullName() + "." + name;
            }
            else
            {
                return name;
            }
        }

        virtual bool open()
        {
            if (!this->active)
            {
                return false;
            }
            std::string fName = this->getFullName();
            return ImGuiUtil::BeginIfEnd(fName, &this->active, [this]()
                                         {
                                             this->doOpen();
                                             this->openChildren(); //
                                         });
        };
        virtual void doOpen()
        {
        }
    };

}; // end of namespace
