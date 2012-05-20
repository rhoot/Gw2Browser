/** \file       Viewers/ImageViewer/ImageControl.cpp
 *  \brief      Contains definition of the image control.
 *  \author     Rhoot
 */

/*	Copyright (C) 2012 Rhoot <https://github.com/rhoot>

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

#include "Data.h"
#include "ImageControl.h"

namespace gw2b
{

ImageControl::ImageControl(wxWindow* pParent, const wxPoint& pPosition, const wxSize& pSize)
    : wxScrolledWindow(pParent, wxID_ANY, pPosition, pSize)
    , mImage(NULL)
    , mChannels(IC_All)
{    
    mBackdrop = data::LoadPNG(data::checkers_png, data::checkers_png_size);
    this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
    this->Connect(wxEVT_PAINT,  wxPaintEventHandler(ImageControl::OnPaintEvt));
}

ImageControl::~ImageControl()
{
}

void ImageControl::SetImage(wxImage* pImage)
{
    mImage = pImage;
    this->UpdateBitmap();
}

void ImageControl::UpdateBitmap()
{
    mBitmap = wxBitmap();

    if (mImage && mImage->IsOk()) {
        mBitmap.Create(mImage->GetWidth(), mImage->GetHeight());

        // Fill with backdrop
        wxMemoryDC dc(mBitmap);
        for (uint y = 0; y < (uint)mBitmap.GetHeight(); y += mBackdrop.GetHeight()) {
            for (uint x = 0; x < (uint)mBitmap.GetWidth(); x += mBackdrop.GetWidth()) {
                dc.DrawBitmap(mBackdrop, x, y);
            }
        }
        wxImage image(*mImage);

        // Check if any channels have been toggled off
        if (mChannels != IC_All) {
            uint numPixels = image.GetWidth() * image.GetHeight();
            uint8* alphaCache = NULL;

            // Any colors toggled off?
            if (!(mChannels & IC_Red) || !(mChannels & IC_Green) || !(mChannels & IC_Blue)) {
                uint8* colors = Alloc<uint8>(numPixels * 3);
                ::memcpy(colors, image.GetData(), numPixels * 3);

                // Setting colors clears the alpha, so cache it for restoration later,
                // if the alpha channel is visible
                if (mChannels & IC_Alpha) {
                    alphaCache = Alloc<uint8>(numPixels);
                    ::memcpy(alphaCache, image.GetAlpha(), numPixels);
                }

                // If all colors are off, but alpha is on, alpha should be made white
                bool noColors   = (!(mChannels & IC_Red) && !(mChannels & IC_Green) && !(mChannels & IC_Blue));
                bool whiteAlpha = noColors && (mChannels & IC_Alpha);

                // Loop through the pixels
                for (uint i = 0; i < numPixels; i++) {
                    if (whiteAlpha) {
                        ::memset(&colors[i * 3], alphaCache[i], sizeof(uint8) * 3);
                    } else {
                        // Red turned off?
                        if (!(mChannels & IC_Red))   { colors[i * 3 + 0] = 0x00; }
                        // Green turned off?
                        if (!(mChannels & IC_Green)) { colors[i * 3 + 1] = 0x00; }
                        // Blue turned off?
                        if (!(mChannels & IC_Blue))  { colors[i * 3 + 2] = 0x00; }
                    }
                }

                // If alpha should be white, it should not be alpha too
                if (whiteAlpha) {
                    FreePointer(alphaCache);
                }

                // Update colors
                image.SetData(colors);
            }

            // Was alpha turned off?
            if (!(mChannels & IC_Alpha)) {
                uint8* alpha = Alloc<uint8>(numPixels);
                ::memset(alpha, 0xff, numPixels);
                image.SetAlpha(alpha);
            } else if (alphaCache) {
                image.SetAlpha(alphaCache);
            }
        }

        // Draw the image
        dc.DrawBitmap(wxBitmap(image), 0, 0);
    }

    this->Refresh();
}

void ImageControl::ToggleChannel(ImageChannels pChannel, bool pToggled)
{
    bool isDirty = false;

    // Red
    if (pChannel == IC_Red && !!(mChannels & IC_Red) != pToggled) {
        mChannels = (ImageChannels)(pToggled ? (mChannels | IC_Red) : (mChannels & ~IC_Red));
        isDirty   = true;

    // Green
    } else if (pChannel == IC_Green && !!(mChannels & IC_Green) != pToggled) {
        mChannels = (ImageChannels)(pToggled ? (mChannels | IC_Green) : (mChannels & ~IC_Green));
        isDirty   = true;

    // Blue
    } else if (pChannel == IC_Blue && !!(mChannels & IC_Blue) != pToggled) {
        mChannels = (ImageChannels)(pToggled ? (mChannels | IC_Blue) : (mChannels & ~IC_Blue));
        isDirty   = true;

    // Alpha
    } else if (pChannel == IC_Alpha && !!(mChannels & IC_Alpha) != pToggled) {
        mChannels = (ImageChannels)(pToggled ? (mChannels | IC_Alpha) : (mChannels & ~IC_Alpha));
        isDirty   = true;
    }

    if (isDirty) {
        this->UpdateBitmap();
    }
}

void ImageControl::OnDraw(wxDC& pDC, wxRect& pRegion)
{
    this->UpdateScrollbars(pDC);
    if (mBitmap.IsOk()) {
        pDC.DrawBitmap(mBitmap, 0, 0);
    }
}

void ImageControl::UpdateScrollbars(wxDC& pDC)
{
    if (mBitmap.IsOk()) {
        this->SetVirtualSize(mBitmap.GetWidth(), mBitmap.GetHeight());
        this->SetScrollRate(0x20, 0x20);
    } else {
        this->SetVirtualSize(0, 0);
    }
}

void ImageControl::OnPaintEvt(wxPaintEvent& pEvent)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    int vX, vY, pX, pY;
    this->GetViewStart(&vX, &vY);
    this->GetScrollPixelsPerUnit(&pX, &pY);
    vX *= pX;
    vY *= pY;

    wxPoint pt = dc.GetDeviceOrigin();
    dc.SetDeviceOrigin(pt.x - vX, pt.y - vY);
    dc.SetUserScale(this->GetScaleX(), this->GetScaleY());

    for (wxRegionIterator upd(this->GetUpdateRegion()); upd; upd++) {
        wxRect updRect(upd.GetRect());
        updRect.x += vX;
        updRect.y += vY;
        this->OnDraw(dc, updRect);
    }
}

}; // namespace gw2b
