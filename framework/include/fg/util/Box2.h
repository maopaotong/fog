/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util/Point2.h"
namespace fog
{
    using namespace Ogre;

    template <typename T>
    class Box2
    {
    public:
        Point2<T> p1;
        Point2<T> p2;

    public:
        Box2() = default;

        Box2(T w) : Box2(w, w)
        {
        }

        Box2(T w, T h) : Box2(0, 0, w, h)
        {
        }

        Box2(T x1, T y1, T x2, T y2) : Box2({x1, y1}, {x2, y2})
        {
        }
        Box2(Point2<T> p1, Point2<T> p2) : p1(p1), p2(p2)
        {
        }
        Box2(Point2<T> p1, T width, T height) : p1(p1), p2(p1.x + width, p1.y + height)
        {
        }

        Box2(const Box2 &) = default;
        Box2(Box2 &&) = default;
        Box2 &operator=(const Box2 &) = default;
        Box2 &operator=(Box2 &&) = default;
        ~Box2() = default;

        void moveCentreTo(const Point2<T> &p)
        {
            T w = this->getWidth();
            T h = this->getHeight();

            this->p1 = p - Point2<T>(w / 2, h / 2);
            this->p2 = p1 + Point2<T>(w, h);
        }

        Point2<T> getCentre()
        {
            return (p1 + p2) / 2.0;
        }

        void expand(T scale)
        {
            T w = this->getWidth();
            T h = this->getHeight();
            T halfW2 = w * scale / 2;
            T halfH2 = h * scale / 2;
            Point2<T> c = this->getCentre();
            this->p1 = c - Point2<T>(halfW2, halfH2);
            this->p2 = c + Point2<T>(halfW2, halfH2);
        }

        bool isIn(T x, T y)
        {
            return x >= p1.x && x < p2.x && y >= p1.y && y < p2.y;
        }

        void scale(T s)
        {
            scale(s, s);
        }
        void scale(T sx, T sy)
        {
            this->p1.x *= sx;
            this->p1.y *= sy;
            this->p2.x *= sx;
            this->p2.y *= sy;
        }

        T getWidth()
        {
            return p2.x - p1.x;
        }

        T getHeight()
        {
            return p2.y - p1.y;
        }

        bool exists()
        {
            return p1.x < p2.x || p1.y < p2.y;
        }

        static Box2<T> intersection(const Box2<T> &r1, const Box2<T> &r2)
        {
            Point2<T> tp1(
                std::max(r1.p1.x, r2.p1.x),
                std::max(r1.p1.y, r2.p1.y));

            Point2<T> tp2(
                std::min(r1.p2.x, r2.p2.x),
                std::min(r1.p2.y, r2.p2.y));
            //
            return {tp1, tp2};
        }

        template <typename... F>
        Box2<T> transform(F &&...funcs)
        {
            Point2<T> p1 = this->p1.transform(funcs...);
            Point2<T> p2 = this->p2.transform(funcs...);
            return Box2<T>(p1, p2);
        }

        template <typename T2>
        Box2<T2> cast()
        {
            return Box2<T2>(p1.cast<T2>(), p2.cast<T2>());
        }
    };
}; // end of namespace
