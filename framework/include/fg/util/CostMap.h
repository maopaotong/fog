/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <fg/util/Common.h>
#include <fg/util/CellKey.h>
#include <fg/util/Component.h>
namespace fog
{

    // === NavNode structure ===
    struct NavNode
    {
        CellKey p;
        float g, h;
        float f() const { return g + h; }
        bool operator>(const NavNode &other) const { return f() > other.f(); }
    };

    class CostMap
    {

    public:
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

        static constexpr float hexSize = 30.0f; // inner radius

    private:
        // pointy top.
        // int dx_even[6] =  {+1, 0, -1, -1, -1, 0};
        // int dy_even[6] = {0, 1, 1, 0, -1, -1};
        // int dx_odd[6] = {+1, +1, 0, -1, 0, +1};
        // int dy_odd[6] = {0, 1, 1, 0, -1, -1};

    public:
        // std::vector<std::vector<int>> costGrid;

        int width, height;

        template <typename F>
        bool isWalkable(const CellKey &p, F &&costFunc) const
        {
            if (!isInSide(p))
            {
                return false;
            }

            return getCost(p, costFunc) > 0;
        }

    public:
        static const int OBSTACLE = 0;
        static const int DEFAULT_COST = 1;

        struct Options
        {
            int width;
            int height;
            Options(int w, int h) : width(w), height(h)
            {
            }
        };

        INJECT(CostMap(Options opts)) : width(opts.width), height(opts.height)
        {
        }
        template <typename F>
        int getCost(const CellKey &p, F &&costFunc) const
        {
            return costFunc(p);
        }
        bool isInSide(const CellKey &p) const
        {
            return p.col >= 0 && p.col < this->width && p.row >= 0 && p.row < this->height;
        }

        CellKey getNeighbor(CellKey &p, int direction) const
        {
            if (direction < 0 || direction >= 6)
                return p;
            return Cell::getNeighbor<CellLayout,Cell::Offset>(p,direction);
        }

        float heuristic(CellKey &p1, CellKey &p2) const
        {
            int q1 = p1.col - (p1.row - (p1.row & 1)) / 2;
            int r1 = p1.row;
            int s1 = -q1 - r1;

            int q2 = p2.col - (p2.row - (p2.row & 1)) / 2;
            int r2 = p2.row;
            int s2 = -q2 - r2;

            int dq = abs(q1 - q2);
            int dr = abs(r1 - r2);
            int ds = abs(s1 - s2);

            return static_cast<float>(std::max({dq, dr, ds}) * DEFAULT_COST);
        }

        // std::vector<HexTile::Key> findPath(HexTile::Key start, HexTile::Key end)
        // {
        //     std::vector<HexTile::Key> path = findPathInternal(start, end);
        //     std::vector<HexTile::Key> cellPath;
        //     for (const auto &pos : path)
        //     {
        //         cellPath.emplace_back(static_cast<int>(pos.x), static_cast<int>(pos.y));
        //     }
        //     return cellPath;
        // }
        template <typename F>
        std::vector<CellKey> findPath(CellKey start, CellKey end, F &&costFunc)
        {
            return findPathInternal(start, end, costFunc);
        }
        template <typename F>
        std::vector<CellKey> findPathInternal(CellKey start, CellKey end, F &&costFunc)
        {
            using CellKey = CellKey;

            if (!isWalkable(start, costFunc) || !isWalkable(end, costFunc))
            {
                return {}; // empty path
            }

            std::priority_queue<NavNode, std::vector<NavNode>, std::greater<NavNode>> openList;
            std::unordered_map<CellKey, CellKey, CellKey::Hash> cameFrom;
            std::unordered_map<CellKey, float, CellKey::Hash> gScore;
            std::unordered_set<CellKey, CellKey::Hash> closed;

            NavNode startNode = {start, 0, heuristic(start, end)};
            openList.push(startNode);
            gScore[{start}] = 0;

            while (!openList.empty())
            {
                NavNode current = openList.top();
                openList.pop();
                CellKey currPos = current.p;

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
                    if (!isWalkable(nk, costFunc))
                        continue;

                    CellKey neighbor = nk;
                    int moveCost = getCost(nk, costFunc);
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
        std::vector<CellKey> reconstructPath(
            const std::unordered_map<CellKey, CellKey, CellKey::Hash> &cameFrom,
            const CellKey &current) const
        {

            std::vector<CellKey> path;
            CellKey node = current;

            while (cameFrom.find(node) != cameFrom.end())
            {
                path.push_back(CellKey::colRow(node.col, node.row));
                node = cameFrom.at(node);
            }
            path.push_back(CellKey::colRow(node.col, node.row));

            std::reverse(path.begin(), path.end());
            return path;
        }

    public:
        template <typename F>
        float calculatePathCost(const std::vector<Ogre::Vector2> &path, F &&costFunc) const
        {
            float totalCost = 0;
            for (size_t i = 1; i < path.size(); i++)
            {
                int x = static_cast<int>(path[i].x);
                int y = static_cast<int>(path[i].y);
                totalCost += getCost(CellKey::colRow(x,y), costFunc);
            }
            return totalCost;
        }

        // === Data interface for Ogre rendering ===
        int getWidth() const { return width; }
        int getHeight() const { return height; }

    private:
    };
}; // end of namespace
