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
#include "TemplateUtil.h"

// using namespace Ogre;

namespace fog
{

    struct CellKey
    {
        using Type = uint8_t;
        // static constexpr Type OFFSET = 0;
        // static constexpr Type AXIAL = 1;
        // static constexpr Type CENTRE = 2;
        struct Offset;
        struct Axial;
        struct Centre;
        using OC = std::tuple<Offset, Centre>;
        using CO = std::tuple<Centre, Offset>;
        // Note: for the reason of template impl limitation, the order is not what you see: Offset => Centre => Point2<float>
        using OCP = std::tuple<Offset, Point2<float>, Centre>;

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

        struct Centre : public Point2<float>
        {
            Centre() : Centre(-1, -1) {}
            Centre(float x, float y) : Point2<float>(x, y)
            {
            }
            Centre(const Point2<float> cKey) : Point2<float>(cKey.x, cKey.y)
            {
            }

        };

        struct Pos3D : public Point3<float>
        {
        };

        // transform template dispatcher.
        template <typename K>
        struct TransformDispatcher
        {
            // template <typename F>
            // static Vector3 transform3(K &cKey, F &&d2d3)
            // {
            //     static_assert(!std::is_same_v<K, K>, "unsuported cell key type.");
            // }

            // template <typename F>
            // static Vector3 transform3(K &cKey, Point2<float> pointInCell, float h, F &&d2d3)
            // {
            //     static_assert(!std::is_same_v<K, K>, "unsuported cell key type.");
            // }
        };

        template <>
        struct TransformDispatcher<Offset>
        {

            // template <typename F>
            // static Vector3 transform3(Offset &cKey, F &&d2d3)
            // {
            //     static Transform::CellKeyToCentre TF_K2C;
            //     return cKey.cast<float>().transform(TF_K2C).transform3(d2d3);
            // }

            // template <typename F>
            // static Vector3 transform3(Offset &cKey, Point2<float> pointInCell, float h, F &&d2d3)
            // {
            //     static Transform::CellKeyToCentre TF_K2C{};
            //     return (cKey.cast<float>().transform(TF_K2C) + pointInCell).transform3(h, d2d3);
            // }
        };

        template <>
        struct TransformDispatcher<Axial>
        {

            // template <typename F>
            // static Vector3 transform3(Axial &cKey, F &&d2d3)
            // {
            //     throw std::runtime_error("TODO");
            // }

            // template <typename F>
            // static Vector3 transform3(Axial &cKey, Point2<float> pointInCell, float h, F &&d2d3)
            // {
            //     throw std::runtime_error("TODO");
            // }
        };

        // transform methods

        // static Transform::CentreToCellKey TF_C2K;
        // static Transform::CellKeyToCentre TF_K2C;

        // template <typename K, typename F>
        // static Vector3 transform3(K &cKey, F &&d2d3)
        // {
        //     return TransformDispatcher<K>::transform3(cKey, d2d3);
        // }
        //

        // template <typename K>
        // static void getCentres(std::vector<K> &cks, std::vector<Point2<float>> &ret)
        // {

        //     CollectionUtil::transform<K, Point2<float>>(cks, ret, [](K &ck)
        //                                                 {
        //                                                 //    return CellKey::transform<CellKey::OC>(ck); //
        //                                                 return CellKey::transform<CellKey::OC>(ck); });

        // }

        //
        // template <typename K, typename F>
        // static Vector3 transform3(K &cKey, Point2<float> pointInCell, float h, F &&d2d3)
        // {
        //     return TransformDispatcher<K>::transform3(cKey, pointInCell, h, d2d3);
        // }

        // //
        // template <typename K>
        // static K from(Point2<float> p)
        // {
        //     static_assert(sizeof(T) == 0, "unsupported cell key type.");
        // };

        // template <>
        // static Offset from<Offset>(Point2<float> p)
        // {
        //     static Transform::CentreToCellKey TF_C2K;
        //     return p.transform(TF_C2K);
        // };

        // template <>
        // static Axial from<Axial>(Point2<float> p)
        // {
        //     throw std::runtime_error("TODO");
        // };

        // transform from key to centre

        // template <typename K>
        // static Point2<float> getCentre(K &cKey)
        // {
        //     static_assert(sizeof(T) == 0, "unsupported cell key type.");
        // }

        // template <>
        // static Point2<float> getCentre<Offset>(Offset &cKey)
        // {
        //     static Transform::CellKeyToCentre TF_K2C;
        //     return cKey.Point2<int>::cast<float>().transform(TF_K2C);
        // }

        // template <>
        // static Point2<float> getCentre<Axial>(Axial &cKey)
        // {
        //     throw std::runtime_error("TODO");
        // }
        // //

        template <typename K>
        Box2<float> getBoxIn2D(K &cKey)
        {
            // Vector2 centre = getOrigin2D(cKey);
            Point2<float> centre = CellKey::transform<K, CellKey::Centre>(cKey);
            float left = centre.x - 1.0; // rad
            float bottom = centre.y - std::sqrt(3.0) / 2.0;
            return Box2<float>(left, bottom, left + 2.0, bottom + std::sqrt(3.0));
        }

        template <typename K>
        static Box2<float> getOuterBoxIn2D(K &cKey)
        {
            // Vector2 centre = getOrigin2D(cKey);

            Point2<float> centre = CellKey::transform<K, CellKey::Centre>(cKey);

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
        template <typename Tuple>
        struct tupleTail;
        template <typename Head, typename... Tail>
        struct tupleTail<std::tuple<Head, Tail...>>
        {
            using type = std::tuple<Tail...>;
        };

        template <typename Tuple>
        struct tupleHead;
        template <typename Head, typename... Tail>
        struct tupleHead<std::tuple<Head, Tail...>>
        {
            using type = Head;
        };

        //
        template <typename Tuple>
        static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 2, std::vector<std::tuple_element_t<1, Tuple>>>
        transformAll(std::vector<std::tuple_element_t<0, Tuple>> &cks)
        {
            using K1 = std::tuple_element_t<0, Tuple>;
            using K2 = std::tuple_element_t<1, Tuple>;
            std::vector<K2> ret;

            CollectionUtil::transform<K1, K2>(cks, ret, [](K &ck)
                                              { return CellKey::transform<Tuple>(ck); });
            return ret;
        }

        template <typename Tuple>
        static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 3, std::vector<std::tuple_element_t<1, Tuple>>>
        transformAll(std::vector<std::tuple_element_t<0, Tuple>> &cks)
        {
            using K1 = std::tuple_element_t<0, Tuple>;
            using K3 = std::tuple_element_t<2, Tuple>;
            using K2 = std::tuple_element_t<1, Tuple>;

            std::vector<K3> cks3;

            CollectionUtil::transform<K1, K3>(cks, cks3, [](K1 &ck)
                                              { return CellKey::transform<K1, K3>(ck); });

            std::vector<K2> ret;
            CollectionUtil::transform<K3, K2>(cks3, ret, [](K3 &ck)
                                              { return CellKey::transform<K3, K2>(ck); });
            return ret;
        }

        // transform tuple easy to call.
        template <typename Tuple>
        static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 2, std::tuple_element_t<1, Tuple>>
        transform(const std::tuple_element_t<0, Tuple> &cKey1)
        {
            using K1 = std::tuple_element_t<0, Tuple>;
            using K2 = std::tuple_element_t<1, Tuple>;
            return transform<K1, K2>(cKey1);
        }

        template <typename Tuple>
        static typename std::enable_if_t<isTuple<Tuple>::value && std::tuple_size_v<Tuple> == 3, std::tuple_element_t<1, Tuple>>
        transform(const std::tuple_element_t<0, Tuple> &cKey1)
        {
            using K1 = std::tuple_element_t<0, Tuple>;
            using K3 = std::tuple_element_t<2, Tuple>;
            using K2 = std::tuple_element_t<1, Tuple>;

            K3 k3 = transform<K1, K3>(cKey1);
            return transform<K3, K2>(k3);
        }

        // transform impl
        template <typename K1, typename K2>
        static K2 transform(const K1 &cKey1)
        {
            static_assert(!std::is_same(K1, K1), "not supported transform between types.");
        }

        template <>
        static Centre transform<Offset, Centre>(const Offset &cKey1)
        {
            static Transform::CellKeyToCentre TF_K2C;

            return Point2<float>(cKey1.x, cKey1.y).transform(TF_K2C);
        }

        template <>
        static Offset transform<Centre, Offset>(const Centre &cKey1)
        {
            static Transform::CentreToCellKey TF_C2K;
            return Point2<float>(cKey1.x, cKey1.y).transform(TF_C2K);
        }

        template <>
        static Point2<float> transform<Centre, Point2<float>>(const Centre &cKey1)
        {
            return Point2<float>(cKey1.x, cKey1.y);
        }
    };

};
