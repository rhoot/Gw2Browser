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

    /** Determines the index of the least significant bit that is set.
     *  \param[in]  pValue  Value to find the lowest set bit of.
     *  \return uint    Zero-based index for the lowest set bit. */
    uint LowestSetBit(uint32 pValue);

}; // namespace gw2b

#endif // UTIL_MISC_H_INCLUDED
