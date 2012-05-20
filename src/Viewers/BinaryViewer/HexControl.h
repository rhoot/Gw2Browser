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

#pragma once

#ifndef VIEWERS_BINARYVIEWER_HEXCONTROL_H_INCLUDED
#define VIEWERS_BINARYVIEWER_HEXCONTROL_H_INCLUDED

#include <wx/scrolwin.h>

namespace gw2b
{

class HexControl : public wxScrolledWindow
{
    const byte* mData;
    uint        mDataSize;
private:
    enum { OUTER_SPACING      = 4 };
    enum { LINE_SPACING       = 4 };
    enum { HEX_COLUMN_SPACING = 4 };
    enum { SECTION_SPACING    = 0x18 };
    enum { BYTES_PER_LINE     = 0x10 };
private:
    struct RedrawState
    {
        wxRect  mClipping;
        wxRect  mOffsetArea;
        wxRect  mHexArea;
        wxRect  mTextArea;
        wxSize  mCharSize;
        uint    mNumLines;
        uint    mLineHeight;
        uint    mFirstLine;
        uint    mLastLine;
    };
public:
    HexControl(wxWindow* pParent, const wxPoint& pPosition = wxDefaultPosition, const wxSize& pSize = wxDefaultSize);
    void OnDraw(wxDC& pDC, wxRect& pRegion);
    void SetData(const byte* pData, uint pSize);
private:
    void UpdateScrollbars(wxDC& pDC, RedrawState& pState);
    void DrawOffsets(wxDC& pDC, RedrawState& pState);
    void DrawHexArea(wxDC& pDC, RedrawState& pState);
    void DrawTextArea(wxDC& pDC, RedrawState& pState);
    char FilterTextChar(byte pChar);

    void OnPaintEvt(wxPaintEvent& pEvent);
};

};

#endif // VIEWERS_BINARYVIEWER_HEXCONTROL_H_INCLUDED
