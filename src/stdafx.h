/** \file       stdafx.h
 *  \brief      Pre-compiled header.
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

#ifndef STDAFX_H_INCLUDED
#define STDAFX_H_INCLUDED

// wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#  include <wx/wx.h>
#endif

// gw2CoreTools
#include <gw2DatTools/compression/inflateBuffer.h>

// Handy defines
#define ArraySize(x)        (sizeof(x) / sizeof(*x))

// Compiler specific
#ifdef _MSC_VER
#  define NakedCall           __declspec(naked)
#  define InlineAsm           __asm
#  define ZeroSizeArray       1
#else
#  error  Compiler not yet supported in 'stdafx.h'.
#endif

namespace gw2b
{
    typedef wxInt8          int8;       /**< Signed 8-bit integer. */  
    typedef wxInt16         int16;      /**< Signed 16-bit integer. */
    typedef wxInt32         int32;      /**< Signed 32-bit integer. */
    typedef wxInt64         int64;      /**< Signed 64-bit integer. */

    typedef wxUint8         uint8;      /**< Unsigned 8-bit integer. */
    typedef wxUint16        uint16;     /**< Unsigned 16-bit integer. */
    typedef wxUint32        uint32;     /**< Unsigned 32-bit integer. */
    typedef wxUint64        uint64;     /**< Unsigned 64-bit integer. */

    typedef uint8           byte;       /**< Unsigned byte. */
    typedef int8            sbyte;      /**< Signed byte. */
    typedef char            char8;      /**< 8-bit character. */    
    typedef wxChar16        char16;     /**< 16-bit character. */
    typedef wxChar32        char32;     /**< 32-bit character. */

    typedef unsigned short  ushort;     /**< Short for 'unsigned short'. */
    typedef unsigned int    uint;       /**< Short for 'unsigned int'. */
    typedef unsigned long   ulong;      /**< Short for 'unsigned long'. */
}; // namespace gw2b

#include "Util/Array.h"
#include "Util/AutoPtr.h"
#include "Util/Delegate.h"
#include "Util/Ensure.h"

namespace gw2b
{
    /** Frees the given pointer (allocated by malloc) and sets it to NULL.
     *  \param[in,out]  pPointer    Pointer to free and set to NULL.
     *  \tparam T           Type of the pointer. */
    template <typename T>
        void FreePointer(T*& pPointer)
    {
        ::free(pPointer);
        pPointer = NULL;
    }

    /** Frees the given pointer (allocated by new) and sets it to NULL.
     *  \param[in,out]  pPointer    Pointer to free and set to NULL.
     *  \tparam T           Type of the pointer. */
    template <typename T>
        void DeletePointer(T*& pPointer)
    {
        delete pPointer;
        pPointer = NULL;
    }

    /** Frees the given pointer (allocated by new[]) and sets it to NULL.
     *  \param[in,out]  pPointer    Pointer to free and set to NULL.
     *  \tparam T           Type of the pointer. */
    template <typename T>
        void DeleteArray(T*& pArray)
    {
        delete[] pArray;
        pArray = NULL;
    }

    /** Swaps values of the two provided variables.
     *  \param[in,out]  pItem1  First item.
     *  \param[in,out]  pItem2  Second item.
     *  \tparam T   Type of the items. */
    template <typename T>
        void Swap(T& pItem1, T& pItem2)
    {
        T temp = pItem1;
        pItem1 = pItem2;
        pItem2 = temp;
    }

    /** Allocates memory to hold the given amount of elements, using malloc.
     *  \param[in]  pCount  amount of elements needed.
     *  \tparam     T       type of elements to allocate memory for.
     *  \return T*  pointer to newly allocated data. */
    template <typename T>
        T* Alloc(uint pCount)
    {
        return (T*)::malloc(pCount * sizeof(T));
    }

    /** Determines whether the given value is a power of two.
     *  \param[in]  pValue  Value to check.
     *  \tparam     T       Type of value.
     *  \return bool    true if power of two, false if not. */
    template <typename T>
        bool IsPowerOfTwo(T pValue)
    {
        return !(pValue & (pValue - 1));
    }

}; // namespace gw2b

#endif // STDAFX_H_INCLUDED
