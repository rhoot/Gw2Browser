/** \file       RefCounted.h
 *  \brief      Contains the declaration for the reference counted base class.
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

#ifndef UTIL_REFCOUNTED_H_INCLUDED
#define UTIL_REFCOUNTED_H_INCLUDED

namespace gw2b
{
    /** Object that deletes itself when all references are gone. */
    class RefCounted
    {
        uint mRefCount;
    public:
        /** Initializes the reference count.
         *  \param[in] pRefCount    Optional initial reference count. Defaults to 1. */
        RefCounted(uint pRefCount = 1) : mRefCount(pRefCount)   {}

        /** Increases the reference count by one. */
        void AddRef()                                           { 
            mRefCount++; 
        }
        /** Reduces the reference count by one, and deletes the object if it reaches 0. */
        void Release()                                          { 
            if (!(--mRefCount)) { delete this; } 
        }
    protected:
        // protected destructor prevents instantiation on the stack
        virtual ~RefCounted()                                   {}
    }; // class RefCounted

}; // namespace gw2mw

#endif // UTIL_REFCOUNTED_H_INCLUDED
