/* \file       Viewers/ImageViewer.cpp
*  \brief      Contains definition of the image viewer.
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
#include "ImageViewer.h"

#include "Viewers/ImageViewer/ImageControl.h"
#include "Readers/ImageReader.h"
#include "Data.h"

namespace gw2b {

	ImageViewer::ImageViewer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
		: Viewer( p_parent, p_pos, p_size )
		, m_imageControl( nullptr ) {
		auto sizer = new wxBoxSizer( wxVERTICAL );

		// Toolbar
		auto toolbar = this->buildToolbar( );
		sizer->Add( toolbar, wxSizerFlags( ).Expand( ).FixedMinSize( ) );

		// Hex control
		m_imageControl = new ImageControl( this );
		sizer->Add( m_imageControl, wxSizerFlags( ).Expand( ).Proportion( 1 ) );

		// Layout
		this->SetSizer( sizer );
		this->Layout( );
	}

	ImageViewer::~ImageViewer( ) {
	}

	void ImageViewer::clear( ) {
		m_imageControl->SetImage( wxImage( ) );
		Viewer::clear( );
	}

	void ImageViewer::setReader( FileReader* p_reader ) {
		Ensure::isOfType<ImageReader>( p_reader );
		Viewer::setReader( p_reader );

		if ( p_reader ) {
			m_image = imageReader( )->getImage( );
			m_imageControl->SetImage( m_image );
		}
	}

	wxToolBar* ImageViewer::buildToolbar( ) {
		auto toolbar = new wxToolBar( this, wxID_ANY );
		auto id = this->NewControlId( 4 );

		// Add the newly generated IDs
		for ( uint i = 0; i < 4; i++ ) {
			m_toolbarButtonIds.Add( id++ );
		}

		// Load all toolbar button icons
		toolbar->SetToolBitmapSize( wxSize( 16, 16 ) );
		m_toolbarButtonIcons.push_back( data::loadPNG( data::toggle_red_png, data::toggle_red_png_size ) );
		m_toolbarButtonIcons.push_back( data::loadPNG( data::toggle_green_png, data::toggle_green_png_size ) );
		m_toolbarButtonIcons.push_back( data::loadPNG( data::toggle_blue_png, data::toggle_blue_png_size ) );
		m_toolbarButtonIcons.push_back( data::loadPNG( data::toggle_alpha_png, data::toggle_alpha_png_size ) );

		// Toggle channel buttons
		for ( uint i = 0; i < 4; i++ ) {
			auto button = new wxToolBarToolBase( toolbar, m_toolbarButtonIds[i], wxEmptyString, m_toolbarButtonIcons[i], m_toolbarButtonIcons[i], wxITEM_CHECK );
			toolbar->AddTool( button );
			button->Toggle( true );
			m_toolbarButtons.Add( button );
			this->Connect( m_toolbarButtonIds[i], wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( ImageViewer::onToolbarClickedEvt ) );
		}

		toolbar->Realize( );
		return toolbar;
	}

	void ImageViewer::onToolbarClickedEvt( wxCommandEvent& pEvent ) {
		auto id = pEvent.GetId( );

		// Toggle red
		if ( id == m_toolbarButtonIds[0] ) {
			m_imageControl->ToggleChannel( ImageControl::IC_Red, m_toolbarButtons[0]->IsToggled( ) );
			// Toggle green
		} else if ( id == m_toolbarButtonIds[1] ) {
			m_imageControl->ToggleChannel( ImageControl::IC_Green, m_toolbarButtons[1]->IsToggled( ) );
			// Toggle blue
		} else if ( id == m_toolbarButtonIds[2] ) {
			m_imageControl->ToggleChannel( ImageControl::IC_Blue, m_toolbarButtons[2]->IsToggled( ) );
			// Toggle alpha
		} else if ( id == m_toolbarButtonIds[3] ) {
			m_imageControl->ToggleChannel( ImageControl::IC_Alpha, m_toolbarButtons[3]->IsToggled( ) );
		} else {
			pEvent.Skip( );
		}
	}

}; // namespace gw2b
