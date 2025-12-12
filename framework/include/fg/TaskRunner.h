/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <string>
#include <vector>
#include <typeindex>

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>
#include <stack>
#include "Stairs.h"
#include "OgreFrameListener.h"
#include "Tasks.h"
#include "State.h"

namespace fog
{

    using namespace Ogre;
    class TaskRunner : public FrameListener
    {
        State *state;

    public:
        TaskRunner(State *state) : state(state)
        {
        }

    };
};
//
