/* \file       Viewers/BinaryViewer.cpp
*  \brief      Contains definition of the hex viewer.
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

#include "BinaryViewer.h"

#include "HexControl.h"
#include "FileReader.h"

namespace gw2b {

	BinaryViewer::BinaryViewer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
		: Viewer( p_parent, p_pos, p_size )
		, m_hexControl( nullptr ) {
		auto sizer = new wxBoxSizer( wxHORIZONTAL );

		// Hex control
		m_hexControl = new HexControl( this );
		sizer->Add( m_hexControl, wxSizerFlags( ).Expand( ).Proportion( 1 ) );

		// Layout
		this->SetSizer( sizer );
		this->Layout( );
	}

	BinaryViewer::~BinaryViewer( ) {
	}

	void BinaryViewer::clear( ) {
		m_hexControl->setData( nullptr, 0 );
		m_binaryData.Clear( );
		Viewer::clear( );
	}

	void BinaryViewer::setReader( FileReader* pReader ) {
		Viewer::setReader( pReader );

		if ( pReader ) {
			m_binaryData = pReader->convertData( );
			m_hexControl->setData( m_binaryData.GetPointer( ), m_binaryData.GetSize( ) );
		}
	}

}; // namespace gw2b
