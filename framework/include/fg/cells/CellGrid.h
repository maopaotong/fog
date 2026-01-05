/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/ogre.h"

#include "fg/util.h"
#include "Common.h"

namespace fog
{
    struct CellsGrid
    {
        
        Point2<float> a;
        Point2<float> b;
        Point2<float> c;
        Point2<float> d;
        //
        CellKey cKey1;
        CellKey cKey2;
        CellType type1;
        CellType type2;
        
        Point2<float> centre1;
        Point2<float> centre2;
        float height1;
        float height2;      
        float aHeight;  

      
        
    };

}; // end of namespace
