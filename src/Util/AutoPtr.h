/** \file       AutoPtr.h
 *  \brief      Contains the declaration for the auto pointer class.
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

#ifndef UTIL_AUTOPTR_H_INCLUDED
#define UTIL_AUTOPTR_H_INCLUDED

#ifndef AUTOPTR_ADDREF
#define AUTOPTR_ADDREF        AddRef
#endif

#ifndef AUTOPTR_RELEASE
#define AUTOPTR_RELEASE       Release
#endif

namespace gw2b
{

template <typename T> class AutoPtr {
    T* mPointer;
public:
    /** Constructor. Sets the internal pointer to NULL. */
    AutoPtr() : mPointer(NULL) {}
    /** Constructor. Sets the internal pointer, and optionally adds a reference to the reference counter. The default behaviour is
     *  NOT to increment it.
     *  \param[in]  pPointer    Pointer value.
     *  \param[in]  pAddRef		Optional. True to increment the reference counter, false to leave it as is. Defaults to false. */
    AutoPtr(T* pPointer, bool pAddRef = false) : mPointer(pPointer) { if (mPointer && pAddRef) mPointer->AUTOPTR_ADDREF(); }
    /** Copy constructor. Sets the internal pointer, and optionally adds a reference to the reference counter. The default behaviour
     *  is to increment it.
     *  \param[in]  pPointer	Pointer value.
     *  \param[in]  pAddRef		Optional. True to increment the reference counter, false to leave it as is. Defaults to true. */
    AutoPtr(const AutoPtr<T>& pOther, bool pAddRef = true) : mPointer(pOther.mPointer) { if (mPointer && pAddRef) mPointer->AUTOPTR_ADDREF(); }
    /** Destructor. Decrements the reference counter on the pointed to object, if any. */
    ~AutoPtr() { if (mPointer) mPointer->AUTOPTR_RELEASE(); }

    /** Decrements the reference counter, assigns the internal pointer to the given pointer, and optionally increments the reference 
     *  counter. The default behaviour is to increment it.
     *  \param[in]  pPointer	Pointer value to assign. 
     *  \param[in]  pAddRef     Optional. True to increment the reference counter, false to leave it as is. Defaults to true. */
    void Assign(T* pPointer, bool pAddRef) { Free(); mPointer = pPointer; if (mPointer && pAddRef) mPointer->AUTOPTR_ADDREF(); }
    /** Decrements the reference counter and sets the internal pointer to NULL. */
    void Free() { if (mPointer) { mPointer->AUTOPTR_RELEASE(); mPointer = NULL; } }

    /** Assigns the internal pointer value to the given value. 
     *  \param[in]  pPointer	Pointer value to assign. */
    AutoPtr<T>& operator=(T* pPointer) { Assign(pPointer, false); return *this; }
    /** Assigns the internal pointer value to the given value. 
     *  \param[in]  pPointer	Pointer value to assign. */
    AutoPtr<T>& operator=(const AutoPtr<T>& pOther) { Assign(pOther.mPointer, true); return *this; }

    /** Overloads the -> operator, allowing you to act on the internal pointer directly.
     *  Example: SmartPtr<Foo> foo(new Foo()); foo->Bar(); */
    T* operator->() { return mPointer; }
    /** Overloads the * prefix operator, allowing you to act on the internal pointer directly.
     *  Example: SmartPtr<Foo> foo(new Foo()); Foo& bar = *foo; */
    T& operator*() { return *mPointer; }
    /** Overloads the -> operator, allowing you to act on the internal pointer directly.
     *  Example: SmartPtr<Foo> foo(new Foo()); foo->Bar(); */
    const T* operator->() const { return mPointer; }
    /** Overloads the * prefix operator, allowing you to act on the internal pointer directly.
     *  Example: SmartPtr<Foo> foo(new Foo()); Foo& bar = *foo; */
    const T& operator*() const { return *mPointer; }

    /** Overloads the T* cast operator, allowing you to act on the internal pointer directly.
     *  Example: SmartPtr<Foo> foo(new Foo()); void Bar(Foo* foo); Bar(foo); */
    operator T*() { return mPointer; }
    /** Overloads the const T* cast operator, allowing you to act on the internal pointer directly.
     *  Example: const SmartPtr<Foo> foo(new Foo()); void Bar(const Foo* foo); Bar(foo); */
    operator const T*() const { return mPointer; }
}; // class AutoPtr

}; // namespace gw2b

#undef AUTOPTR_ADDREF
#undef AUTOPTR_RELEASE

#endif // UTIL_AUTOPTR_H_INCLUDED
