/* \file       Viewers/BinaryViewer/HexControl.h
*  \brief      Contains declaration of the hex view control.
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

#ifndef VIEWERS_BINARYVIEWER_HEXCONTROL_H_INCLUDED
#define VIEWERS_BINARYVIEWER_HEXCONTROL_H_INCLUDED

#include <wx/scrolwin.h>

namespace gw2b {

	class HexControl : public wxScrolledWindow {
		const byte* m_data;
		uint        m_dataSize;
	private:
		enum {
			OUTER_SPACING = 4
		};
		enum {
			LINE_SPACING = 4
		};
		enum {
			HEX_COLUMN_SPACING = 4
		};
		enum {
			SECTION_SPACING = 0x18
		};
		enum {
			BYTES_PER_LINE = 0x10
		};
	private:
		struct RedrawState {
			wxRect  clipping;
			wxRect  offsetArea;
			wxRect  hexArea;
			wxRect  textArea;
			wxSize  charSize;
			uint    numLines;
			uint    lineHeight;
			uint    firstLine;
			uint    lastLine;
		};
	public:
		HexControl( wxWindow* p_parent, const wxPoint& p_position = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
		void onDraw( wxDC& p_DC, wxRect& p_region );
		void setData( const byte* p_data, uint p_size );
	private:
		void updateScrollbars( wxDC& p_DC, RedrawState& p_state );
		void drawOffsets( wxDC& p_DC, RedrawState& p_state );
		void drawHexArea( wxDC& p_DC, RedrawState& p_state );
		void drawTextArea( wxDC& p_DC, RedrawState& p_state );
		char filterTextChar( byte p_char );

		void onPaintEvt( wxPaintEvent& p_event );
	};

};

#endif // VIEWERS_BINARYVIEWER_HEXCONTROL_H_INCLUDED
