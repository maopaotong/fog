/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

//fg modules
#include "fg/util.h"
#include "fg/core.h"
//Same folder.
#include "Common.h"
#include "SimpleCore.h"
namespace fog
{

    class SimpleApp : public App
    {
    private:
    public: 
        INJECT(SimpleApp())
        {
            
        }
        virtual ~SimpleApp()
        {
        }

        void setup() override
        {

            
        }


        void close() override
        {
            std::cout << "Closing application.\n";            

        }
    };
}; // end of namespace
