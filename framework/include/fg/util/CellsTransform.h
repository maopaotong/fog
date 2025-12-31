/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "CellKey.h"

// using namespace Ogre;

namespace fog
{
    struct CellsTransform
    {
        using Layout = Cell::Layout;
        using System = Cell::System;

        // // pointy
        // template <Layout l, System s11, System s22>
        // struct TransformTag
        // {
        //     static constexpr Layout layout = l;
        //     static constexpr System s1 = s11;
        //     static constexpr System s2 = s22;
        // };

        // template

        template <Cell::Layout layout, Cell::System s1, Cell::System s2>
        static typename std::vector<typename Cell::SystemInfo<s2>::type>
        transformAll(std::vector<typename Cell::SystemInfo<s1>::type> &cks)
        {

            using K1 = typename Cell::SystemInfo<s1>::type;
            using K2 = typename Cell::SystemInfo<s2>::type;

            std::vector<K2> ret;

            CollectionUtil::transform<K1, K2>(cks, ret, [](K1 &ck)
                                              { return Cell::Transform<layout>::transform<s1, s2>(ck); });
            return ret;
        }

    private:
    };

    // using CellKey = Cell::OffsetKey;
    // using Hash = Cell::HashOp<Cell::Offset>;

};
