/* \file       ReadIndexTask.cpp
*  \brief      Contains declaration of the ScanDatTask class.
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
#include "ScanDatTask.h"

#include "DatFile.h"
#include "DatIndex.h"
#include "FileReader.h"

namespace gw2b {

	ScanDatTask::ScanDatTask( const std::shared_ptr<DatIndex>& p_index, DatFile& p_datFile )
		: m_index( p_index )
		, m_datFile( p_datFile ) {
		Ensure::notNull( p_index.get( ) );
		Ensure::notNull( &p_datFile );
	}

	ScanDatTask::~ScanDatTask( ) {
	}

	bool ScanDatTask::init( ) {
		this->setMaxProgress( m_datFile.numFiles( ) );
		this->setCurrentProgress( m_index->highestMftEntry( ) + 1 );

		uint filesLeft = m_datFile.numFiles( ) - ( m_index->highestMftEntry( ) + 1 );
		m_index->reserveEntries( filesLeft );

		return true;
	}

	void ScanDatTask::perform( ) {
		// Make sure the output buffer is big enough
		this->ensureBufferSize( 0x20 );

		// Read file
		uint32 entryNumber = this->currentProgress( );
		uint size = m_datFile.peekFile( entryNumber, 0x20, m_outputBuffer.GetPointer( ) );

		// Skip if empty
		if ( !size ) {
			this->setCurrentProgress( entryNumber + 1 );
			return;
		}

		// Get the file type
		ANetFileType fileType;
		auto results = m_datFile.identifyFileType( m_outputBuffer.GetPointer( ), size, fileType );

		// Enough data to identify the file type?
		uint lastRequestedSize = 0x20;
		while ( results == DatFile::IR_NotEnoughData ) {
			uint sizeRequired = this->requiredIdentificationSize( m_outputBuffer.GetPointer( ), size, fileType );

			// Prevent infinite loops
			if ( sizeRequired == lastRequestedSize ) {
				break;
			}
			lastRequestedSize = sizeRequired;

			// Re-read with the newly asked-for size
			this->ensureBufferSize( sizeRequired );
			size = m_datFile.peekFile( entryNumber, sizeRequired, m_outputBuffer.GetPointer( ) );
			results = m_datFile.identifyFileType( m_outputBuffer.GetPointer( ), size, fileType );
		}

		// Need another check, since the file might have been reloaded a couple of times
		if ( !size ) {
			this->setCurrentProgress( entryNumber + 1 );
			return;
		}

		// Categorize the entry
		auto category = this->categorize( fileType, m_outputBuffer.GetPointer( ), size );

		// Add to index
		uint baseId = m_datFile.baseIdFromFileNum( entryNumber );
		auto& newEntry = m_index->addIndexEntry( )
			->setBaseId( baseId )
			.setFileId( m_datFile.fileIdFromFileNum( entryNumber ) )
			.setFileType( fileType )
			.setMftEntry( entryNumber )
			.setName( wxString::Format( wxT( "%d" ), baseId ) );
		// Found a file with no baseId...
		if ( baseId == 0 ) {
			newEntry.setName( wxString::Format( wxT( "ID-less_%d" ), entryNumber ) );
		}
		// Finalize the add
		category->addEntry( &newEntry );
		newEntry.finalizeAdd( );

		// Delete the reader and proceed to the next file
		this->setText( wxString::Format( wxT( "Scanning .dat: %d/%d" ), entryNumber, this->maxProgress( ) ) );
		this->setCurrentProgress( entryNumber + 1 );
	}

	uint ScanDatTask::requiredIdentificationSize( const byte* p_data, uint p_size, ANetFileType p_fileType ) {
		switch ( p_fileType ) {
		case ANFT_Binary:
			if ( p_size >= 0x40 ) {
				return *reinterpret_cast<const uint32*>( p_data + 0x3c ) + 0x18;
			} else {
				return 0x140;   // Seems true for most of them
			}
		case ANFT_ABFF:
			return 0x64;
		case ANFT_Sound:
			return 0x160;
		default:
			return 0x20;
		}
	}

#define MakeCategory(x)     { category = m_index->findOrAddCategory(x); }
#define MakeSubCategory(x)  { category = category->findOrAddSubCategory(x); }
	DatIndexCategory* ScanDatTask::categorize( ANetFileType p_fileType, const byte* p_data, uint p_size ) {
		DatIndexCategory* category = nullptr;

		// Textures
		if ( p_fileType > ANFT_TextureStart && p_fileType < ANFT_TextureEnd ) {
			MakeCategory( wxT( "Textures" ) );

			switch ( p_fileType ) {
			case ANFT_ATEX:
				MakeSubCategory( wxT( "Generic textures" ) );
				break;
			case ANFT_ATTX:
				MakeSubCategory( wxT( "Terrain textures" ) );
				break;
			case ANFT_ATEC:
				MakeSubCategory( wxT( "ATEC" ) );
				break;
			case ANFT_ATEP:
				MakeSubCategory( wxT( "Map textures" ) );
				break;
			case ANFT_ATEU:
				MakeSubCategory( wxT( "UI textures" ) );
				break;
			case ANFT_ATET:
				MakeSubCategory( wxT( "ATET" ) );
				break;
			case ANFT_DDS:
				MakeSubCategory( wxT( "DDS" ) );
				break;
			}

			if ( p_fileType != ANFT_DDS && p_size >= 12 ) {
				uint16 width = *reinterpret_cast<const uint16*>( p_data + 0x8 );
				uint16 height = *reinterpret_cast<const uint16*>( p_data + 0xa );
				MakeSubCategory( wxString::Format( wxT( "%dx%d" ), width, height ) );
			} else if ( p_size >= 20 ) {
				uint32 width = *reinterpret_cast<const uint32*>( p_data + 0x10 );
				uint32 height = *reinterpret_cast<const uint32*>( p_data + 0x0c );
				MakeSubCategory( wxString::Format( wxT( "%dx%d" ), width, height ) );
			}
		}

		switch ( p_fileType ) {

		// Sounds
		case ANFT_MP3:
		case ANFT_OGG:
		case ANFT_Sound:
			MakeCategory( wxT( "Sounds" ) );
			break;

		// Binaries
		case ANFT_Binary:
		case ANFT_EXE:
		case ANFT_DLL:
			MakeCategory( wxT( "Binaries" ) );
			break;

		// Strings
		case ANFT_StringFile:
			MakeCategory( wxT( "Strings" ) );
			break;

		// Manifests
		case ANFT_Manifest:
			MakeCategory( wxT( "Manifests" ) );
			break;

		// Bank files
		case ANFT_Bank:
			MakeCategory( wxT( "Bank files" ) );
			break;

		// Model files
		case ANFT_Model:
			MakeCategory( wxT( "Models" ) );
			break;

		// Dependency table
		case ANFT_DependencyTable:
			MakeCategory( wxT( "Dependency tables" ) );
			break;

		// EULA
		case ANFT_EULA:
			MakeCategory( wxT( "EULA" ) );
			break;

		// Cinematic
		case ANFT_Cinematic:
			MakeCategory( wxT( "Cinematics" ) );
			break;

		// Havok
		case ANFT_HavokCloth:
			MakeCategory( wxT( "Havok cloth" ) );
			break;

		// Maps
		case ANFT_Map:
			MakeCategory( wxT( "Maps" ) );
			break;

		// Materials
		case ANFT_Material:
			MakeCategory( wxT( "Materials" ) );
			break;

		// Random PF files
		case ANFT_PF:
			MakeCategory( wxT( "Misc" ) );
			if ( p_size >= 12 ) {
				MakeSubCategory( wxString( reinterpret_cast<const char*>( p_data + 8 ), 4 ) );
			}
			break;

		// ABFF
		case ANFT_ABFF :
			MakeCategory( wxT( "Misc" ) );
			MakeSubCategory( wxT( "ABFF" ) );
			break;

		// unknown stuff
		default:
			MakeCategory( wxT( "Unknown" ) );
			MakeSubCategory( wxString::Format( wxT( "%x" ), *reinterpret_cast<const uint32*>( p_data ) ) );
		}

		return category;
	}

	void ScanDatTask::ensureBufferSize( uint p_size ) {
		if ( m_outputBuffer.GetSize( ) < p_size ) {
			m_outputBuffer.SetSize( p_size );
		}
	}

}; // namespace gw2b
