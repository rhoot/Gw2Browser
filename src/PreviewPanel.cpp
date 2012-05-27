/** \file       PreviewPanel.cpp
 *  \brief      Contains definition of the preview panel control.
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
#include "PreviewPanel.h"

#include "DatFile.h"
#include "DatIndex.h"

#include "Viewers/BinaryViewer.h"
#include "Viewers/ImageViewer.h"

namespace gw2b
{

PreviewPanel::PreviewPanel(wxWindow* pParent, const wxPoint& pLocation, const wxSize& pSize)
    : wxPanel(pParent, wxID_ANY, pLocation, pSize)
    , mCurrentView(NULL)
    , mCurrentDataType(FileReader::DT_None)
{
    // FINE I'LL USE A GOD DAMN SIZER STUPID WXWIDGETS
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(sizer);
}

PreviewPanel::~PreviewPanel()
{
}

bool PreviewPanel::PreviewFile(DatFile& pDatFile, const DatIndexEntry& pEntry)
{
    Array<byte> entryData = pDatFile.ReadFile(pEntry.GetMftEntry());
    if (!entryData.GetSize()) { return false; }

    // Create file reader
    FileReader* reader = FileReader::GetReaderForData(entryData, pEntry.GetFileType());

    if (reader) {
        if (mCurrentView) {
            // Check if we can re-use the current viewer
            if (mCurrentDataType == reader->GetDataType()) {
                mCurrentView->SetReader(reader);
                return true;
            }
        
            // Destroy the old viewer
            if (mCurrentView) {
                this->GetSizer()->Remove(0);
                mCurrentView->Destroy();
            }
        }

        mCurrentView = this->CreateViewerForDataType(reader->GetDataType());
        if (mCurrentView) {
            // Workaround for wxWidgets fuckups
            this->GetSizer()->Add(mCurrentView, wxSizerFlags().Expand().Proportion(1));
            this->GetSizer()->Layout();
            this->GetSizer()->Fit(this);
            // Set the reader
            mCurrentView->SetReader(reader);
            mCurrentDataType = reader->GetDataType();
            return true;
        }
    }

    return false;
}

Viewer* PreviewPanel::CreateViewerForDataType(FileReader::DataType pDataType)
{
    switch (pDataType) {
    case FileReader::DT_Image:
        return new ImageViewer(this);
    case FileReader::DT_Binary:
    default:
        return new BinaryViewer(this);
    }
}

}; // namespace gw2b
