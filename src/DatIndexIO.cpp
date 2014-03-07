/* \file       DatIndexIO.cpp
*  \brief      Contains the definition for the index reader and writer classes.
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
#include "DatIndexIO.h"

namespace gw2b {

	//----------------------------------------------------------------------------
	//      DatIndexReader
	//----------------------------------------------------------------------------

	DatIndexReader::DatIndexReader( DatIndex& p_index )
		: m_index( p_index ) {
		Ensure::notNull( &p_index );
		::memset( &m_header, 0, sizeof( m_header ) );
	}

	DatIndexReader::~DatIndexReader( ) {
		this->close( );
	}

	bool DatIndexReader::open( const wxString& p_filename ) {
		this->close( );
		if ( !wxFile::Exists( p_filename ) ) {
			return false;
		}

		m_file.Open( p_filename );
		if ( m_file.IsOpened( ) && m_file.Length( ) > sizeof( m_header ) ) {
			m_file.Read( &m_header, sizeof( m_header ) );
			if ( m_header.magicInteger != DatIndex_Magic ) {
				this->close( ); return false;
			}
			if ( m_header.version != DatIndex_Version ) {
				this->close( ); return false;
			}
			m_index.clear( ); // always start with a fresh index
			m_index.setDatTimestamp( m_header.datTimestamp );
			m_index.reserveEntries( m_header.numEntries );
			m_index.reserveCategories( m_header.numEntries );
			return true;
		}

		return false;
	}

	void DatIndexReader::close( ) {
		m_file.Close( );
		::memset( &m_header, 0, sizeof( m_header ) );
	}

	bool DatIndexReader::isDone( ) const {
		return ( m_index.numCategories( ) == m_header.numCategories )
			&& ( m_index.numEntries( ) == m_header.numEntries );
	}

	DatIndexReader::ReadResult DatIndexReader::read( uint p_amount ) {
		ReadResult result = RR_Failure;

		for ( uint i = 0; i < p_amount; i++ ) {
			ssize_t bytesRead;

			// First read all categories, one at a time
			if ( m_index.numCategories( ) < m_header.numCategories ) {
				// Read fixed-width fields
				DatIndexCategoryFields fields;
				bytesRead = m_file.Read( &fields, sizeof( fields ) );
				if ( bytesRead < sizeof( fields ) ) {
					result = RR_CorruptFile; goto READ_FAILED;
				}
				// Read name
				Array<char> nameData( fields.nameLength );
				bytesRead = m_file.Read( nameData.GetPointer( ), nameData.GetSize( ) );
				if ( bytesRead < ( ssize_t ) nameData.GetSize( ) ) {
					result = RR_CorruptFile; goto READ_FAILED;
				}
				// Add category
				auto name = wxString::FromUTF8Unchecked( nameData.GetPointer( ), nameData.GetSize( ) );
				auto category = m_index.addIndexCategory( name, false );
				// Set parent
				if ( fields.parent != DatIndex_RootCategory ) {
					auto parent = m_index.category( fields.parent );
					if ( parent ) {
						parent->addSubCategory( category );
					}
				}
			}

			// If all categories are read, start reading the files instead (note the 'else')
			else if ( m_index.numEntries( ) < m_header.numEntries ) {
				// Read fixed-width fields
				DatIndexEntryFields fields;
				bytesRead = m_file.Read( &fields, sizeof( fields ) );
				if ( bytesRead < sizeof( fields ) ) {
					result = RR_CorruptFile; goto READ_FAILED;
				}
				// Read name
				Array<char> nameData( fields.nameLength );
				bytesRead = m_file.Read( nameData.GetPointer( ), nameData.GetSize( ) );
				if ( bytesRead < ( ssize_t ) nameData.GetSize( ) ) {
					result = RR_CorruptFile; goto READ_FAILED;
				}
				// Add entry
				auto name = wxString::FromUTF8Unchecked( nameData.GetPointer( ), nameData.GetSize( ) );
				auto& newEntry = m_index.addIndexEntry( false )
					->setBaseId( fields.baseId )
					.setFileId( fields.fileId )
					.setMftEntry( fields.mftEntry )
					.setFileType( ( ANetFileType ) fields.fileType )
					.setName( name );
				auto category = m_index.category( fields.category );
				if ( !category ) {
					result = RR_CorruptFile; goto READ_FAILED;
				}
				category->addEntry( &newEntry );
				newEntry.finalizeAdd( );
			}

			// If both are done we can skip this loop
			else {
				break;
			}
		}

		return RR_Success;

	READ_FAILED:
		return result;
	}

	//----------------------------------------------------------------------------
	//      DatIndexWriter
	//----------------------------------------------------------------------------

	DatIndexWriter::DatIndexWriter( DatIndex& p_index )
		: m_index( p_index )
		, m_categoriesWritten( 0 )
		, m_entriesWritten( 0 ) {
		Ensure::notNull( &p_index );
	}

	DatIndexWriter::~DatIndexWriter( ) {
		this->close( );
	}

	bool DatIndexWriter::open( const wxString& p_filename ) {
		this->close( );

		m_file.Open( p_filename, wxFile::write );
		if ( m_file.IsOpened( ) ) {
			DatIndexHead header;
			header.magicInteger = DatIndex_Magic;
			header.version = DatIndex_Version;
			header.datTimestamp = m_index.datTimestamp( );
			header.numEntries = m_index.numEntries( );
			header.numCategories = m_index.numCategories( );

			auto bytesWritten = m_file.Write( &header, sizeof( header ) );
			if ( bytesWritten < sizeof( header ) ) {
				this->close( ); return false;
			}

			return true;
		}

		return false;
	}

	void DatIndexWriter::close( ) {
		m_file.Close( );
		m_categoriesWritten = 0;
		m_entriesWritten = 0;
	}

	bool DatIndexWriter::isDone( ) const {
		return ( m_index.numEntries( ) == m_entriesWritten )
			&& ( m_index.numCategories( ) == m_categoriesWritten );
	}

	bool DatIndexWriter::write( uint p_amount ) {
		for ( uint i = 0; i < p_amount; i++ ) {
			ssize_t bytesWritten;

			// First write categories, one at a time
			if ( m_categoriesWritten < m_index.numCategories( ) ) {
				auto category = m_index.category( m_categoriesWritten );
				auto parent = category->parent( );
				wxScopedCharBuffer nameBuffer = category->name( ).ToUTF8( );
				// Fixed-width fields
				DatIndexCategoryFields fields;
				fields.parent = ( parent ? parent->index( ) : -1 );
				fields.nameLength = nameBuffer.length( );
				bytesWritten = m_file.Write( &fields, sizeof( fields ) );
				if ( bytesWritten < sizeof( fields ) ) {
					return false;
				}
				// Name
				bytesWritten = m_file.Write( nameBuffer, fields.nameLength );
				if ( bytesWritten < fields.nameLength ) {
					return false;
				}
				// Increase the counter
				m_categoriesWritten++;
			}

			// Then, write entries one at a time (note the 'else')
			else if ( m_entriesWritten < m_index.numEntries( ) ) {
				auto entry = m_index.entry( m_entriesWritten );
				auto category = entry->category( );
				auto nameBuffer = entry->name( ).ToUTF8( );
				// Fixed-width fields
				DatIndexEntryFields fields;
				fields.category = category->index( );
				fields.baseId = entry->baseId( );
				fields.fileId = entry->fileId( );
				fields.mftEntry = entry->mftEntry( );
				fields.fileType = entry->fileType( );
				fields.nameLength = nameBuffer.length( );
				bytesWritten = m_file.Write( &fields, sizeof( fields ) );
				if ( bytesWritten < sizeof( fields ) ) {
					return false;
				}
				// Name
				bytesWritten = m_file.Write( nameBuffer, fields.nameLength );
				if ( bytesWritten < fields.nameLength ) {
					return false;
				}
				// Increase the counter
				m_entriesWritten++;
			}

			// Both done = ditch this loop
			else {
				break;
			}
		}

		// Remove dirty flag if everything is saved
		if ( this->isDone( ) ) {
			m_index.setDirty( false );
		}

		return true;
	}

}; // namespace gw2b
