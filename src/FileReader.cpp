/* \file       FileReader.cpp
*  \brief      Contains the definition for the base class of readers for the
*              various file types.
*  \author     Rhoot
*/

/*
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>
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
#include "FileReader.h"

#include "Readers/ImageReader.h"
#include "Readers/ModelReader.h"

namespace gw2b {

	FileReader::FileReader( const Array<byte>& p_data, ANetFileType p_fileType )
		: m_data( p_data )
		, m_fileType( p_fileType ) {
	}

	FileReader::~FileReader() {
	}

	void FileReader::clean() {
		m_data.Clear();
		m_fileType = ANFT_Unknown;
	}

	Array<byte> FileReader::convertData() const {
		return m_data;
	}

	const wxChar* FileReader::extension() const {
		switch ( this->m_fileType ) {
		// Texture
		case ANFT_ATEX:
			return wxT( ".atex" );
			break;
		case ANFT_ATTX:
			return wxT( ".attx" );
			break;
		case ANFT_ATEC:
			return wxT( ".atec" );
			break;
		case ANFT_ATEP:
			return wxT( ".atep" );
			break;
		case ANFT_ATEU:
			return wxT( ".ateu" );
			break;
		case ANFT_ATET:
			return wxT( ".atet" );
			break;
		case ANFT_DDS:
			return wxT( ".dds" );
			break;
		case ANFT_JPEG:
			return wxT( ".jpg" );
			break;
		case ANFT_WEBP:
			return wxT( ".webp" );
			break;

		// String files
		case ANFT_StringFile:
			return wxT( ".strs" );
			break;
		case ANFT_EULA:
			return wxT( ".eula" );
			break;

		// Maps stuff
		case ANFT_Map:
			return wxT( ".mapc" );
			break;
		case ANFT_MapShadow:
			return wxT( ".mpsd" );
			break;
		case ANFT_PagedImageTable:
			return wxT( ".pimg" );
			break;

		case ANFT_Material:
			return wxT( ".amat" );
			break;
		case ANFT_Composite:
			return wxT( ".cmpc" );
			break;
		case ANFT_HavokCloth:
			return wxT( ".hvkc" );
			break;
		case ANFT_Animation:
			return wxT( ".anic" );
			break;
		case ANFT_EmoteAnimation:
			return wxT( ".emoc" );
			break;

		case ANFT_Cinematic:
			return wxT( ".cinp" );
			break;

		case ANFT_PortalManifest:
			return wxT( ".prlt" );
			break;

		case ANFT_TextPackManifest:
			return wxT( ".txtm" );
			break;
		case ANFT_TextPackVoices:
			return wxT( ".txtv" );
			break;

		// Sound
		case ANFT_Sound:
			return wxT( ".asnd" );
			break;

		// Audio script
		case ANFT_AudioScript:
			return wxT( ".amsp" );
			break;

		// Binary
		case ANFT_DLL:
			return wxT( ".dll" );
			break;
		case ANFT_EXE:
			return wxT( ".exe" );
			break;

		default:
			return wxT( ".raw" );
			break;
		}
	}

	FileReader* FileReader::readerForFileType( const Array<byte>& p_data, ANetFileType p_fileType ) {
		return new FileReader( p_data, p_fileType );
	}

	FileReader* FileReader::readerForData( const Array<byte>& p_data, ANetFileType p_fileType ) {
		switch ( p_fileType ) {
		case ANFT_ATEX:
		case ANFT_ATTX:
		case ANFT_ATEC:
		case ANFT_ATEP:
		case ANFT_ATEU:
		case ANFT_ATET:
		case ANFT_DDS:
		case ANFT_JPEG:
			if ( ImageReader::isValidHeader( p_data.GetPointer(), p_data.GetSize() ) ) {
				return new ImageReader( p_data, p_fileType );
			}
			break;
		case ANFT_Model:
			return new ModelReader( p_data, p_fileType );
			break;
		default:
			break;
		}
		return new FileReader( p_data, p_fileType );
	}

}; // namespace gw2b
