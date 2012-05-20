/** \file       Util/Ensure.h
 *  \brief      Contains some common asserts.
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

#ifndef UTIL_ENSURE_H_INCLUDED
#define UTIL_ENSURE_H_INCLUDED

namespace gw2b
{

namespace Ensure
{

/** Asserts that the given object is castable to the given type.
 *  \param[in]  pObject     Object to verify type of.
 *  \tparam     T           Type the object should be castable to. */
template <typename T, typename TPtr>
    void IsOfType(TPtr* pObject)
{
    assert(dynamic_cast<T*>(pObject) != NULL);
}

/** Asserts that the given pointer is not NULL. 
 *  \param[in]  pPointer    Pointer to check. */
template <typename T>
    void NotNull(T* pPointer)
{
    assert(pPointer != NULL);
}

/** Asserts that the given pointer is NULL.
 *  \param[in]  pPointer    Pointer to check. */
template <typename T>
    void IsNull(T* pPointer)
{
    assert(pPointer == NULL);
}

}; // namespace Ensure

}; // namespace gw2b

#endif // UTIL_ENSURE_H_INCLUDED
