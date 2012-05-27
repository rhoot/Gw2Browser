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

ExtractFilesWindow::ExtractFilesWindow(const Array<const DatIndexEntry*>& pEntries, DatFile& pDatFile, const wxString& pPath, ExtractionMode pMode)
    : wxFrame(NULL, wxID_ANY, wxT("ProxyWindow"))
    , mDatFile(pDatFile)
    , mEntries(pEntries)
    , mProgress(NULL)
    , mPath(pPath)
    , mCurrentProgress(0)
    , mMode(pMode)
{
    this->Hide();
    if (pEntries.GetSize() == 0) {
        this->Destroy();
        return;
    }

    // Init progress dialog
    wxString title = wxString::Format(wxT("Extracting %d %s..."), pEntries.GetSize(), (pEntries.GetSize() == 1 ? wxT("file") : wxT("files")));
    mProgress = new wxProgressDialog(title, wxT("Preparing to extract..."), pEntries.GetSize(), this, wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME);
    mProgress->Show();

    // Register the idle event
    this->Connect(wxEVT_IDLE, wxIdleEventHandler(ExtractFilesWindow::OnIdleEvt));
}

void ExtractFilesWindow::OnIdleEvt(wxIdleEvent& pEvent)
{
    // DONE
    if (mCurrentProgress >= mEntries.GetSize()) {
        DeletePointer(mProgress);
        this->Disconnect(wxEVT_IDLE, wxIdleEventHandler(ExtractFilesWindow::OnIdleEvt));
        this->Destroy();
        return;
    }

    // Extract current file
    this->ExtractFile(*mEntries[mCurrentProgress]);
    mProgress->Update(mCurrentProgress, wxString::Format(wxT("Extracting file %d/%d..."), mCurrentProgress, mEntries.GetSize()));
    mCurrentProgress++;
    pEvent.RequestMore();
}

void ExtractFilesWindow::ExtractFile(const DatIndexEntry& pEntry)
{
    wxFileName filename(mPath, pEntry.GetName());
    this->AppendPaths(filename, *pEntry.GetCategory());

    // Create directory if inexistant
    if (!filename.DirExists()) {
        filename.Mkdir(511, wxPATH_MKDIR_FULL);
    }

    // Read file contents
    Array<byte> contents = mDatFile.ReadFile(pEntry.GetMftEntry());
    if (!contents.GetSize()) { return; }

    // Should we convert the files first?
    FileReader* reader = NULL;
    if (mMode == EM_Converted) {
        ANetFileType fileType = ANFT_Unknown;
        mDatFile.IdentifyFileType(contents.GetPointer(), contents.GetSize(), fileType);
        reader = FileReader::GetReaderForData(contents, fileType);

        if (reader) {
            contents = reader->ConvertData();
            filename.SetExt(reader->GetExtension());
        }
    }

    // Open file for writing
    wxFile file(filename.GetFullPath(), wxFile::write);
    if (file.IsOpened()) {
        file.Write(contents.GetPointer(), contents.GetSize());
    }
    file.Close();

    DeletePointer(reader);
}

void ExtractFilesWindow::AppendPaths(wxFileName& pPath, const DatIndexCategory& pCategory)
{
    const DatIndexCategory* parent = pCategory.GetParent();
    if (parent) { this->AppendPaths(pPath, *parent); }
    pPath.AppendDir(pCategory.GetName());
}

}; // namespace gw2b
