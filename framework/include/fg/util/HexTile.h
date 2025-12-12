/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util/Point2.h"
#include "fg/util/CollectionUtil.h"
#include "fg/util/Context.h"
#include "fg/util/Common.h"
#include "fg/util/Box2.h"
#include "fg/util/Transform.h"

namespace fog
{
    struct HexTile
    {

        struct Key : public Point2<int>
        {

            Key() : Key(-1, -1) {}
            Key(int x, int y) : Point2<int>(x, y)
            {
            }
            Key(const Point2<float> cKey) : Key(static_cast<int>(cKey.x), static_cast<int>(cKey.y))
            {
            }

            bool operator==(const Key &ck) const
            {
                return this->x == ck.x && this->y == ck.y;
            }

            struct Hash
            {
                std::size_t operator()(const Key &p) const
                {
                    auto h1 = std::hash<int>{}(p.x);
                    auto h2 = std::hash<int>{}(p.y);
                    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
                }
            };

            Vector3 transform3()
            {
                return this->cast<float>().transform(Transform::CellCentreByKey()).transform3(*Context<Transform::D2H2D3>::get());
            }

            Vector3 transform3(Point2<float> pointInCell)
            {
                return (this->cast<float>().transform(Transform::CellCentreByKey()) + pointInCell).transform3(*Context<Transform::D2H2D3>::get());
            }

            Vector3 transform3(Point2<float> pointInCell, float h)
            {
                return (this->cast<float>().transform(Transform::CellCentreByKey()) + pointInCell).transform3(Transform::D2_HV_D3(*Context<Transform::D2H2D3>::get(), h));
            }

            static Key from(Point2<float> p)
            {
                return p.transform(Transform::CentreToCellKey());
            };

            Point2<float> getCentre()
            {
                return this->Point2<int>::cast<float>().transform(Transform::CellCentreByKey());
            }

            static void getCentres(std::vector<Key> &cks, std::vector<Point2<float>> &ret)
            {
                CollectionUtil::transform<Key, Point2<float>>(cks, ret, [](Key &ck)
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

        class KeyUtil
        {
        public:
            static std::string toString(const Key &cKey)
            {
                return "(" + std::to_string(cKey.x) + "," + std::to_string(cKey.y) + ")";
            }
        };
    };


}; // end of namespace
