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
// using namespace Ogre;

namespace fog
{

    struct CellKey
    {
        using Type = uint8_t;
        static constexpr Type OFFSET = 0;
        static constexpr Type AXIAL = 1;
        static constexpr Type CENTRE = 2;

        struct Hash
        {
            std::size_t operator()(const Point2<int> &p) const
            {
                auto h1 = std::hash<int>{}(p.x);
                auto h2 = std::hash<int>{}(p.y);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        struct Offset : public Point2<int>
        {

            Offset() : Offset(-1, -1) {}
            Offset(int x, int y) : Point2<int>(x, y)
            {
            }
            Offset(const Point2<float> cKey) : Point2<int>(static_cast<int>(cKey.x), static_cast<int>(cKey.y))
            {
            }
            Offset(const Point2<int> cKey) : Point2<int>(cKey)
            {
            }

            bool operator==(const Point2<int> &ck) const
            {
                return this->x == ck.x && this->y == ck.y;
            }
        };
        struct Axial : public Point2<int>
        {
            Axial() : Axial(-1, -1) {}
            Axial(int x, int y) : Point2<int>(x, y)
            {
            }
            Axial(const Point2<float> cKey) : Point2<int>(static_cast<int>(cKey.x), static_cast<int>(cKey.y))
            {
            }
            Axial(const Point2<int> cKey) : Point2<int>(cKey)
            {
            }

            bool operator==(const Point2<int> &ck) const
            {
                return this->x == ck.x && this->y == ck.y;
            }
        };

        struct Centre : public Point2<int>
        {
            Centre() : Centre(-1, -1) {}
            Centre(int x, int y) : Point2<int>(x, y)
            {
            }
            Centre(const Point2<float> cKey) : Point2<int>(static_cast<int>(cKey.x), static_cast<int>(cKey.y))
            {
            }
            Centre(const Point2<int> cKey) : Point2<int>(cKey)
            {
            }

            bool operator==(const Point2<int> &ck) const
            {
                return this->x == ck.x && this->y == ck.y;
            }
        };

        // transform template dispatcher.
        template <typename K>
        struct TransformDispatcher
        {
            template <typename F>
            static Vector3 transform3(K &cKey, F &&d2d3)
            {
                static_assert(!std::is_same_v<K, K>, "unsuported cell key type.");
            }

            template <typename F>
            static Vector3 transform3(K &cKey, Point2<float> pointInCell, float h, F &&d2d3)
            {
                static_assert(!std::is_same_v<K, K>, "unsuported cell key type.");
            }
        };

        template <>
        struct TransformDispatcher<Offset>
        {

            template <typename F>
            static Vector3 transform3(Offset &cKey, F &&d2d3)
            {
                static Transform::CellKeyToCentre TF_K2C;
                return cKey.cast<float>().transform(TF_K2C).transform3(d2d3);
            }

            template <typename F>
            static Vector3 transform3(Offset &cKey, Point2<float> pointInCell, float h, F &&d2d3)
            {
                static Transform::CellKeyToCentre TF_K2C{};
                return (cKey.cast<float>().transform(TF_K2C) + pointInCell).transform3(h, d2d3);
            }
        };

        template <>
        struct TransformDispatcher<Axial>
        {

            template <typename F>
            static Vector3 transform3(Axial &cKey, F &&d2d3)
            {
                throw std::runtime_error("TODO");
            }

            template <typename F>
            static Vector3 transform3(Axial &cKey, Point2<float> pointInCell, float h, F &&d2d3)
            {
                throw std::runtime_error("TODO");
            }
        };

        // transform methods

        // static Transform::CentreToCellKey TF_C2K;
        // static Transform::CellKeyToCentre TF_K2C;

        template <typename K, typename F>
        static Vector3 transform3(K &cKey, F &&d2d3)
        {
            return TransformDispatcher<K>::transform3(cKey, d2d3);
        }
        //

        template <typename K>
        static void getCentres(std::vector<K> &cks, std::vector<Point2<float>> &ret)
        {
            CollectionUtil::transform<K, Point2<float>>(cks, ret, [](K &ck)
                                                        {
                                                            return CellKey::getCentre(ck); //
                                                        });
        }

        //
        template <typename K, typename F>
        static Vector3 transform3(K &cKey, Point2<float> pointInCell, float h, F &&d2d3)
        {
            return TransformDispatcher<K>::transform3(cKey, pointInCell, h, d2d3);
        }

        //
        template <typename K>
        static K from(Point2<float> p)
        {
            static_assert(sizeof(T) == 0, "unsupported cell key type.");
        };

        template <>
        static Offset from<Offset>(Point2<float> p)
        {
            static Transform::CentreToCellKey TF_C2K;
            return p.transform(TF_C2K);
        };

        template <>
        static Axial from<Axial>(Point2<float> p)
        {
            throw std::runtime_error("TODO");
        };

        // transform from key to centre
        template <typename K>
        static Point2<float> getCentre(K &cKey)
        {
            static_assert(sizeof(T) == 0, "unsupported cell key type.");
        }

        template <>
        static Point2<float> getCentre<Offset>(Offset &cKey)
        {
            static Transform::CellKeyToCentre TF_K2C;
            return cKey.Point2<int>::cast<float>().transform(TF_K2C);
        }

        template <>
        static Point2<float> getCentre<Axial>(Axial &cKey)
        {
            throw std::runtime_error("TODO");
        }
        //

        template <typename K>
        Box2<float> getBoxIn2D(K &cKey)
        {
            // Vector2 centre = getOrigin2D(cKey);
            Point2<float> centre = getCentre<K>(cKey);
            float left = centre.x - 1.0; // rad
            float bottom = centre.y - std::sqrt(3.0) / 2.0;
            return Box2<float>(left, bottom, left + 2.0, bottom + std::sqrt(3.0));
        }

        template <typename K>
        static Box2<float> getOuterBoxIn2D(K &cKey)
        {
            // Vector2 centre = getOrigin2D(cKey);

            Point2<float> centre = getCentre<K>(cKey);

            float left = centre.x - 1.0;                    // rad
            float bottom = centre.y - 2.0 / std::sqrt(3.0); // R
            return Box2<float>(left, bottom, left + 2.0, bottom + 4.0 / std::sqrt(3.0));
        }

        template <typename K>
        static Box2<float> getBoxInUV(K &cKey, int width, int height)
        {
            Box2<float> r = getBoxIn2D<K>(cKey);
            return r.transform(Transform::D2CellWorldUV(width, height));
        }

        template <typename K>
        static Box2<float> getOuterBoxInUV(K &cKey, int width, int height)
        {
            Box2<float> r = getOuterBoxIn2D<K>(cKey);
            return r.transform(Transform::D2CellWorldUV(width, height));
        }
        /*
         */
    };

};
