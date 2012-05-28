/** \file       Util/Vector2.h
 *  \brief      Contains the declaration of the 2D vector class.
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

#ifndef UTIL_VECTOR2_H_INCLUDED
#define UTIL_VECTOR2_H_INCLUDED

namespace gw2b
{
    class Vector2
    {
    public:
        float x;
        float y;

    public:
        /** Vector with all 3 components set to zero. */
        static const Vector2 zero;
        /** Vector with all 3 components set to one. */
        static const Vector2 one;

    public:
        Vector2();
        Vector2(float pVal);
        Vector2(float pX, float pY);
        Vector2(const Vector2& pOther);

        void Clear();
        void Set(const Vector2& pOther);
        void Set(float pX, float pY);

        float Magnitude() const;
        float MagnitudeSquare() const;

        Vector2& operator=(float pVal);
        Vector2& operator=(const Vector2& pOther);

        Vector2 operator-() const;
        Vector2 operator+(float pVal) const;
        Vector2 operator+(const Vector2& pOther) const;
        Vector2 operator-(float pVal) const;
        Vector2 operator-(const Vector2& pOther) const;
        Vector2 operator*(float pVal) const;
        Vector2 operator*(const Vector2& pOther) const;
        Vector2 operator/(float pVal) const;
        Vector2 operator/(const Vector2& pOther) const;

        Vector2& operator+=(float pVal);
        Vector2& operator+=(const Vector2& pOther);
        Vector2& operator-=(float pVal);
        Vector2& operator-=(const Vector2& pOther);
        Vector2& operator*=(float pVal);
        Vector2& operator*=(const Vector2& pOther);
        Vector2& operator/=(float pVal);
        Vector2& operator/=(const Vector2& pOther);

        bool operator==(const Vector2& pOther) const;
        bool operator!=(const Vector2& pOther) const;

        static float Dot(const Vector2& a, const Vector2& b);
    };

}; // namespace gw2b

#include "Vector2.inl"

#endif // UTIL_VECTOR2_H_INCLUDED
