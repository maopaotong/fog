/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Actor.h"

namespace fog
{
    // TODO use State only for dynamically created user level entity.
    // And use this statemenager for static concepts.
    template<typename T>
    struct Manager
    {
        List<std::unique_ptr<T>> children;

        void add(T *s)
        {
            children.push_back(std::unique_ptr<T>(s));
        }

        template <typename F>
        bool forEach(F &&func)
        {
            for (auto &it = children.begin(); it != children.end(); ++it)
            {
                T *s = it->get();
                if (!func(s))
                {
                    return false;
                }
            }
            return true;
        }
    };
    
};
