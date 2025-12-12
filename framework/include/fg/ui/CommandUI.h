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
#include "imgui.h"
#include <string>
#include "fg/CoreMod.h"
#include "fg/State.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"

namespace fog
{
    class CommandUI
    {
        CoreMod *core;
        CostMap *costMap;
        int cellX = 0;
        int cellY = 0;
        float heightOffset = 10.0f;

    public:
        CommandUI(CoreMod *core, CostMap *costMap)
        {
            this->core = core;
            this->costMap = costMap;
        }

        bool Open()
        {
            if (!ImGui::Begin("Command Tray"))
            {
                return false;
            }

            ImGui::SliderInt("Cell:X", &cellX, 0, costMap->getWidth());
            ImGui::SliderInt("Cell:Y", &cellY, 0, costMap->getHeight());
            ImGui::SliderFloat("HeightOffset", &heightOffset, 0.0f, 100.0f);
            

            if (ImGui::Button("Test:TerrainGround"))
            {
               // vertices ={};// TerrainedGround::calculateVertices3D(cellX, cellY, costMap, CostMap::hexSize, heightOffset);
            }
            if (ImGui::Button("Test:Ground"))
            {
                //std::vector<Vector3> row = Ground::calculateVertices3D(cellX, cellY, costMap, CostMap::hexSize, 1.0f, Global::getTerrainHeightAtPositionWithOffset, heightOffset);
               // vertices = {row};
            }

            if (ImGui::Button("Clear"))
            {
               // vertices = {};
            }

            int rows = 0;//vertices.size();
            for (int i = 0; i < rows; i++)
            {
                // std::vector<Vector3> &row = {};//vertices[i];
                // int cols = row.size();
                // ImGui::Text(fmt::format("{}@{}/{}:", i, rows, cols).c_str());
                // for (int j = 0; j < cols; j++)
                // {
                //     ImGui::SameLine();
                //     Vector3 val = row[j];
                //     ImGuiUtil::Text(val);
                // }
            }

            ImGui::End();
            return true;
        }
    };

}; // end of namespace
