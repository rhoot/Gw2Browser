/* \file       DatIndex.cpp
*  \brief      Contains the definition for the index model class.
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

#include "stdafx.h"
#include <wx/file.h>
#include <new>

#include "DatIndex.h"

namespace gw2b {

	//----------------------------------------------------------------------------
	//      DatIndexEntry
	//----------------------------------------------------------------------------

	DatIndexEntry::DatIndexEntry( DatIndex& p_owner )
		: m_owner( &p_owner )
		, m_fileId( 0 )
		, m_baseId( 0 )
		, m_mftEntry( 0 )
		, m_fileType( ANFT_Unknown )
		, m_category( nullptr ) {
		Ensure::notNull( &p_owner );
	}

	void DatIndexEntry::onAddedToCategory( DatIndexCategory* p_category ) {
		m_category = p_category;
	}

	void DatIndexEntry::finalizeAdd( ) {
		m_owner->onEntryAddComplete( *this );
	}

	//----------------------------------------------------------------------------
	//      DatIndexCategory
	//----------------------------------------------------------------------------

	DatIndexCategory::DatIndexCategory( DatIndex& p_owner, const wxString& p_name, int p_index )
		: m_owner( &p_owner )
		, m_index( p_index )
		, m_name( p_name )
		, m_parent( nullptr ) {
		Ensure::notNull( &p_owner );
	}

	DatIndexCategory* DatIndexCategory::findSubCategory( const wxString& p_name ) {
		for ( uint i = 0; i < m_subCategories.GetSize( ); i++ ) {
			if ( m_subCategories[i]->name( ) == p_name ) {
				return m_subCategories[i];
			}
		}
		return nullptr;
	}

	DatIndexCategory* DatIndexCategory::findOrAddSubCategory( const wxString& p_name ) {
		auto subCat = this->findSubCategory( p_name );
		if ( !subCat ) {
			subCat = m_owner->addIndexCategory( p_name );
			this->addSubCategory( subCat );
		}
		return subCat;
	}

	uint DatIndexCategory::numEntries( bool p_recursive ) const {
		auto count = m_entries.GetSize( );

		if ( p_recursive ) {
			for ( uint i = 0; i < m_subCategories.GetSize( ); i++ ) {
				count += m_subCategories[i]->numEntries( p_recursive );
			}
		}

		return count;
	}

	void DatIndexCategory::addEntry( DatIndexEntry* p_entry ) {
		m_entries.Add( p_entry );
		p_entry->onAddedToCategory( this );
	}

	void DatIndexCategory::addSubCategory( DatIndexCategory* p_subCategory ) {
		Ensure::notNull( p_subCategory );
		Assert( !p_subCategory->parent( ) );

		m_subCategories.Add( p_subCategory );
		p_subCategory->onAddedToCategory( this );
	}

	void DatIndexCategory::onAddedToCategory( DatIndexCategory* p_parent ) {
		Ensure::isNull( m_parent );
		m_parent = p_parent;
	}

	//----------------------------------------------------------------------------
	//      DatIndex
	//----------------------------------------------------------------------------

	DatIndex::DatIndex( )
		: m_datTimestamp( 0 )
		, m_highestMftEntry( -1 )
		, m_isDirty( false )
		, m_numEntries( 0 )
		, m_numCategories( 0 ) {
	}

	DatIndex::~DatIndex( ) {
		this->clear( );

		// Notify listeners, so they can clear pointers etc
		for ( auto it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
			( *it )->onIndexDestruction( *this );
		}
	}

	void DatIndex::clear( ) {
		// destruct all entries before clearing their memory
		for ( uint i = 0; i < m_numEntries; i++ ) {
			delete m_entries[i];
		}
		m_entries.Clear( );
		// also destruct all categories before clearing their memory
		for ( uint i = 0; i < m_numCategories; i++ ) {
			delete m_categories[i];
		}
		m_categories.Clear( );

		m_datTimestamp = 0;
		m_highestMftEntry = -1;
		m_isDirty = false;
		m_numEntries = 0;
		m_numCategories = 0;

		// Notify listeners
		for ( auto it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
			( *it )->onIndexCleared( *this );
		}
	}

	DatIndexEntry* DatIndex::addIndexEntry( bool p_setDirty ) {
		if ( m_numEntries == m_entries.GetSize( ) ) {
			if ( !reserveEntries( 1 ) ) {
				return nullptr;
			}
		}

		uint index = m_numEntries++;
		m_entries[index] = new DatIndexEntry( *this );

		m_isDirty = ( m_isDirty || p_setDirty );
		return m_entries[index];
	}

	DatIndexCategory* DatIndex::findCategory( const wxString& p_name, bool p_rootsOnly ) {
		for ( uint i = 0; i < m_numCategories; i++ ) {
			if ( !p_rootsOnly || !( m_categories[i]->parent( ) ) ) {
				if ( m_categories[i]->name( ) == p_name ) {
					return m_categories[i];
				}
			}
		}
		return nullptr;
	}

	DatIndexCategory* DatIndex::addIndexCategory( const wxString& p_name, bool p_setDirty ) {
		if ( m_numCategories == m_categories.GetSize( ) ) {
			if ( !reserveCategories( 1 ) ) {
				return nullptr;
			}
		}

		uint index = m_numCategories++;
		m_categories[index] = new DatIndexCategory( *this, p_name, index );
		auto& category = *m_categories[index];

		// Notify listeners
		for ( auto it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
			( *it )->onIndexCategoryAdded( *this, category );
		}

		m_isDirty = ( m_isDirty || p_setDirty );
		return &category;
	}

	DatIndexCategory* DatIndex::findOrAddCategory( const wxString& p_name, bool p_setDirty ) {
		auto category = this->findCategory( p_name, true );
		if ( !category ) {
			category = this->addIndexCategory( p_name, p_setDirty );
		}
		return category;
	}

	bool DatIndex::reserveEntries( uint p_additionalEntries ) {
		if ( ( UINT_MAX - m_entries.GetSize( ) ) < p_additionalEntries ) {
			return false;
		}
		m_entries.SetSize( m_entries.GetSize( ) + p_additionalEntries );
		return true;
	}

	bool DatIndex::reserveCategories( uint p_additionalCategories ) {
		if ( ( UINT_MAX - m_categories.GetSize( ) ) < p_additionalCategories ) {
			return false;
		}
		m_categories.SetSize( m_categories.GetSize( ) + p_additionalCategories );
		return true;
	}

	void DatIndex::addListener( IDatIndexListener* p_listener ) {
		m_listeners.insert( p_listener );
	}

	void DatIndex::removeListener( IDatIndexListener* p_listener ) {
		m_listeners.erase( p_listener );
	}

	void DatIndex::onEntryAddComplete( DatIndexEntry& p_entry ) {
		if ( static_cast<int>( p_entry.mftEntry( ) ) > m_highestMftEntry ) {
			m_highestMftEntry = static_cast<int>( p_entry.mftEntry( ) );
		}

		// Notify listeners
		for ( auto it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
			( *it )->onIndexFileAdded( *this, p_entry );
		}
	}

};
