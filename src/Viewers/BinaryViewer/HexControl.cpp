/** \file       Viewers/BinaryViewer/HexControl.h
 *  \brief      Contains declaration of the hex view control.
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

#include "HexControl.h"

namespace gw2b
{

HexControl::HexControl(wxWindow* pParent, const wxPoint& pPosition, const wxSize& pSize)
    : wxScrolledWindow(pParent, wxID_ANY, pPosition, pSize, wxBORDER_THEME)
    , mData(NULL)
    , mDataSize(0)
{
    this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    this->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    this->SetFont(wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT));

    this->Connect(wxEVT_PAINT,  wxPaintEventHandler(HexControl::OnPaintEvt));
}

void HexControl::OnDraw(wxDC& pDC, wxRect& pRegion)
{
    RedrawState state;

    // Create a buffer bitmap to draw to
    wxSize clientSize(this->GetClientSize());
    wxBitmap buffer(clientSize.x, clientSize.y);
    
    // Get sizes of stuff
    //pDC.GetClippingBox(state.mClipping);
    state.mCharSize.Set(pDC.GetCharWidth(), pDC.GetCharHeight());
    state.mClipping   = pRegion;
    state.mNumLines   = (mDataSize == 0 ? 1 : ((mDataSize - 1) >> 4) + 1);
    state.mLineHeight = state.mCharSize.x + LINE_SPACING;
    state.mFirstLine  = wxMax(0, (state.mClipping.y - OUTER_SPACING) / (int)state.mLineHeight);
    state.mLastLine   = wxMin(state.mNumLines - 1, (state.mClipping.GetBottom() - OUTER_SPACING) / (int)state.mLineHeight);

    // Calculate the offset area
    state.mOffsetArea.x      = OUTER_SPACING;
    state.mOffsetArea.y      = OUTER_SPACING;
    state.mOffsetArea.width  = (state.mCharSize.x * 9);
    state.mOffsetArea.height = (state.mNumLines * (state.mLineHeight)) - LINE_SPACING;

    // Calculate the hex area
    state.mHexArea.x         = state.mOffsetArea.GetRight() + SECTION_SPACING;
    state.mHexArea.y         = OUTER_SPACING;
    state.mHexArea.width     = ((state.mCharSize.x * 2 + HEX_COLUMN_SPACING) * BYTES_PER_LINE) - HEX_COLUMN_SPACING;
    state.mHexArea.height    = state.mOffsetArea.height;

    // Calculate the text area
    state.mTextArea.x        = state.mHexArea.GetRight() + SECTION_SPACING;
    state.mTextArea.y        = OUTER_SPACING;
    state.mTextArea.width    = (state.mCharSize.x * BYTES_PER_LINE);
    state.mTextArea.height   = state.mOffsetArea.height;

    // Perform the redrawing
    this->UpdateScrollbars(pDC, state);
    this->DrawOffsets(pDC, state);
    this->DrawHexArea(pDC, state);
    this->DrawTextArea(pDC, state);
}

void HexControl::UpdateScrollbars(wxDC& pDC, RedrawState& pState)
{
    uint totalWidth  = pState.mTextArea.GetRight() + OUTER_SPACING;
    uint totalHeight = pState.mOffsetArea.GetBottom() + OUTER_SPACING;
    this->SetVirtualSize(totalWidth, totalHeight);
    this->SetScrollRate(pState.mCharSize.x, pState.mCharSize.y + LINE_SPACING);
}

void HexControl::DrawOffsets(wxDC& pDC, RedrawState& pState)
{
    // Don't redraw unless necessary
    if (!pState.mOffsetArea.Intersects(pState.mClipping)) { return; }

    // Loop through the lines and draw each one
    for (uint i = pState.mFirstLine; i <= pState.mLastLine; i++) {
        uint left = pState.mOffsetArea.x;
        uint top  = pState.mOffsetArea.y + (i * pState.mLineHeight);
        pDC.DrawText(wxString::Format(wxT("%08xh"), i * BYTES_PER_LINE), left, top);
    }
}

void HexControl::DrawHexArea(wxDC& pDC, RedrawState& pState)
{
    // Don't redraw unless necessary
    if (!pState.mHexArea.Intersects(pState.mClipping)) { return; }

    // Loop through the lines
    int byteWidth = (pState.mCharSize.x * 2) + HEX_COLUMN_SPACING;
    for (uint i = pState.mFirstLine; i <= pState.mLastLine; i++) {
        // Determine the characters that need drawing
        int firstChar = (pState.mClipping.x - pState.mHexArea.x) / byteWidth;
        int lastChar  = (pState.mClipping.GetRight() - pState.mHexArea.x) / byteWidth;
        firstChar = wxMax(0, firstChar);
        lastChar  = wxMin(BYTES_PER_LINE - 1, lastChar);

        // Display the byte
        for (int j = firstChar; j <= lastChar; j++) {
            uint left = pState.mHexArea.x + (j * byteWidth);
            uint top  = pState.mHexArea.y + (i * pState.mLineHeight);
            pDC.DrawText(wxString::Format(wxT("%02x"), mData[(i * BYTES_PER_LINE) + j]), left, top);
        }
    }
}

void HexControl::DrawTextArea(wxDC& pDC, RedrawState& pState)
{
    // Don't redraw unless necessary
    if (!pState.mTextArea.Intersects(pState.mClipping)) { return; }

    // Loop through the lines
    for (uint i = pState.mFirstLine; i <= pState.mLastLine; i++) {
        // Determine the characters that need drawing
        int firstChar = (pState.mClipping.x - pState.mTextArea.x) / pState.mCharSize.x;
        int lastChar  = (pState.mClipping.GetRight() - pState.mTextArea.x) / pState.mCharSize.x;
        firstChar = wxMax(0, firstChar);
        lastChar  = wxMin(BYTES_PER_LINE - 1, lastChar);

        // Build the ouput string
        wxString output;
        for (int j = firstChar; j <= lastChar; j++) {
            char sign = this->FilterTextChar(mData[(i * BYTES_PER_LINE) + j]);
            output.Append(sign);
        }

        // Display the bytes
        uint left = pState.mTextArea.x + (firstChar * pState.mCharSize.x);
        uint top  = pState.mTextArea.y + (i * pState.mLineHeight);
        pDC.DrawText(output, left, top);
    }
}

char HexControl::FilterTextChar(byte pChar)
{
	if (pChar != 173 && ((pChar > 31 && pChar < 127) || pChar > 159))
        return pChar;
    else
        return '.';
}

void HexControl::SetData(const byte* pData, uint pSize)
{
    if (pSize) {
        Ensure::NotNull(pData);
        mData     = pData;
        mDataSize = pSize;
    } else {
        mData     = NULL;
        mDataSize = 0;
    }

    this->Refresh();
}

void HexControl::OnPaintEvt(wxPaintEvent& pEvent)
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
