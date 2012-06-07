/** \file       Util/Misc.h
 *  \brief      Contains declarations for some helper functions that don't fit
 *              anywhere else.
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

#ifndef UTIL_MISC_H_INCLUDED
#define UTIL_MISC_H_INCLUDED

namespace gw2b
{

/** Frees the given pointer (allocated by malloc) and sets it to nullptr.
    *  \param[in,out]   po_pointer  Pointer to free and set to nullptr.
    *  \tparam          T           Type of the pointer. */
template <typename T>
    void freePointer(T*& po_pointer)
{
    ::free(po_pointer);
    po_pointer = nullptr;
}

//============================================================================/

/** Frees the given pointer (allocated by new) and sets it to nullptr.
    *  \param[in,out]   po_pointer  Pointer to free and set to nullptr.
    *  \tparam          T           Type of the pointer. */
template <typename T>
    void deletePointer(T*& po_pointer)
{
    delete po_pointer;
    po_pointer = nullptr;
}

//============================================================================/

/** Frees the given pointer (allocated by new[]) and sets it to nullptr.
    *  \param[in,out]   po_array    Pointer to free and set to nullptr.
    *  \tparam          T           Type of the pointer. */
template <typename T>
    void deleteArray(T*& po_array)
{
    delete[] po_array;
    po_array = nullptr;
}

//============================================================================/

/** Calls Release() on the given pointer, as required by DirectX. This to 
 *  ensure it is being released properly. Also sets the given pointer to 
 *  nullptr.
 *  \param[in]  po_pointer      Object to release and set to nullptr.
 *  \tparam     T               Type of the pointer. */
template <typename T>
    void releasePointer(T*& po_pointer)
{
    if (po_pointer) {
        po_pointer->Release();
        po_pointer = nullptr;
    }
}

//============================================================================/

/** Allocates memory to hold the given amount of elements, using malloc.
 *  \param[in]  p_count amount of elements needed.
 *  \tparam     T       type of elements to allocate memory for.
 *  \return     T*      pointer to newly allocated data. */
template <typename T>
    T* allocate(uint p_count)
{
    return static_cast<T*>(::malloc(p_count * sizeof(T)));
}

//============================================================================/

/** Determines whether the given value is a power of two.
 *  \param[in]  p_value Value to check.
 *  \tparam     T       Type of value.
 *  \return bool        true if power of two, false if not. */
template <typename T>
    bool isPowerOfTwo(T p_value)
{
    return !(p_value & (p_value - 1));
}

//============================================================================/

/** Determines whether the program was compiled in debug mode. Returns
 *  p_true if it was, p_false if it wasn't.
 *  \param[in]  p_true   Value to return if in debug mode.
 *  \param[in]  p_false  Value to return if not in debug mode. */
template <typename T>
    T ifDebug(T p_true, T p_false)
{
#ifdef _DEBUG
    return p_true;
#else
    return p_false;
#endif
}

//============================================================================/

/** Determines the index of the least significant bit that is set.
    *  \param[in]  p_value  Value to find the lowest set bit of.
    *  \return uint    Zero-based index for the lowest set bit. */
uint lowestSetBit(uint32 p_value);

//============================================================================/

/** Determines the amount of set bits.
 *  \param[in] p_value  Value to count bits in. 
 *  \return uint    Amount of bits set. */
uint numSetBits(uint32 p_value);

}; // namespace gw2b

#endif // UTIL_MISC_H_INCLUDED
