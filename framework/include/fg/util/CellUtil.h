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
        static std::string toString(const CellKey &cKey)
        {
            return "(" + std::to_string(cKey.col) + "," + std::to_string(cKey.row) + ")";
        }
    };
}; // end of namespace
