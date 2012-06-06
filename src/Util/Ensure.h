/** \file       Util/Ensure.h
 *  \brief      Contains some common Asserts.
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
 *  \param[in]  p_object     Object to verify type of.
 *  \tparam     T           Type the object should be castable to. */
template <typename T, typename TPtr>
    void isOfType(TPtr* p_object)
{
    Assert(dynamic_cast<T*>(p_object) != nullptr);
}

/** Asserts that the given pointer is not nullptr. 
 *  \param[in]  p_pointer    Pointer to check. */
template <typename T>
    void notNull(T* p_pointer)
{
    Assert(p_pointer != nullptr);
}

/** Asserts that the given pointer is nullptr.
 *  \param[in]  p_pointer    Pointer to check. */
template <typename T>
    void isNull(T* p_pointer)
{
    Assert(p_pointer == nullptr);
}

}; // namespace Ensure

}; // namespace gw2b

#endif // UTIL_ENSURE_H_INCLUDED
