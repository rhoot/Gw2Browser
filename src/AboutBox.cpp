/* \file       AboutBox.h
*  \brief      Contains declaration of the about box dialog.
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
#include "AboutBox.h"

#include "Data.h"

namespace gw2b {

	AboutBox::AboutBox( wxWindow* p_parent )
		: wxDialog( p_parent, wxID_ANY, wxT( "About Gw2Browser" ), wxDefaultPosition, wxSize( 400, 384 ), wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxTAB_TRAVERSAL ) {
		// Set colors
		this->SetBackgroundColour( wxColor( ( ulong ) 0xffffff ) );
		this->SetForegroundColour( wxColor( ( ulong ) 0x000000 ) );

		// Make sure the *client size* is 400x384 and not the outer size
		this->SetClientSize( 400, 384 );

		// Create bitmap at top
		auto bitmap = data::loadJPG( data::about_box_jpg, data::about_box_jpg_size );
		auto bitmapBox = new wxStaticBitmap( this, wxID_ANY, bitmap, wxPoint( 0, 0 ), wxSize( 400, 384 ) );
	}

	//============================================================================/

	AboutBox::~AboutBox( ) {
	}

}; // namespace gw2b
