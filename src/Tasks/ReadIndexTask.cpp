/* \file       ReadIndexTask.cpp
*  \brief      Contains declaration of the ReadIndexTask class.
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
#include "ReadIndexTask.h"

namespace gw2b {

	ReadIndexTask::ReadIndexTask( const std::shared_ptr<DatIndex>& p_index, const wxString& p_filename, uint64 p_datTimestamp )
		: m_index( p_index )
		, m_reader( *p_index )
		, m_filename( p_filename )
		, m_errorOccured( false )
		, m_datTimestamp( p_datTimestamp ) {
		Ensure::notNull( p_index.get( ) );
	}

	bool ReadIndexTask::init( ) {
		m_index->clear( );
		m_index->setDirty( false );

		bool result = m_reader.open( m_filename );
		if ( result ) {
			result = ( m_index->datTimestamp( ) == m_datTimestamp );
		}
		if ( result ) {
			this->setMaxProgress( m_reader.numEntries( ) + m_reader.numCategories( ) );
		}

		return result;
	}

	void ReadIndexTask::perform( ) {
		if ( !this->isDone( ) ) {
			m_errorOccured = !( m_reader.read( 7 ) & DatIndexReader::RR_Success );
			if ( m_errorOccured ) {
				m_index->clear( );
			}
			uint progress = m_reader.currentEntry( ) + m_reader.currentCategory( );
			this->setCurrentProgress( progress );
			this->setText( wxT( "Reading .dat index..." ) );
		}
	}

	void ReadIndexTask::abort( ) {
		this->clean( );
	}

	void ReadIndexTask::clean( ) {
		m_reader.close( );
	}

	bool ReadIndexTask::isDone( ) const {
		return ( m_errorOccured || !m_reader.isOpen( ) || m_reader.isDone( ) );
	}

}; // namespace gw2b
