/* \file       Array.h
*  \brief      Contains the declaration for the array class.
*  \author     Rhoot
*/

/*
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

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

namespace gw2b {
	template <typename T>
	struct ArrayData : public wxRefCounter {
		T*      mData;
		uint    mSize;
	public:
		ArrayData( )
			: mData( nullptr )
			, mSize( 0 ) {
		}

		ArrayData( const ArrayData& pOther ) {
			if ( pOther.mSize ) {
				mData = allocate<T>( pOther.mSize );
				mSize = pOther.mSize;
				::memcpy( mData, pOther.mData, mSize * sizeof( T ) );
			} else {
				mData = nullptr;
				mSize = 0;
			}
		}

		virtual ~ArrayData( ) {
			freePointer( mData );
		}
	};

	/** Class representing an array of elements.
	*  \tparam T           Type of elements stored in the array.
	*  \tparam Granularity Mask deciding how many objects to allocate at a time. */
	template <typename T, uint Granularity = 0x7>
	class Array {
		wxObjectDataPtr< ArrayData<T> >  mData;
	public:
		/** Default constructor. */
		Array( )
			: mData( new ArrayData<T>( ) ) {
		}

		/** Constructor.
		*  \param[in]  pSize   The initial size of the array. */
		Array( uint pSize )
			: mData( new ArrayData<T>( ) ) {
			this->SetSize( pSize );
		}

		/** Copy constructor.
		*  \param[in]  pOther  Object to copy. */
		Array( const Array& pOther )
			: mData( pOther.mData ) {
		}

		/** Destructor. */
		~Array( ) {
		}

		/** Assignment operator.
		*  \param[in]  pOther   Object to copy.
		*  \return Array   This array. */
		Array& operator=( const Array& pOther ) {
			mData = pOther.mData;
			return *this;
		}

		/** Addition assignment operator. Adds the elements of the other array
		*  to the end of this one.
		*  \param[in]  pOther  Array to append to this.
		*  \return Array   This array. */
		Array& operator+=( const Array& pOther ) {
			this->UnShare( );

			uint startPos = mData.get( )->mSize;
			uint newSize = mData.get( )->mSize + pOther.GetSize( );
			this->SetSize( newSize );
			::memcpy( &( ( *this )[startPos] ), pOther.GetPointer( ), pOther.GetSize( ) * sizeof( T ) );

			return *this;
		}

		/** Clears this array's elements, making it an empty array. */
		void Clear( ) {
			this->UnShare( );
		}

		/** Appends an item to this array.
		*  \param[in]  pItem   Item to add.
		*  \return uint    Index of newly added item. */
		uint Add( const T& pItem ) {
			this->UnShare( );
			uint index = mData.get( )->mSize;
			SetSize( mData.get( )->mSize + 1 );
			( *this )[index] = pItem;
			return index;
		}

		/** Appends a new item to this array.
		*  \return T&  reference to newly added item. */
		T& AddNew( ) {
			this->UnShare( );
			uint index = mData.get( )->mSize;
			SetSize( mData.get( )->mSize + 1 );
			return ( *this )[index];
		}

		/** Removes the given item from this array.
		*  \param[in]  pItem   Item to remove from this array. */
		void Remove( const T& item ) {
			uint& size = mData.get( )->mSize;
			T*& data = mData.get( )->mData;

			for ( uint i = 0; i < size; i++ ) {
				if ( data[i] == pItem )
					RemoveAt( i );
			}
		}

		/** Removes the item residing at the given index.
		*  \param[in]  pIndex  Index of element to remove. */
		void RemoveAt( uint pIndex ) {
			Assert( pIndex >= 0 && pIndex < mData.get( )->mSize );
			this->UnShare( );

			uint& size = mData.get( )->mSize;
			T*& data = mData.get( )->mData;

			if ( pIndex != size - 1 ) {
				::memmove( &data[pIndex], &( data[pIndex + 1] ), ( size - pIndex - 1 ) * sizeof( T ) );
			}
			size--;

			uint count = ( ( size + Granularity - 1 ) / Granularity ) * Granularity;
			data = static_cast<T*>( ::realloc( data, count * sizeof( T ) ) );
		}

		/** Sets the size of the array.
		*  \param[in]  pSize   New size of the array. */
		void SetSize( uint pSize ) {
			this->UnShare( );
			uint count = ( pSize + Granularity ) & ~Granularity;
			mData.get( )->mData = static_cast<T*>( ::realloc( this->GetPointer( ), count * sizeof( T ) ) );
			mData.get( )->mSize = pSize;
		}

		/** Gets the size of the array.
		*  \return uint    Size of the array. */
		uint GetSize( ) const {
			return mData.get( )->mSize;
		}

		/** Gets the size of the array, in bytes.
		*  \return uint    Size of the array, in bytes. */
		uint GetByteSize( ) const {
			return mData.get( )->mSize * sizeof( T );
		}

		/** Gets a pointer to this array.
		*  \return T*  Pointer to this array. */
		T* GetPointer( ) {
			return mData.get( )->mData;
		}

		/** Gets a const pointer to this array.
		*  \return T*  Pointer to this array. */
		const T* GetPointer( ) const {
			return mData.get( )->mData;
		}

		/** Array index operator. Returns the element at the given index.
		*  \param[in]  pIndex  Index of the element to retrieve.
		*  \return T&  Reference to the found item. */
		inline T& operator[]( uint pIndex ) {
			Assert( pIndex < mData.get( )->mSize );
			return mData.get( )->mData[pIndex];
		}

		/** Const array index operator. Returns the element at the given index.
		*  \param[in]  pIndex  Index of the element to retrieve.
		*  \return T&  Reference to the found item. */
		inline const T& operator[]( uint pIndex ) const {
			Assert( pIndex < mData.get( )->mSize );
			return mData.get( )->mData[pIndex];
		}

		/** Wraps the given array in this object, giving it control over
		*  the data contained. The data *must* be allocated using malloc.
		*  \param[in]  pData   Data to wrap.
		*  \param[in]  pSize   Size of the array. */
		void Wrap( T* pData, uint pSize ) {
			this->UnShare( false );
			mData.get( )->mData = pData;
			mData.get( )->mSize = pSize;
		}

		/** Unwraps the data from this array object, thus freeing the data from
		*  this control (not from memory). The data is then up to the user to
		*  free.
		*  \return T*  Unwrapped data. */
		T* UnWrap( ) {
			this->UnShare( );
			T* data = mData.get( )->mData;
			mData.get( )->mData = nullptr;
			mData.get( )->mSize = 0;
			return data;
		}
	protected:
		void UnShare( bool pCopy = true ) {
			if ( mData->GetRefCount( ) == 1 ) {
				if ( !pCopy ) {
					freePointer( mData.get( )->mData );
					mData.get( )->mSize = 0;
				}
				return;
			}

			mData = ( pCopy ? new ArrayData<T>( *mData ) : new ArrayData<T>( ) );
		}
	};
};

#endif // UTIL_ARRAY_H_INCLUDED
