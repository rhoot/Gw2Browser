/** \file       Viewers/BinaryViewer.cpp
 *  \brief      Contains definition of the hex viewer.
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
#include "BinaryViewer.h"
#include "FileReader.h"
#include "BinaryViewer/HexControl.h"

namespace gw2b
{

BinaryViewer::BinaryViewer(wxWindow* pParent, const wxPoint& pPos, const wxSize& pSize)
    : Viewer(pParent, pPos, pSize)
    , mHexControl(NULL)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Hex control
    mHexControl = new HexControl(this);
    sizer->Add(mHexControl, wxSizerFlags().Expand().Proportion(1));

    // Layout
    this->SetSizer(sizer);
    this->Layout();
}

BinaryViewer::~BinaryViewer()
{
}

void BinaryViewer::Clear()
{
    mHexControl->SetData(NULL, 0);
    mBinaryData.Clear();
    Viewer::Clear();
}

void BinaryViewer::SetReader(FileReader* pReader)
{
    Viewer::SetReader(pReader);

    if (pReader) {
        mBinaryData = pReader->ConvertData();
        mHexControl->SetData(mBinaryData.GetPointer(), mBinaryData.GetSize());
    }
}

}; // namespace gw2b
