/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Common.h"
namespace fog
{

    template <typename T>
    struct Point2
    {
        T x;
        T y;
        Point2() : x(0), y(0)
        {
        }
        Point2(T x, T y) : x(x), y(y) {}
        Point2(Ogre::Vector2 v2) : Point2(v2.x, v2.y)
        {
        }

        Point2<T> &scale(T s)
        {
            return scale(s, s);
        }
        Point2<T> &scale(T sx, T sy)
        {
            this->x *= sx;
            this->y *= sy;
            return *this;
        }

        Point2<T> operator*(const T& s) const
        {
            return Point2{x * s, y * s};
        }

        Point2<T> operator+(const Point2<T> &p2) const
        {
            return Point2{x + p2.x, y + p2.y};
        }

        Point2<T> operator-(const Point2<T> &p2) const
        {
            return Point2<T>{x - p2.x, y - p2.y};
        }

        Point2<T> operator/(const T& s) const
        {
            return Point2<T>{x / s, y / s};
        }

        Point2<T> operator/=(const T &s) const
        {
            x /= s;
            y /= s;
            return *this;
        }

        Point2<T> operator+=(const Point2<T> &p) 
        {
            this->x+=p.x;
            this->y+=p.y;
            return *this;
        }

        bool operator!=(const Point2<T> &p2) const
        {
            return x != p2.x || y != p2.y;
        }

        bool operator==(const Point2<T> &p2) const
        {
            return x == p2.x && y == p2.y;
        }

        bool sameSign(T a, T b)
        {
            return (a ^ b) >= 0;
        }

        bool sameSign(Point2<T> v2)
        {
            return sameSign(x, v2.x) && sameSign(y, v2.y);
        }

        float sqrLength(){
            return x * x + y * y;
        }
        
        float length()
        {
            return std::sqrt(x * x + y * y);
        }

        float distance(Point2<T> p2)
        {
            return (*this - p2).length();
        }

        template <typename T2>
        Point2<T2> cast()
        {
            return Point2<T2>(static_cast<T2>(x), static_cast<T2>(y));
        }

        template <typename... F>
        Point2<T> transform(F &&...funcs)
        {
            T x = this->x;
            T y = this->y;
            (funcs(x, y), ...);
            return Point2<T>(x, y);
        }

        template <typename... F>
        Ogre::Vector3 transform3(float height, F &&...funcs)
        {
            // TODO
            Ogre::Vector3 ret = transform3(funcs...);
            ret.y = height;
            return ret;
        }

        template <typename... F>
        Ogre::Vector3 transform3(F &&...funcs)
        {
            T x = this->x;
            T y = this->y;
            T z = T{};
            (funcs(x, y, z), ...);
            return Ogre::Vector3(x, y, z);
        }

        template <typename... F>
        static Point2<T> from(Ogre::Vector3 pos, F &&...funcs)
        {
            (funcs(pos.x, pos.y, pos.z), ...);
            return Point2<T>(pos.x, pos.y);
        }

        operator Ogre::Vector2() const
        {
            return Ogre::Vector2(x, y);
        }

        void normalise()
        {
            Ogre::Vector2 ov(x, y);
            ov.normalise();
            this->x = ov.x;
            this->y = ov.y;
        }
    };

}
