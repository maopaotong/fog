/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
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
        Point2<int> p;
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

        bool isWalkable(Point2<int> p) const
        {
            if (!isInSide(p))
            {
                return false;
            }

            return getCost(p) > 0;
        }
    public:
        static const int OBSTACLE = 0;
        static const int DEFAULT_COST = 1;

        CostMap(int w, int h) : width(w), height(h)
        {
            costGrid.resize(height, std::vector<int>(width, DEFAULT_COST));
        }

        void setCost(Point2<int> p, int cost)
        {
            costGrid[p.y][p.x] = cost;
        }

        int getCost(Point2<int> p) const
        {
            return costGrid[p.y][p.x];
        }
        bool isInSide(Point2<int> p) const
        {
            return p.x >= 0 && p.x < this->width && p.y >= 0 && p.y < this->height;
        }
        

        HexTile::Key getNeighbor(Point2<int> p, int direction) const
        {
            if (direction < 0 || direction >= 6)
                return p;
            if (p.y % 2 == 0)
            {
                return {p.x + dx_even[direction], p.y + dy_even[direction]};
            }
            else
            {
                return {p.x + dx_odd[direction], p.y + dy_odd[direction]};
            }
        }

        float heuristic(Point2<int> p1, Point2<int> p2) const
        {
            int q1 = p1.x - (p1.y - (p1.y & 1)) / 2;
            int r1 = p1.y;
            int s1 = -q1 - r1;

            int q2 = p2.x - (p2.y - (p2.y & 1)) / 2;
            int r2 = p2.y;
            int s2 = -q2 - r2;

            int dq = abs(q1 - q2);
            int dr = abs(r1 - r2);
            int ds = abs(s1 - s2);

            return static_cast<float>(std::max({dq, dr, ds}) * DEFAULT_COST);
        }

        std::vector<HexTile::Key> findPath(HexTile::Key start, HexTile::Key end)
        {
            std::vector<HexTile::Key> path = findPathInternal(start, end);
            std::vector<HexTile::Key> cellPath;
            for (const auto &pos : path)
            {
                cellPath.emplace_back(static_cast<int>(pos.x), static_cast<int>(pos.y));
            }
            return cellPath;
        }
        std::vector<HexTile::Key> findNormPath(HexTile::Key start, HexTile::Key end)
        {
            return findPathInternal(start, end);
        }
        std::vector<HexTile::Key> findPathInternal(HexTile::Key start, HexTile::Key end)
        {
            using Pos = HexTile::Key;

            if (!isWalkable(start) || !isWalkable(end))
            {
                return {};
            }

            std::priority_queue<NavNode, std::vector<NavNode>, std::greater<NavNode>> openList;
            std::unordered_map<Pos, Pos, HexTile::Key::Hash> cameFrom;
            std::unordered_map<Pos, float, HexTile::Key::Hash> gScore;
            std::unordered_set<Pos, HexTile::Key::Hash> closed;

            NavNode startNode = {start, 0, heuristic(start, end)};
            openList.push(startNode);
            gScore[{start}] = 0;

            while (!openList.empty())
            {
                NavNode current = openList.top();
                openList.pop();
                Pos currPos = current.p;

                if (closed.find(currPos) != closed.end())
                    continue;
                closed.insert(currPos);

                if (current.p == end)
                {
                    return reconstructPath(cameFrom, currPos);
                }

                for (int i = 0; i < 6; i++)
                {
                    auto nk = getNeighbor(current.p, i);
                    if (!isWalkable(nk))
                        continue;

                    Pos neighbor = nk;
                    int moveCost = getCost(nk);
                    float tentativeG = gScore[currPos] + moveCost;

                    auto it = gScore.find(neighbor);
                    if (it == gScore.end() || tentativeG < it->second)
                    {
                        cameFrom[neighbor] = currPos;
                        gScore[neighbor] = tentativeG;
                        float h = heuristic(nk, end);

                        NavNode node = {nk, tentativeG, h};
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
                totalCost += getCost(Point2<int>(x, y));
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
