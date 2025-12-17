/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "imgui.h"
#include <string>
#include "fg/ogre.h"
#include "fg/core.h"
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
