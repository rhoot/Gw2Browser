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
#include "Viewers/ModelViewer.h"

namespace gw2b
{

PreviewPanel::PreviewPanel(wxWindow* p_parent, const wxPoint& p_location, const wxSize& p_size)
    : wxPanel(p_parent, wxID_ANY, p_location, p_size)
    , m_currentView(nullptr)
    , m_currentDataType(FileReader::DT_None)
{
    // FINE I'LL USE A GOD DAMN SIZER STUPID WXWIDGETS
    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(sizer);
}

PreviewPanel::~PreviewPanel()
{
}

bool PreviewPanel::previewFile(DatFile& p_datFile, const DatIndexEntry& p_entry)
{
    auto entryData = p_datFile.readFile(p_entry.mftEntry());
    if (!entryData.GetSize()) { return false; }

    // Create file reader
    auto reader = FileReader::readerForData(entryData, p_entry.fileType());

    if (reader) {
        if (m_currentView) {
            // Check if we can re-use the current viewer
            if (m_currentDataType == reader->dataType()) {
                m_currentView->setReader(reader);
                return true;
            }
        
            // Destroy the old viewer
            if (m_currentView) {
                this->GetSizer()->Remove(0);
                m_currentView->Destroy();
            }
        }

        m_currentView = this->createViewerForDataType(reader->dataType(), p_datFile);
        if (m_currentView) {
            // Workaround for wxWidgets fuckups
            this->GetSizer()->Add(m_currentView, wxSizerFlags().Expand().Proportion(1));
            this->GetSizer()->Layout();
            this->GetSizer()->Fit(this);
            // Set the reader
            m_currentView->setReader(reader);
            m_currentDataType = reader->dataType();
            return true;
        }
    }

    return false;
}

Viewer* PreviewPanel::createViewerForDataType(FileReader::DataType p_dataType, DatFile& p_datFile)
{
    Viewer* newViewer = nullptr;

    switch (p_dataType) {
    case FileReader::DT_Image:
        newViewer = new ImageViewer(this);
        break;
    case FileReader::DT_Model:
        newViewer = new ModelViewer(this);
        break;
    case FileReader::DT_Binary:
    default:
        newViewer = new BinaryViewer(this);
        break;
    }

    auto needer = dynamic_cast<INeedDatFile*>(newViewer);
    if (needer) { needer->setDatFile(&p_datFile); }

    return newViewer;
}

}; // namespace gw2b
