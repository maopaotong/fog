/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <string>
#include <fmt/format.h>

namespace fog
{

class Video
{
public:
    struct Mode
    {
        int width;
        int height;
        int bpp;

        operator std::string() const
        {
            return fmt::format("{} x {} x {}", width, height, bpp);
        }
    };
};
};
