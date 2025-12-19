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
