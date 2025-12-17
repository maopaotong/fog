/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Common.h"
namespace fog
{

    struct Mod
    {
        virtual std::string getName() = 0;
        virtual void deactive() = 0;
    };
};
