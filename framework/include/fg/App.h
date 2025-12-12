/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Mod.h"
namespace fog
{
    class App
    {
    public:
        virtual void add(Mod *mod) = 0;
        virtual void setup() = 0;
        virtual void startRendering() = 0;
        virtual void close() = 0;
    };
};
