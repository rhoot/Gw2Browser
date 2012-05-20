/** \file       Array.h
 *  \brief      Contains the declaration for the array class.
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

#ifndef UTIL_ARRAY_H_INCLUDED
#define UTIL_ARRAY_H_INCLUDED

#include <new>

namespace gw2b
{
    /** Less than functor. */
    struct LessThan
    {
        template <typename T>
            bool operator()(const T& v1, const T& v2) const { return v1 < v2; }
    };

    /** Less than functor, for pointer types. */
    struct LessThanPtr
    {
        template <typename T>
            bool operator()(const T& v1, const T& v2) const { return *v1 < *v2; }
    };

    /** Class representing an array of elements.
     *  \tparam T           Type of elements stored in the array.
     *  \tparam Construct   true to construct and destruct items, false to not
     *  \tparam Granularity Mask deciding how many objects to allocate at a time. */
    template <typename T, bool Construct=true, uint Granularity=0x7>	
        class Array
    {
        T*   mData;
        uint mSize;
    public:
        /** Default constructor. */
        Array()
            : mData(NULL), mSize(0)
        {
        }

        /** Constructor.
         *  \param[in]  pSize   The initial size of the array. */
        Array(uint pSize)
            : mData(NULL), mSize(0)
        {
            this->SetSize(pSize);
        }

        /** Copy constructor.
         *  \param[in]  pOther  Object to copy. */
        Array(const Array& pOther)
            : mData(NULL), mSize(0)
        {
            *this = pOther;
        }

        /** Destructor. */
        ~Array()
        {
            this->Clear();
        }

        /** Assignment operator.
         *  \param[in]  pOther   Object to copy.
         *  \return Array   This array. */
        Array& operator=(const Array& pOther)
        {
            Clear();
            return (*this += pOther);
        }

        /** Addition assignment operator. Adds the elements of the other array 
         *  to the end of this one.
         *  \param[in]  pOther  Array to append to this.
         *  \return Array   This array. */
        Array& operator+=(const Array& pOther)
        {
            uint startPos = mSize;
            uint newSize  = mSize + pOther.GetSize();
            SetSize(newSize);

            for (uint i = 0; i < newSize; ++i)
                mData[startPos + i] = pOther.mData[i];
            return *this;
        }

        /** Clears this array's elements, making it an empty array. */
        void Clear()
        {
            if (Construct)
            {
                for (uint i = 0; i < mSize; ++i)
                    mData[i].~T();
            }
            ::free(mData);
            mData = NULL;
            mSize = 0;
        }

        /** Appends an item to this array.
         *  \param[in]  pItem   Item to add. 
         *  \return uint    Index of newly added item. */
        uint Add(const T& pItem)
        {
            uint index = mSize;
            SetSize(mSize + 1);
            mData[index] = pItem;
            return index;
        }

        /** Appends a new item to this array.
         *  \return T&  reference to newly added item. */
        T& AddNew()
        {
            uint index = mSize;
            SetSize(mSize + 1);
            if (Construct) {
                new(&mData[index]) T;
            }
            return mData[index];
        }

        /** Removes the given item from this array.
         *  \param[in]  pItem   Item to remove from this array. */
        void Remove(const T& item)
        {
            for (uint i = 0; i < mSize; i++) {
                if (mData[i] == pItem)
                    RemoveAt(i);
            }
        }

        /** Removes the item residing at the given index. 
         *  \param[in]  pIndex  Index of element to remove. */
        void RemoveAt(uint pIndex)
        {
            assert(pIndex >= 0 && pIndex < mSize);

            if (Construct)
                mData[pIndex].~T();
            if (pIndex != mSize - 1)
                ::memmove(&mData[pIndex], &mData[pIndex + 1], (mSize - pIndex - 1) * sizeof(T));

            mSize--;
            uint count = ((mSize + Granularity - 1) / Granularity) * Granularity;
            mData = (T*)::realloc(mData, count * sizeof(T));
        }

        /** Sets the size of the array. 
         *  \param[in]  pSize   New size of the array. */
        void SetSize(uint pSize)
        {
            /* Destruct */
            if (Construct)
            {
                for (uint i = pSize; i < mSize; ++i)
                    mData[i].~T();
            }

            /* Realloc */
            uint count = (pSize + Granularity) & ~Granularity;
            mData = (T*)::realloc(mData, count * sizeof(T));

            /* Construct */
            if (Construct)
            {
                for (uint i = mSize; i < pSize; ++i)
                    new(&(mData[i])) T;
            }

            mSize = pSize;
        }

        /** Gets the size of the array.
         *  \return uint    Size of the array. */
        uint GetSize() const
        {
            return mSize;
        }

        /** Gets the size of the array, in bytes.
         *  \return uint    Size of the array, in bytes. */
        uint GetByteSize() const
        {
            return mSize * sizeof(T);
        }

        /** Gets a pointer to this array.
         *  \return T*  Pointer to this array. */
        T* GetPointer()
        {
            return mData;
        }

        /** Gets a const pointer to this array.
         *  \return T*  Pointer to this array. */
        const T* GetPointer() const
        {
            return mData;
        }

        /** Array index operator. Returns the element at the given index.
         *  \param[in]  pIndex  Index of the element to retrieve.
         *  \return T&  Reference to the found item. */
        inline T& operator[](uint pIndex)
        {
            assert(pIndex < mSize);
            return mData[pIndex];
        }

        /** Const array index operator. Returns the element at the given index.
         *  \param[in]  pIndex  Index of the element to retrieve.
         *  \return T&  Reference to the found item. */
        inline const T& operator[](uint pIndex) const
        {
            assert(pIndex < mSize);
            return mData[pIndex];
        }

        /** Wraps the given array in this object, giving it control over
         *  the data contained. The data *must* be allocated using malloc.
         *  \param[in]  pData   Data to wrap.
         *  \param[in]  pSize   Size of the array. */
        void Wrap(T* pData, uint pSize) 
        {
            Clear();
            mData = pData;
            mSize = pSize;
        }

        /** Unwraps the data from this array object, thus freeing the data from
         *  this control (not from memory). The data is then up to the user to
         *  free. 
         *  \return T*  Unwrapped data. */
        T* UnWrap()
        {
            T* data = mData;
            mData = NULL;
            mSize = 0;
            return data;
        }

        /** Sorts this array using the provided sort function.
         *  \tparam SortFunc    Function to use for comparing objects. */
        template <typename SortFunc>
            void Sort()
        {
            SortFunc func;
            uint lowestId;
            uint highestId;

            uint count = mSize >> 1;  // divide by 2
            for (uint i = 0; i < count; ++i)
            {
                uint end = (mSize - 1) - i;

                lowestId = i;
                highestId = end;

                for (uint j = i; j <= end; ++j)
                {
                    if (func(mData[j], mData[lowestId]))  { lowestId = j; }
                    if (func(mData[highestId], mData[j])) { highestId = j; }
                }

                if (lowestId != highestId)
                {
                    if (lowestId  != i)              { Swap(mData[i], mData[lowestId]); }
                    if (highestId != end)          
                    {
                        if (highestId == i)
                            Swap(mData[end], mData[lowestId]);
                        else
                            Swap(mData[end], mData[highestId]); 
                    }
                }
            }
        }
    };
};

#endif // UTIL_ARRAY_H_INCLUDED
