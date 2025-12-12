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
// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>
#include "fg/util/HexTile.h"
namespace fog
{
    
    // === NavNode structure ===
    struct NavNode
    {
        int x, y;
        float g, h;
        float f() const { return g + h; }
        bool operator>(const NavNode &other) const { return f() > other.f(); }
    };

    class CostMap
    {
        struct PairHash
    {
        template <typename T, typename U>
        std::size_t operator()(const std::pair<T, U> &p) const
        {
            auto h1 = std::hash<T>{}(p.first);
            auto h2 = std::hash<U>{}(p.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    public:
        static constexpr float hexSize = 30.0f; // inner radius

    private:
        // === Fixed hexagon neighbor offsets (flat-top) ===
        int dx_even[6] = {+1, 0, -1, -1, -1, 0};
        int dy_even[6] = {0, -1, -1, 0, +1, +1};
        int dx_odd[6] = {+1, +1, 0, -1, 0, +1};
        int dy_odd[6] = {0, -1, -1, 0, +1, +1};

    public:
        std::vector<std::vector<int>> costGrid;
        int width, height;

    public:
        static const int OBSTACLE = 0;
        static const int DEFAULT_COST = 1;

        CostMap(int w, int h) : width(w), height(h)
        {
            costGrid.resize(height, std::vector<int>(width, DEFAULT_COST));
        }

        void setCost(int x, int y, int cost)
        {
            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                costGrid[y][x] = cost;
            }
        }

        int getCost(int x, int y) const
        {
            if (x < 0 || x >= width || y < 0 || y >= height)
                return OBSTACLE;
            return costGrid[y][x];
        }

        bool isWalkable(int x, int y) const
        {
            return getCost(x, y) > 0;
        }

        HexTile::Key getNeighbor(int x, int y, int direction) const
        {
            if (direction < 0 || direction >= 6)
                return {x, y};
            if (y % 2 == 0)
            {
                return {x + dx_even[direction], y + dy_even[direction]};
            }
            else
            {
                return {x + dx_odd[direction], y + dy_odd[direction]};
            }
        }

        float heuristic(int x1, int y1, int x2, int y2) const
        {
            int q1 = x1 - (y1 - (y1 & 1)) / 2;
            int r1 = y1;
            int s1 = -q1 - r1;

            int q2 = x2 - (y2 - (y2 & 1)) / 2;
            int r2 = y2;
            int s2 = -q2 - r2;

            int dq = abs(q1 - q2);
            int dr = abs(r1 - r2);
            int ds = abs(s1 - s2);

            return static_cast<float>(std::max({dq, dr, ds}) * DEFAULT_COST);
        }

        std::vector<HexTile::Key> findPath(HexTile::Key start, HexTile::Key end)
        {
            std::vector<HexTile::Key> path = findPathInternal(start.x, start.y, end.x, end.y);
            std::vector<HexTile::Key> cellPath;
            for (const auto &pos : path)
            {
                cellPath.emplace_back(static_cast<int>(pos.x), static_cast<int>(pos.y));
            }
            return cellPath;
        }
        std::vector<HexTile::Key> findNormPath(HexTile::Key start, HexTile::Key end)
        {
            return findPathInternal(start.x, start.y, end.x, end.y);
        }
        std::vector<HexTile::Key> findPathInternal(int startX, int startY, int endX, int endY)
        {
            using Pos = HexTile::Key;

            if (!isWalkable(startX, startY) || !isWalkable(endX, endY))
            {
                return {};
            }

            std::priority_queue<NavNode, std::vector<NavNode>, std::greater<NavNode>> openList;
            std::unordered_map<Pos, Pos, HexTile::Key::Hash> cameFrom;
            std::unordered_map<Pos, float, HexTile::Key::Hash> gScore;
            std::unordered_set<Pos, HexTile::Key::Hash> closed;

            NavNode start = {startX, startY, 0, heuristic(startX, startY, endX, endY)};
            openList.push(start);
            gScore[{startX, startY}] = 0;

            while (!openList.empty())
            {
                NavNode current = openList.top();
                openList.pop();
                Pos currPos = {current.x, current.y};

                if (closed.find(currPos) != closed.end())
                    continue;
                closed.insert(currPos);

                if (current.x == endX && current.y == endY)
                {
                    return reconstructPath(cameFrom, currPos);
                }

                for (int i = 0; i < 6; i++)
                {
                    auto [nx, ny] = getNeighbor(current.x, current.y, i);
                    if (!isWalkable(nx, ny))
                        continue;

                    Pos neighbor = {nx, ny};
                    int moveCost = getCost(nx, ny);
                    float tentativeG = gScore[currPos] + moveCost;

                    auto it = gScore.find(neighbor);
                    if (it == gScore.end() || tentativeG < it->second)
                    {
                        cameFrom[neighbor] = currPos;
                        gScore[neighbor] = tentativeG;
                        float h = heuristic(nx, ny, endX, endY);

                        NavNode node = {nx, ny, tentativeG, h};
                        openList.push(node);
                    }
                }
            }

            return {};
        }

    private:
        std::vector<HexTile::Key> reconstructPath(
            const std::unordered_map<HexTile::Key, HexTile::Key, HexTile::Key::Hash> &cameFrom,
            const HexTile::Key &current) const
        {

            std::vector<HexTile::Key> path;
            HexTile::Key node = current;

            while (cameFrom.find(node) != cameFrom.end())
            {
                path.push_back(HexTile::Key(node.x, node.y));
                node = cameFrom.at(node);
            }
            path.push_back(HexTile::Key(node.x, node.y));

            std::reverse(path.begin(), path.end());
            return path;
        }

    public:
        float calculatePathCost(const std::vector<Ogre::Vector2> &path) const
        {
            float totalCost = 0;
            for (size_t i = 1; i < path.size(); i++)
            {
                int x = static_cast<int>(path[i].x);
                int y = static_cast<int>(path[i].y);
                totalCost += getCost(x, y);
            }
            return totalCost;
        }

        // === Data interface for Ogre rendering ===
        const std::vector<std::vector<int>> &getCostGrid() const { return costGrid; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }

    private:
    };
}; // end of namespace
