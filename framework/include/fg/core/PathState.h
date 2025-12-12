/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/MaterialNames.h"
#include "fg/util/CostMap.h"
#include "fg/util/HexGridPrinter.h"
#include "fg/CellInstanceManager.h"
namespace fog
{
    using namespace Ogre;

#define DEFAULT_HIGH_OFFSET 1.1f

    class PathState : public State
    {
        std::vector<Cell::Key> currentPath;        

    public:
        PathState()
        {
        }
        ~PathState()
        {
            this->resetPathColor(true);
        }

        void setPath(const std::vector<Cell::Key> &path)
        {
            this->resetPathColor(true);
            currentPath = path;                        
            this->resetPathColor(false);
        }

        void resetPathColor(bool unset)
        {
            for (auto it = currentPath.begin(); it != currentPath.end(); ++it)
            {
                Cell::Key cKey = *it;
                CellInstanceState *cis = Context<CellInstanceManager>::get()->getCellInstanceStateByCellKey(cKey);
                if (unset)
                {
                    cis->popColour();
                }
                else
                {

                    cis->pushColour(Ogre::ColourValue::Blue);
                }
            }
        }

    };

}; // end of namespace
