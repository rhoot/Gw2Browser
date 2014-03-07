/* \file       Viewers/ImageViewer/ImageControl.h
*  \brief      Contains declaration of the image control.
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

#ifndef VIEWERS_IMAGEVIEWER_IMAGECONTROL_H_INCLUDED
#define VIEWERS_IMAGEVIEWER_IMAGECONTROL_H_INCLUDED

#include <wx/scrolwin.h>

namespace gw2b {

	class ImageControl : public wxScrolledWindow {
	public:
		enum ImageChannels {
			IC_None = 0,
			IC_Red = 1,
			IC_Green = 2,
			IC_Blue = 4,
			IC_Alpha = 8,
			IC_All = 15,
		};
	private:
		wxImage         mImage;
		wxBitmap        mBitmap;
		wxBitmap        mBackdrop;
		ImageChannels   mChannels;
	public:
		ImageControl( wxWindow* pParent, const wxPoint& pPosition = wxDefaultPosition, const wxSize& pSize = wxDefaultSize );
		virtual ~ImageControl( );
		void SetImage( wxImage pImage );
		void OnDraw( wxDC& pDC, wxRect& pRegion );
		void ToggleChannel( ImageChannels pChannel, bool pToggled );
	private:
		void UpdateBitmap( );
		void UpdateScrollbars( wxDC& pDC );
		void OnPaintEvt( wxPaintEvent& pEvent );
	};

}; // namespace gw2b

#endif // VIEWERS_IMAGEVIEWER_IMAGECONTROL_H_INCLUDED
