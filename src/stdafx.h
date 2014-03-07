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

// STL includes
#include <memory>

// OpenMP
#include <omp.h>

// wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

// gw2CoreTools
#include <gw2DatTools/compression/inflateDatFileBuffer.h>
#include <gw2DatTools/compression/inflateTextureFileBuffer.h>

// OpenGL
#include <GL/glew.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// 16-bit floats
#include "Imported/half.h"

// Handy defines
#define ArraySize(x)                (sizeof(x) / sizeof(*x))
#define Assert                      wxASSERT

namespace gw2b
{
    typedef int8_t                  int8;       /**< Signed 8-bit integer. */  
    typedef int16_t                 int16;      /**< Signed 16-bit integer. */
    typedef int32_t                 int32;      /**< Signed 32-bit integer. */
    typedef int64_t                 int64;      /**< Signed 64-bit integer. */

    typedef uint8_t                 uint8;      /**< Unsigned 8-bit integer. */
    typedef uint16_t                uint16;     /**< Unsigned 16-bit integer. */
    typedef uint32_t                uint32;     /**< Unsigned 32-bit integer. */
    typedef uint64_t                uint64;     /**< Unsigned 64-bit integer. */

    typedef uint8                   byte;       /**< Unsigned byte. */
    typedef int8                    sbyte;      /**< Signed byte. */
    typedef char                    char8;      /**< 8-bit character. */    
    typedef wxChar16                char16;     /**< 16-bit character. */
    typedef wxChar32                char32;     /**< 32-bit character. */

    typedef unsigned short          ushort;     /**< Short for 'unsigned short'. */
    typedef unsigned int            uint;       /**< Short for 'unsigned int'. */
    typedef unsigned long           ulong;      /**< Short for 'unsigned long'. */

    typedef neo::math::HalfFloat    half;       /**< 16-bit IEEE floating point number. */
    typedef neo::math::HalfFloat    float16;    /**< 16-bit IEEE floating point number. */
    typedef float                   float32;    /**< 32-bit IEEE floating point number. */
    typedef double                  float64;    /**< 64-bit IEEE floating point number. */

}; // namespace gw2b

// Gw2Browser includes
#include "Util/Array.h"
#include "Util/Ensure.h"
#include "Util/Misc.h"

#endif // STDAFX_H_INCLUDED
