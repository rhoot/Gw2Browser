/** \file       Util/Vector3.h
 *  \brief      Contains the declaration of the 3D vector class.
 *  \author     Rhoot
 */

/*	Copyright (C) 2012 Rhoot <https://github.com/rhoot>

    This file is part of Gw2Browser.

    Gw2Browser is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef UTIL_VECTOR3_H_INCLUDED
#define UTIL_VECTOR3_H_INCLUDED

namespace gw2b
{
    class Vector3
    {
    public:
        float x;
        float y;
        float z;

    public:
        /** Vector with all 3 components set to zero. */
        static const Vector3 zero;
        /** Vector with all 3 components set to one. */
        static const Vector3 one;

    public:
        Vector3();
        Vector3(float pVal);
        Vector3(float pX, float pY, float pZ);
        Vector3(const Vector3& pOther);

        void Clear();
        void Set(const Vector3& pOther);
        void Set(float pX, float pY, float pZ);

        float Magnitude() const;
        float MagnitudeSquare() const;
        void Normalize();
        Vector3 GetNormalized() const;

        Vector3& operator=(float pVal);
        Vector3& operator=(const Vector3& pOther);

        Vector3 operator-() const;
        Vector3 operator+(float pVal) const;
        Vector3 operator+(const Vector3& pOther) const;
        Vector3 operator-(float pVal) const;
        Vector3 operator-(const Vector3& pOther) const;
        Vector3 operator*(float pVal) const;
        Vector3 operator*(const Vector3& pOther) const;
        Vector3 operator/(float pVal) const;
        Vector3 operator/(const Vector3& pOther) const;

        Vector3& operator+=(float pVal);
        Vector3& operator+=(const Vector3& pOther);
        Vector3& operator-=(float pVal);
        Vector3& operator-=(const Vector3& pOther);
        Vector3& operator*=(float pVal);
        Vector3& operator*=(const Vector3& pOther);
        Vector3& operator/=(float pVal);
        Vector3& operator/=(const Vector3& pOther);

        bool operator==(const Vector3& pOther) const;
        bool operator!=(const Vector3& pOther) const;

        static float Dot(const Vector3& a, const Vector3& b);
        static inline Vector3 Cross(const Vector3& a, const Vector3& b);
    };

}; // namespace gw2b

#include "Vector3.inl"

#endif // UTIL_VECTOR3_H_INCLUDED
