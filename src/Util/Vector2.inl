/** \file       Util/Vector2.inl
 *  \brief      Contains the inline definition of the 2D vector class.
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

#ifdef UTIL_VECTOR2_H_INCLUDED

namespace gw2b
{

inline Vector2::Vector2() 
    : x(0)
    , y(0)
{
}

inline Vector2::Vector2(float pVal)
    : x(pVal)
    , y(pVal)
{
}

inline Vector2::Vector2(float pX, float pY)
    : x(pX)
    , y(pY)
{
}

inline Vector2::Vector2(const Vector2& pOther)
    : x(pOther.x)
    , y(pOther.y)
{
}

inline void Vector2::Clear()
{
    ::memset(this, 0, sizeof(*this));
}

inline void Vector2::Set(const Vector2& pOther)
{
    this->Set(pOther.x, pOther.y);
}

inline void Vector2::Set(float pX, float pY)
{
    x = pX;
    y = pY;
}

inline float Vector2::Magnitude() const
{
    return ::sqrt((x*x)+(y*y));
}

inline float Vector2::MagnitudeSquare() const
{
    return (x*x)+(y*y);
}

inline Vector2& Vector2::operator=(float pVal)
{
    this->Set(pVal, pVal);
    return *this;
}

inline Vector2& Vector2::operator=(const Vector2& pOther)
{
    this->Set(pOther);
    return *this;
}

inline Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y);
}

inline Vector2 Vector2::operator+(float pVal) const
{
    return Vector2(x + pVal, y + pVal);
}

inline Vector2 Vector2::operator+(const Vector2& pOther) const
{
    return Vector2(x + pOther.x, y + pOther.y);
}

inline Vector2 Vector2::operator-(float pVal) const
{
    return Vector2(x - pVal, y - pVal);
}

inline Vector2 Vector2::operator-(const Vector2& pOther) const
{
    return Vector2(x - pOther.x, y - pOther.y);
}

inline Vector2 Vector2::operator*(float pVal) const
{
    return Vector2(x * pVal, y * pVal);
}

inline Vector2 Vector2::operator*(const Vector2& pOther) const
{
    return Vector2(x * pOther.x, y * pOther.y);
}

inline Vector2 Vector2::operator/(float pVal) const
{
    float f = 1.0f / pVal;
    return Vector2(x * f, y * f);
}

inline Vector2 Vector2::operator/(const Vector2& pOther) const
{
    return Vector2(x / pOther.x, y / pOther.y);
}

inline Vector2& Vector2::operator+=(float pVal)
{
    x += pVal;
    y += pVal;
    return *this;
}

inline Vector2& Vector2::operator+=(const Vector2& pOther)
{
    x += pOther.x;
    y += pOther.y;
    return *this;
}

inline Vector2& Vector2::operator-=(float pVal)
{
    x -= pVal;
    y -= pVal;
    return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& pOther)
{
    x -= pOther.x;
    y -= pOther.y;
    return *this;
}

inline Vector2& Vector2::operator*=(float pVal)
{
    x *= pVal;
    y *= pVal;
    return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& pOther)
{
    x *= pOther.x;
    y *= pOther.y;
    return *this;
}

inline Vector2& Vector2::operator/=(float pVal)
{
    float f = 1.0f / pVal;
    x *= f;
    y *= f;
    return *this;
}

inline Vector2& Vector2::operator/=(const Vector2& pOther)
{
    x /= pOther.x;
    y /= pOther.y;
    return *this;
}

inline bool Vector2::operator==(const Vector2& pOther) const
{
    return (x == pOther.x && y == pOther.y);
}

inline bool Vector2::operator!=(const Vector2& pOther) const
{
    return (x != pOther.x || y != pOther.y);
}

};

#endif // UTIL_Vector2_H_INCLUDED
