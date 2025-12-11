/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
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
        std::vector<HexTile::Key> currentPath;        

    public:
        PathState()
        {
        }
        ~PathState()
        {
            this->resetPathColor(true);
        }

        void setPath(const std::vector<HexTile::Key> &path)
        {
            this->resetPathColor(true);
            currentPath = path;                        
            this->resetPathColor(false);
        }

        void resetPathColor(bool unset)
        {
            for (auto it = currentPath.begin(); it != currentPath.end(); ++it)
            {
                HexTile::Key cKey = *it;
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
