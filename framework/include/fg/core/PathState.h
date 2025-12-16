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
#include "fg/CellInstanceStateManager.h"
namespace fog
{
    using namespace Ogre;

#define DEFAULT_HIGH_OFFSET 1.1f

    class PathState : public State
    {
        std::vector<CellKey> currentPath;
        CellInstanceStateManager *cisManager;

    public:
        PathState(CellInstanceStateManager *cisManager) : cisManager(cisManager)
        {
        }
        ~PathState()
        {
            this->resetPathColor(true);
        }

        void setPath(const std::vector<CellKey> &path)
        {
            this->resetPathColor(true);
            currentPath = path;
            this->resetPathColor(false);
        }

        void resetPathColor(bool unset)
        {
            for (auto it = currentPath.begin(); it != currentPath.end(); ++it)
            {
                CellKey cKey = *it;
                CellInstanceState *cis = cisManager ->getCellInstanceStateByCellKey(cKey);
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
