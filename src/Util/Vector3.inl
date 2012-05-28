/** \file       Util/Vector3.inl
 *  \brief      Contains the inline definition of the 3D vector class.
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

#ifdef UTIL_VECTOR3_H_INCLUDED

namespace gw2b
{

inline Vector3::Vector3() 
    : x(0)
    , y(0)
    , z(0)
{
}

inline Vector3::Vector3(float pVal)
    : x(pVal)
    , y(pVal)
    , z(pVal)
{
}

inline Vector3::Vector3(float pX, float pY, float pZ)
    : x(pX)
    , y(pY)
    , z(pZ)
{
}

inline Vector3::Vector3(const Vector3& pOther)
    : x(pOther.x)
    , y(pOther.y)
    , z(pOther.z)
{
}

inline void Vector3::Clear()
{
    ::memset(this, 0, sizeof(*this));
}

inline void Vector3::Set(const Vector3& pOther)
{
    this->Set(pOther.x, pOther.y, pOther.z);
}

inline void Vector3::Set(float pX, float pY, float pZ)
{
    x = pX;
    y = pY;
    z = pZ;
}

inline float Vector3::Magnitude() const
{
    return ::sqrt((x*x)+(y*y)+(z*z));
}

inline float Vector3::MagnitudeSquare() const
{
    return (x*x)+(y*y)+(z*z);
}

inline void Vector3::Normalize()
{
    if ((x * y * z) == 0) {
        x = 1;
    } else {
        *this /= Magnitude();
    }
}

inline Vector3 Vector3::GetNormalized() const
{
    Vector3 v(*this);
    v.Normalize();
    return v;
}

inline Vector3& Vector3::operator=(float pVal)
{
    this->Set(pVal, pVal, pVal);
    return *this;
}

inline Vector3& Vector3::operator=(const Vector3& pOther)
{
    this->Set(pOther);
    return *this;
}

inline Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator+(float pVal) const
{
    return Vector3(x + pVal, y + pVal, z + pVal);
}

inline Vector3 Vector3::operator+(const Vector3& pOther) const
{
    return Vector3(x + pOther.x, y + pOther.y, z + pOther.z);
}

inline Vector3 Vector3::operator-(float pVal) const
{
    return Vector3(x - pVal, y - pVal, z - pVal);
}

inline Vector3 Vector3::operator-(const Vector3& pOther) const
{
    return Vector3(x - pOther.x, y - pOther.y, z - pOther.z);
}

inline Vector3 Vector3::operator*(float pVal) const
{
    return Vector3(x * pVal, y * pVal, z * pVal);
}

inline Vector3 Vector3::operator*(const Vector3& pOther) const
{
    return Vector3(x * pOther.x, y * pOther.y, z * pOther.z);
}

inline Vector3 Vector3::operator/(float pVal) const
{
    float f = 1.0f / pVal;
    return Vector3(x * f, y * f, z * f);
}

inline Vector3 Vector3::operator/(const Vector3& pOther) const
{
    return Vector3(x / pOther.x, y / pOther.y, z / pOther.z);
}

inline Vector3& Vector3::operator+=(float pVal)
{
    x += pVal;
    y += pVal;
    z += pVal;
    return *this;
}

inline Vector3& Vector3::operator+=(const Vector3& pOther)
{
    x += pOther.x;
    y += pOther.y;
    z += pOther.z;
    return *this;
}

inline Vector3& Vector3::operator-=(float pVal)
{
    x -= pVal;
    y -= pVal;
    z -= pVal;
    return *this;
}

inline Vector3& Vector3::operator-=(const Vector3& pOther)
{
    x -= pOther.x;
    y -= pOther.y;
    z -= pOther.z;
    return *this;
}

inline Vector3& Vector3::operator*=(float pVal)
{
    x *= pVal;
    y *= pVal;
    z *= pVal;
    return *this;
}

inline Vector3& Vector3::operator*=(const Vector3& pOther)
{
    x *= pOther.x;
    y *= pOther.y;
    z *= pOther.z;
    return *this;
}

inline Vector3& Vector3::operator/=(float pVal)
{
    float f = 1.0f / pVal;
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

inline Vector3& Vector3::operator/=(const Vector3& pOther)
{
    x /= pOther.x;
    y /= pOther.y;
    z /= pOther.z;
    return *this;
}

inline bool Vector3::operator==(const Vector3& pOther) const
{
    return (x == pOther.x && y == pOther.y && z == pOther.z);
}

inline bool Vector3::operator!=(const Vector3& pOther) const
{
    return (x != pOther.x || y != pOther.y || z != pOther.z);
}

inline float Vector3::Dot(const Vector3& pA, const Vector3& pB)
{
    return ((pA.x * pB.x) + (pA.y * pB.y) + (pA.z * pB.z));
}

inline Vector3 Vector3::Cross(const Vector3& pA, const Vector3& pB)
{
    return Vector3(((pA.y * pB.z) - (pA.z * pB.y)), ((pA.z * pB.x) - (pA.x * pB.z)), ((pA.x * pB.y) - (pA.y * pB.x)));
}

};

#endif // UTIL_VECTOR3_H_INCLUDED
