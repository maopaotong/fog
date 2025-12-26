/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util/Common.h"
#include <cmath>
#include <algorithm>
#include "Common.h"

#include "Point2.h"
#include "Box2.h"

#include "CollectionUtil.h"
#include "Transform.h"
using namespace Ogre;

namespace fog
{

    struct CellKey : public Point2<int>
    {
        static inline Transform::CellKeyToCentre TF_K2C{};
        static inline Transform::CentreToCellKey TF_C2K{};

        CellKey() : CellKey(-1, -1) {}
        CellKey(int x, int y) : Point2<int>(x, y)
        {
        }
        CellKey(const Point2<float> cKey) : CellKey(static_cast<int>(cKey.x), static_cast<int>(cKey.y))
        {
        }
        CellKey(const Point2<int> cKey) : Point2<int>(cKey)
        {
        }

        bool operator==(const CellKey &ck) const
        {
            return this->x == ck.x && this->y == ck.y;
        }

        struct Hash
        {
            std::size_t operator()(const CellKey &p) const
            {
                auto h1 = std::hash<int>{}(p.x);
                auto h2 = std::hash<int>{}(p.y);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        template <typename F>
        Vector3 transform3(F &&d2d3)
        {
            return this->cast<float>().transform(TF_K2C).transform3(d2d3);
        }

        template <typename F>
        Vector3 transform3(Point2<float> pointInCell, float h, F &&d2d3)
        {
            return (this->cast<float>().transform(TF_K2C) + pointInCell).transform3(h, d2d3);
        }

        static CellKey from(Point2<float> p)
        {
            return p.transform(TF_C2K);
        };

        Point2<float> getCentre()
        {
            return this->Point2<int>::cast<float>().transform(TF_K2C);
        }

        static void getCentres(std::vector<CellKey> &cks, std::vector<Point2<float>> &ret)
        {
            CollectionUtil::transform<CellKey, Point2<float>>(cks, ret, [](CellKey &ck)
                                                              {
                                                                  return ck.getCentre(); //
                                                              });
        }

        Box2<float> getBoxIn2D()
        {
            // Vector2 centre = getOrigin2D(cKey);
            Point2<float> centre = getCentre();
            float left = centre.x - 1.0; // rad
            float bottom = centre.y - std::sqrt(3.0) / 2.0;
            return Box2<float>(left, bottom, left + 2.0, bottom + std::sqrt(3.0));
        }

        Box2<float> getOuterBoxIn2D()
        {
            // Vector2 centre = getOrigin2D(cKey);

            Point2<float> centre = getCentre();

            float left = centre.x - 1.0;                    // rad
            float bottom = centre.y - 2.0 / std::sqrt(3.0); // R
            return Box2<float>(left, bottom, left + 2.0, bottom + 4.0 / std::sqrt(3.0));
        }

        Box2<float> getBoxInUV(int width, int height)
        {
            Box2<float> r = getBoxIn2D();
            return r.transform(Transform::D2CellWorldUV(width, height));
        }
        Box2<float> getOuterBoxInUV(int width, int height)
        {
            Box2<float> r = getOuterBoxIn2D();
            return r.transform(Transform::D2CellWorldUV(width, height));
        }

        //
        template <typename F, typename... Args>
        static void forEachPointOnCircle(float size, float offsetAngle, F &&visit, Args... args)
        {

            // rad=1, RAD=2
            float angle = 360.0f / size;

            for (int i = 0; i < size; i++)
            {

                float angle_rad = (angle * i + offsetAngle) * Ogre::Math::PI / 180.0f;

                float dx = std::cos(angle_rad);
                float dy = std::sin(angle_rad);

                visit(i, Vector2(dx, dy), args...);
            }
        };

        /*
         */
    };

};
