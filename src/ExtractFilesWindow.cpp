 /** \file      ExtractFilesWindow.h
 *  \brief      Contains definition of the file extraction window.
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
#include <wx/filename.h>

#include "DatFile.h"
#include "DatIndex.h"
#include "ExtractFilesWindow.h"
#include "FileReader.h"

namespace gw2b
{

ExtractFilesWindow::ExtractFilesWindow(const Array<const DatIndexEntry*>& p_entries, DatFile& p_datFile, const wxString& p_path, ExtractionMode p_mode)
    : wxFrame(nullptr, wxID_ANY, wxT("ProxyWindow"))
    , m_datFile(p_datFile)
    , m_entries(p_entries)
    , m_progress(nullptr)
    , m_path(p_path)
    , m_currentProgress(0)
    , m_mode(p_mode)
{
    this->Hide();
    if (p_entries.GetSize() == 0) {
        this->Destroy();
        return;
    }

    // Init progress dialog
    auto title = wxString::Format(wxT("Extracting %d %s..."), p_entries.GetSize(), (p_entries.GetSize() == 1 ? wxT("file") : wxT("files")));
    m_progress = new wxProgressDialog(title, wxT("Preparing to extract..."), p_entries.GetSize(), this, wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME);
    m_progress->Show();

    // Register the idle event
    this->Connect(wxEVT_IDLE, wxIdleEventHandler(ExtractFilesWindow::onIdleEvt));
}

void ExtractFilesWindow::onIdleEvt(wxIdleEvent& p_event)
{
    // DONE
    if (m_currentProgress >= m_entries.GetSize()) {
        deletePointer(m_progress);
        this->Disconnect(wxEVT_IDLE, wxIdleEventHandler(ExtractFilesWindow::onIdleEvt));
        this->Destroy();
        return;
    }

    // Extract current file
    this->extractFile(*m_entries[m_currentProgress]);
    m_progress->Update(m_currentProgress, wxString::Format(wxT("Extracting file %d/%d..."), m_currentProgress, m_entries.GetSize()));
    m_currentProgress++;
    p_event.RequestMore();
}

void ExtractFilesWindow::extractFile(const DatIndexEntry& p_entry)
{
    wxFileName filename(m_path, p_entry.name());
    this->appendPaths(filename, *p_entry.category());

    // Create directory if in-existant
    if (!filename.DirExists()) {
        filename.Mkdir(511, wxPATH_MKDIR_FULL);
    }

    // Read file contents
    auto contents = m_datFile.readFile(p_entry.mftEntry());
    if (!contents.GetSize()) { return; }

    // Should we convert the files first?
    FileReader* reader = nullptr;
    if (m_mode == EM_Converted) {
        auto fileType = ANFT_Unknown;
        m_datFile.identifyFileType(contents.GetPointer(), contents.GetSize(), fileType);
        reader = FileReader::readerForData(contents, fileType);

        if (reader) {
            contents = reader->convertData();
            filename.SetExt(reader->extension());
        }
    }

    // Open file for writing
    wxFile file(filename.GetFullPath(), wxFile::write);
    if (file.IsOpened()) {
        file.Write(contents.GetPointer(), contents.GetSize());
    }
    file.Close();

    deletePointer(reader);
}

void ExtractFilesWindow::appendPaths(wxFileName& p_path, const DatIndexCategory& p_category)
{
    auto parent = p_category.parent();
    if (parent) { this->appendPaths(p_path, *parent); }
    p_path.AppendDir(p_category.name());
}

}; // namespace gw2b
