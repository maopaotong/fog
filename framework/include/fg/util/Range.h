/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

namespace fog
{

    template <typename T>
    struct Range
    {
        T low;
        T high;
        bool reverse;
        Range(T l, T h) : low(std::min(l, h)), high(std::max(l, h)), reverse(l > h)
        {
        }

        T mapTo(T value, T l2, T r2)
        {
            return mapTo(value, Range<T>(l2, r2));
        }
        T mapTo(T value, Range<T> r2)
        {
            T ret = r2.low + (r2.high - r2.low) * ((value - low) / (high - low));
            if (reverse == r2.reverse)
            {
                return ret;
            }
            return r2.low + r2.high - ret;
        }
    };

}; // end of namespace
