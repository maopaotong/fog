/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "Common.h"
#include <OgreVector.h>
namespace fog
{
    template <typename T>
    struct Point3;

    using Vector3 = Point3<float>;

    template <typename T>
    struct Point3
    {

        T x;
        T y;
        T z;

        Point3() : x(0), y(0), z(0)
        {
        }
        Point3(T x, T y, T z) : x(x), y(y), z(z) {}

        Point3(Ogre::Vector3 v3) : x(v3.x), y(v3.y), z(v3.z)
        {
        }

        Point3<T> &scale(T s)
        {
            return scale(s, s, s);
        }
        Point3<T> &scale(T sx, T sy, T sz)
        {
            this->x *= sx;
            this->y *= sy;
            this->z *= sz;
            return *this;
        }

        Point3<T> operator*(const T &s) const
        {
            return Point3<T>{x * s, y * s, z * s};
        }

        Point3<T> operator+(const Point3<T> &p2) const
        {
            return Point3<T>{x + p2.x, y + p2.y, z + p2.z};
        }

        Point3<T> operator-(const Point3<T> &p2) const
        {
            return Point3<T>{x - p2.x, y - p2.y, z - p2.z};
        }

        Point3<T> &operator-()
        {
            x = -x;
            y = -y;
            z = -z;
            return *this;
        }

        Point3<T> operator/(const T &s) const
        {
            return Point3<T>{x / s, y / s, z / s};
        }

        Point3<T> operator/=(const T &s) const
        {
            x /= s;
            y /= s;
            z /= s;
            return *this;
        }

        Point3<T> operator+=(const Point3<T> &p)
        {
            this->x += p.x;
            this->y += p.y;
            this->z += p.z;
            return *this;
        }

        bool operator!=(const Point3<T> &p2) const
        {
            return x != p2.x || y != p2.y || z != p2.z;
        }

        bool operator==(const Point3<T> &p2) const
        {
            return x == p2.x && y == p2.y && z == p2.z;
        }

        bool sameSign(T a, T b)
        {
            return (a ^ b) >= 0 && (b ^ z) >= 0;
        }

        bool sameSign(Point3<T> v2)
        {
            return sameSign(x, v2.x) && sameSign(y, v2.y) && sameSign(z, v2.z);
        }

        float sqrLength()
        {
            return x * x + y * y + z * z;
        }

        float length()
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        float distance(Point3<T> p2)
        {
            return (*this - p2).length();
        }

        template <typename T2>
        Point3<T2> cast()
        {
            return Point3<T2>(static_cast<T2>(x), static_cast<T2>(y), static_cast<T2>(z));
        }

        template <typename... F>
        Point3<T> transform(F &&...funcs)
        {
            T x = this->x;
            T y = this->y;
            T z = this->z;
            (funcs(x, y, z), ...);
            return Point3<T>(x, y, z);
        }

        operator Ogre::Vector3() const
        {
            return Ogre::Vector3(x, y, z);
        }

        void normalise()
        {
            Ogre::Vector3 ov(x, y, z);
            ov.normalise();
            this->x = ov.x;
            this->y = ov.y;
            this->z = ov.z;
        }
    };

}
