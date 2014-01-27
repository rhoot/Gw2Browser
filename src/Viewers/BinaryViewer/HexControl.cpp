/* \file       Viewers/BinaryViewer/HexControl.cpp
*  \brief      Contains declaration of the hex view control.
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
#include <wx/dcbuffer.h>

#include "HexControl.h"

// GOD DAMNIT MICROSOFT, STOP DEFINING RANDOM DEFINES
#ifdef DrawText
#  undef DrawText
#endif

namespace gw2b {

	HexControl::HexControl( wxWindow* p_parent, const wxPoint& p_position, const wxSize& p_size )
		: wxScrolledWindow( p_parent, wxID_ANY, p_position, p_size, wxBORDER_THEME )
		, m_data( nullptr )
		, m_dataSize( 0 ) {
		this->SetBackgroundStyle( wxBG_STYLE_CUSTOM );
		this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
		this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ) );
		wxFont hexFont( 10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
		this->SetFont( hexFont );

		this->Connect( wxEVT_PAINT, wxPaintEventHandler( HexControl::onPaintEvt ) );
	}

	void HexControl::onDraw( wxDC& p_DC, wxRect& p_region ) {
		RedrawState state;

		// Create a buffer bitmap to draw to
		wxSize clientSize( this->GetClientSize( ) );
		wxBitmap buffer( clientSize.x, clientSize.y );

		// Get sizes of stuff
		//p_DC.GetClippingBox(state.mClipping);
		state.charSize.Set( p_DC.GetCharWidth( ), p_DC.GetCharHeight( ) );
		state.clipping = p_region;
		state.numLines = ( m_dataSize == 0 ? 1 : ( ( m_dataSize - 1 ) >> 4 ) + 1 );
		state.lineHeight = state.charSize.x + LINE_SPACING;
		state.firstLine = wxMax( 0, ( state.clipping.y - OUTER_SPACING ) / static_cast<int>( state.lineHeight ) );
		state.lastLine = wxMin( state.numLines - 1, ( state.clipping.GetBottom( ) - OUTER_SPACING ) / static_cast<int>( state.lineHeight ) );

		// Calculate the offset area
		state.offsetArea.x = OUTER_SPACING;
		state.offsetArea.y = OUTER_SPACING;
		state.offsetArea.width = ( state.charSize.x * 9 );
		state.offsetArea.height = ( state.numLines * ( state.lineHeight ) ) - LINE_SPACING;

		// Calculate the hex area
		state.hexArea.x = state.offsetArea.GetRight( ) + SECTION_SPACING;
		state.hexArea.y = OUTER_SPACING;
		state.hexArea.width = ( ( state.charSize.x * 2 + HEX_COLUMN_SPACING ) * BYTES_PER_LINE ) - HEX_COLUMN_SPACING;
		state.hexArea.height = state.offsetArea.height;

		// Calculate the text area
		state.textArea.x = state.hexArea.GetRight( ) + SECTION_SPACING;
		state.textArea.y = OUTER_SPACING;
		state.textArea.width = ( state.charSize.x * BYTES_PER_LINE );
		state.textArea.height = state.offsetArea.height;

		// Perform the redrawing
		this->updateScrollbars( p_DC, state );
		this->drawOffsets( p_DC, state );
		this->drawHexArea( p_DC, state );
		this->drawTextArea( p_DC, state );
	}

	void HexControl::updateScrollbars( wxDC& p_DC, RedrawState& p_state ) {
		uint totalWidth = p_state.textArea.GetRight( ) + OUTER_SPACING;
		uint totalHeight = p_state.offsetArea.GetBottom( ) + OUTER_SPACING;
		this->SetVirtualSize( totalWidth, totalHeight );
		this->SetScrollRate( p_state.charSize.x, p_state.charSize.y + LINE_SPACING );
	}

	void HexControl::drawOffsets( wxDC& p_DC, RedrawState& p_state ) {
		// Don't redraw unless necessary
		if ( !p_state.offsetArea.Intersects( p_state.clipping ) ) {
			return;
		}

		// Loop through the lines and draw each one
		for ( uint i = p_state.firstLine; i <= p_state.lastLine; i++ ) {
			uint left = p_state.offsetArea.x;
			uint top = p_state.offsetArea.y + ( i * p_state.lineHeight );
			p_DC.DrawText( wxString::Format( wxT( "%08xh" ), i * BYTES_PER_LINE ), left, top );
		}
	}

	void HexControl::drawHexArea( wxDC& p_DC, RedrawState& p_state ) {
		// Don't redraw unless necessary
		if ( !p_state.hexArea.Intersects( p_state.clipping ) ) {
			return;
		}

		// Loop through the lines
		int byteWidth = ( p_state.charSize.x * 2 ) + HEX_COLUMN_SPACING;
		for ( uint i = p_state.firstLine; i <= p_state.lastLine; i++ ) {
			// Determine the characters that need drawing
			int firstChar = ( p_state.clipping.x - p_state.hexArea.x ) / byteWidth;
			int lastChar = ( p_state.clipping.GetRight( ) - p_state.hexArea.x ) / byteWidth;
			firstChar = wxMax( 0, firstChar );
			lastChar = wxMin( BYTES_PER_LINE - 1, lastChar );

			if ( firstChar >= static_cast<int>( m_dataSize ) ) {
				break;
			}

			// Display the byte
			for ( int j = firstChar; j <= lastChar; j++ ) {
				uint index = ( i * BYTES_PER_LINE ) + j;
				if ( index >= m_dataSize ) {
					break;
				}
				uint left = p_state.hexArea.x + ( j * byteWidth );
				uint top = p_state.hexArea.y + ( i * p_state.lineHeight );
				p_DC.DrawText( wxString::Format( wxT( "%02x" ), m_data[index] ), left, top );
			}
		}
	}

	void HexControl::drawTextArea( wxDC& p_DC, RedrawState& p_state ) {
		// Don't redraw unless necessary
		if ( !p_state.textArea.Intersects( p_state.clipping ) ) {
			return;
		}

		// Loop through the lines
		for ( uint i = p_state.firstLine; i <= p_state.lastLine; i++ ) {
			// Determine the characters that need drawing
			int firstChar = ( p_state.clipping.x - p_state.textArea.x ) / p_state.charSize.x;
			int lastChar = ( p_state.clipping.GetRight( ) - p_state.textArea.x ) / p_state.charSize.x;
			firstChar = wxMax( 0, firstChar );
			lastChar = wxMin( BYTES_PER_LINE - 1, lastChar );

			if ( firstChar >= static_cast<int>( m_dataSize ) ) {
				break;
			}

			// Build the ouput string
			wxString output;
			for ( int j = firstChar; j <= lastChar; j++ ) {
				uint index = ( i * BYTES_PER_LINE ) + j;
				if ( index >= m_dataSize ) {
					break;
				}
				char sign = this->filterTextChar( m_data[index] );
				output.Append( sign );
			}

			// Display the bytes
			uint left = p_state.textArea.x + ( firstChar * p_state.charSize.x );
			uint top = p_state.textArea.y + ( i * p_state.lineHeight );
			p_DC.DrawText( output, left, top );
		}
	}

	char HexControl::filterTextChar( byte pChar ) {
		//if ( pChar != 173 && ( ( pChar > 31 && pChar < 127 ) || pChar > 159 ) ) {
		// Display only printable ASCII character
		if ( pChar > 31 && pChar < 127 ) {
			return pChar;
		} else {
			return '.';
		}
	}

	void HexControl::setData( const byte* pData, uint p_size ) {
		if ( p_size ) {
			Ensure::notNull( pData );
			m_data = pData;
			m_dataSize = p_size;
		} else {
			m_data = nullptr;
			m_dataSize = 0;
		}

		this->Refresh( );
	}

	void HexControl::onPaintEvt( wxPaintEvent& pEvent ) {
		wxAutoBufferedPaintDC dc( this );
		dc.Clear( );

		int vX, vY, pX, pY;
		this->GetViewStart( &vX, &vY );
		this->GetScrollPixelsPerUnit( &pX, &pY );
		vX *= pX;
		vY *= pY;

		auto pt = dc.GetDeviceOrigin( );
		dc.SetDeviceOrigin( pt.x - vX, pt.y - vY );
		dc.SetUserScale( this->GetScaleX( ), this->GetScaleY( ) );

		for ( wxRegionIterator upd( this->GetUpdateRegion( ) ); upd; upd++ ) {
			wxRect updRect( upd.GetRect( ) );
			updRect.x += vX;
			updRect.y += vY;
			this->onDraw( dc, updRect );
		}
	}

}; // namespace gw2b
