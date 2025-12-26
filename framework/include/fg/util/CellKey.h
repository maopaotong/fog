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
//using namespace Ogre;

namespace fog
{

    struct CellKey
    {
        using Type = uint8_t;
        static constexpr Type OFFSET = 0;
        static constexpr Type AXIAL = 1;

        struct BaseKey : public Point2<int>
        {
            static inline Transform::CellKeyToCentre TF_K2C{};
            static inline Transform::CentreToCellKey TF_C2K{};

            BaseKey() : BaseKey(-1, -1) {}
            BaseKey(int x, int y) : Point2<int>(x, y)
            {
            }
            BaseKey(const Point2<float> cKey) : BaseKey(static_cast<int>(cKey.x), static_cast<int>(cKey.y))
            {
            }
            BaseKey(const Point2<int> cKey) : Point2<int>(cKey)
            {
            }

            struct Hash
            {
                std::size_t operator()(const BaseKey &p) const
                {
                    auto h1 = std::hash<int>{}(p.x);
                    auto h2 = std::hash<int>{}(p.y);
                    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
                }
            };

        };

        struct Offset : public BaseKey
        {

            Offset() : Offset(-1, -1) {}
            Offset(int x, int y) : BaseKey(x, y)
            {
            }
            Offset(const Point2<float> cKey) : BaseKey(static_cast<int>(cKey.x), static_cast<int>(cKey.y))
            {
            }
            Offset(const Point2<int> cKey) : BaseKey(cKey)
            {
            }
            
            bool operator==(const BaseKey &ck) const
            {
                return this->x == ck.x && this->y == ck.y;
            }

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

            static BaseKey from(Point2<float> p)
            {
                return p.transform(TF_C2K);
            };

            Point2<float> getCentre()
            {
                return this->Point2<int>::cast<float>().transform(TF_K2C);
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
        };
        struct Axial : public BaseKey
        {
        };

        template <typename K>
        static void getCentres(std::vector<K> &cks, std::vector<Point2<float>> &ret)
        {
            static_assert(sizeof(K) == 0, "not supported type.");
        }

        template <>
        static void getCentres<Offset>(std::vector<Offset> &cks, std::vector<Point2<float>> &ret)
        {
            CollectionUtil::transform<Offset, Point2<float>>(cks, ret, [](Offset &ck)
                                                             {
                                                                 return ck.getCentre(); //
                                                             });
        }

        /*
         */
    };

};
