/** \file       Util/Misc.cpp
 *  \brief      Contains definitions for some helper functions that don't fit
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

#include "stdafx.h"
#include "Misc.h"

namespace gw2b
{

#pragma warning(push)
#pragma warning(disable: 4146)  // unary minus operator applied to unsigned type, result still unsigned

    // http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup
    uint LowestSetBit(uint32 pValue)
    {
        static const int32 MultiplyDeBruijnBitPosition[32] =  {
            0x00, 0x01, 0x1c, 0x02, 0x1d, 0x0e, 0x18, 0x03, 0x1e, 0x16, 0x14, 0x0f, 0x19, 0x11, 0x04, 0x08, 
            0x1f, 0x1b, 0x0d, 0x17, 0x15, 0x13, 0x10, 0x07, 0x1a, 0x0c, 0x12, 0x06, 0x0b, 0x05, 0x0a, 0x09,
        };
        return MultiplyDeBruijnBitPosition[((uint32_t)((pValue & -pValue) * 0x077CB531U)) >> 27];
    }

#pragma warning(pop)

}; // namespace gw2b
