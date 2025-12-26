/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <string>
#include "CellKey.h"
namespace fog
{
    struct CellUtil
    {
        static std::string toString(const CellKey::OffsetPointy &cKey)
        {
            return "(" + std::to_string(cKey.x) + "," + std::to_string(cKey.y) + ")";
        }
    };
}; // end of namespace
