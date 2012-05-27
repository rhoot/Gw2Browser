/** \file       BrowserWindow.cpp
 *  \brief      Contains definition of the browser window. 
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

#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "Imported/crc.h"
#include "BrowserWindow.h"

#include "AboutBox.h"
#include "DatIndexIO.h"
#include "ExtractFilesWindow.h"
#include "FileReader.h"
#include "ProgressStatusBar.h"
#include "PreviewPanel.h"

#include "Tasks/ReadIndexTask.h"
#include "Tasks/ScanDatTask.h"
#include "Tasks/WriteIndexTask.h"

namespace gw2b
{

BrowserWindow::BrowserWindow(const wxString& pTitle)
    : wxFrame(NULL, wxID_ANY, pTitle, wxDefaultPosition, wxSize(800, 512))
    , mIndex(new DatIndex())
    , mProgress(NULL)
    , mCurrentTask(NULL)
    , mSplitter(NULL)
    , mCatTree(NULL)
    , mPreviewPanel(NULL)
{
    wxMenuBar* menuBar = new wxMenuBar();
    // File menu
    wxMenu* fileMenu = new wxMenu();
    wxAcceleratorEntry accel(wxACCEL_CTRL, 'O');
    fileMenu->Append(wxID_OPEN, wxT("&Open"), wxT("Open a file for browsing"))->SetAccel(&accel);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, wxT("E&xit"));
    // Help menu
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT, wxT("&About Gw2Browser"));
    // Attach menu
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(helpMenu, wxT("&Help"));
    this->SetMenuBar(menuBar);

    // Setup statusbar
    mProgress = new ProgressStatusBar(this);
    this->SetStatusBar(mProgress);

    // Splitter
    mSplitter = new wxSplitterWindow(this);

    // Category tree
    mCatTree = new CategoryTree(mSplitter);
    mCatTree->SetIndex(mIndex);
    mCatTree->AddListener(this);

    // Preview panel
    mPreviewPanel = new PreviewPanel(mSplitter);
    mPreviewPanel->Hide();

    // Initialize splitter
    mSplitter->Initialize(mCatTree);

    // Hook up events
    this->Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BrowserWindow::OnOpenEvt));
    this->Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BrowserWindow::OnExitEvt));
    this->Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BrowserWindow::OnAboutEvt));
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(BrowserWindow::OnCloseEvt));
}

BrowserWindow::~BrowserWindow()
{
    DeletePointer(mCurrentTask);
}

bool BrowserWindow::PerformTask(Task* pTask)
{
    Ensure::NotNull(pTask);

    // Already have a task running?
    if (mCurrentTask) {
        if (mCurrentTask->CanAbort()) {
            mCurrentTask->Abort();
            DeletePointer(mCurrentTask);
            this->Disconnect(wxEVT_IDLE, wxIdleEventHandler(BrowserWindow::OnPerformTaskEvt));
            mProgress->HideProgressBar();
        } else {
            DeletePointer(pTask);
            return false;
        }
    }

    // Initialize succeeded?
    mCurrentTask = pTask;
    if (!mCurrentTask->Init()) {
        DeletePointer(mCurrentTask);
        return false;
    }

    this->Connect(wxEVT_IDLE, wxIdleEventHandler(BrowserWindow::OnPerformTaskEvt));
    mProgress->SetMaxValue(mCurrentTask->GetMaxProgress());
    mProgress->ShowProgressBar();
    return true;
}

void BrowserWindow::OpenFile(const wxString& pPath)
{
    // Try to open the file
    if (!mDatFile.Open(pPath)) {
        wxMessageBox(wxString::Format(wxT("Failed to open file: %s"), pPath), 
            wxMessageBoxCaptionStr, wxOK | wxCENTER | wxICON_ERROR);
        return;
    }
    mDatPath = pPath;

    // Open the index file
    wxFileName indexFile = this->FindDatIndex();
    uint64 datTimeStamp = wxFileModificationTime(pPath);
    ReadIndexTask* readIndexTask = new ReadIndexTask(mIndex, indexFile.GetFullPath(), datTimeStamp);

    // Start reading the index
    readIndexTask->GetOnCompleteHandler() += Task::OnCompleteHandler(this, &BrowserWindow::OnReadIndexComplete);
    if (!this->PerformTask(readIndexTask)) {
        this->ReIndexDat();
    }
}

void BrowserWindow::ViewEntry(const DatIndexEntry& pEntry)
{
    if (mPreviewPanel->PreviewFile(mDatFile, pEntry)) {
        mPreviewPanel->Show();
        // Split it!
        mSplitter->SetMinimumPaneSize(100);
        mSplitter->SplitVertically(mCatTree, mPreviewPanel, mSplitter->GetClientSize().x / 4);
    }
}

wxFileName BrowserWindow::FindDatIndex()
{
    wxString configPath    = wxStandardPaths().GetUserDataDir();
    int datPathCrc         = ::compute_crc(INITIAL_CRC, mDatPath.char_str(), mDatPath.Length());
    wxString indexFileName = wxString::Format(wxT("%x.idx"), (uint)datPathCrc);
    
    return wxFileName(configPath, indexFileName);
}

void BrowserWindow::ReIndexDat()
{
    mIndex->Clear();
    mIndex->SetDatTimeStamp(wxFileModificationTime(mDatPath));
    this->IndexDat();
}

void BrowserWindow::IndexDat()
{
    ScanDatTask* scanTask = new ScanDatTask(mIndex, mDatFile);
    scanTask->GetOnCompleteHandler() += Task::OnCompleteHandler(this, &BrowserWindow::OnScanTaskComplete);
    this->PerformTask(scanTask);
}

void BrowserWindow::OnOpenEvt(wxCommandEvent& WXUNUSED(pEvent))
{
    wxFileDialog dialog(this, wxFileSelectorPromptStr, wxT(""), wxT("Gw2.dat"), 
        wxT("Guild Wars 2 DAT|*.dat"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK) {
        this->OpenFile(dialog.GetPath());
    }
}

void BrowserWindow::OnExitEvt(wxCommandEvent& WXUNUSED(pEvent))
{
    this->Close(true);
}

void BrowserWindow::OnAboutEvt(wxCommandEvent& WXUNUSED(pEvent))
{
    AboutBox about(this);
    about.ShowModal();
}

void BrowserWindow::OnCloseEvt(wxCloseEvent& pEvent)
{
    // Drop out if we can't cancel the window closing
    if (!pEvent.CanVeto()) {
        pEvent.Skip();
        return;
    }

    // Cancel current task if possible.
    if (mCurrentTask) {
        if (mCurrentTask->CanAbort()) {
            mCurrentTask->Abort();
            DeletePointer(mCurrentTask);
            this->Disconnect(wxEVT_IDLE, wxIdleEventHandler(BrowserWindow::OnPerformTaskEvt));
        } else {
            this->Disable();
            mCurrentTask->GetOnCompleteHandler() += Task::OnCompleteHandler(this, &BrowserWindow::TryClose);
            pEvent.Veto();
            return;
        }
    }

    // Add a write task if the index is dirty
    if (!mCurrentTask && mIndex->IsDirty()) {
        wxFileName indexPath = this->FindDatIndex();
        if (!indexPath.DirExists()) { indexPath.Mkdir(511, wxPATH_MKDIR_FULL); }

        WriteIndexTask* writeTask = new WriteIndexTask(mIndex, indexPath.GetFullPath());
        writeTask->GetOnCompleteHandler() += Task::OnCompleteHandler(this, &BrowserWindow::OnWriteTaskCloseCompleted);
        if (this->PerformTask(writeTask)) {
            this->Disable();
            pEvent.Veto();
            return;
        }
    }

    pEvent.Skip();
}

void BrowserWindow::OnPerformTaskEvt(wxIdleEvent& pEvent)
{
    Ensure::NotNull(mCurrentTask);
    mCurrentTask->Perform();

    if (!mCurrentTask->IsDone()) {
        mProgress->Update(mCurrentTask->GetCurrentProgress(), mCurrentTask->GetText());
        pEvent.RequestMore();
    } else {
        this->Disconnect(wxEVT_IDLE, wxIdleEventHandler(BrowserWindow::OnPerformTaskEvt));
        mProgress->SetStatusText(wxEmptyString);
        mProgress->HideProgressBar();
        Task::OnCompleteHandler onComplete = mCurrentTask->GetOnCompleteHandler();
        DeletePointer(mCurrentTask);
        onComplete();
    }
}

void BrowserWindow::OnReadIndexComplete()
{
    // If it failed, it was cleared.
    if (mIndex->GetDatTimeStamp() == 0 || mIndex->GetNumEntries() == 0) {
        this->ReIndexDat();
        return;
    }

    // Was it complete?
    bool isComplete = (mIndex->GetHighestMftEntry() == mDatFile.GetNumFiles());
    if (!isComplete) {
        this->IndexDat();
    }
}

void BrowserWindow::OnScanTaskComplete()
{
    WriteIndexTask* writeTask = new WriteIndexTask(mIndex, this->FindDatIndex().GetFullPath());
    this->PerformTask(writeTask);
}

void BrowserWindow::OnWriteTaskCloseCompleted()
{
    // Forcing this here causes the OnCloseEvt to not try to write the index
    // again. In case it failed the first time, it's likely to fail again and
    // we don't want to get stuck in an infinite loop.
    this->Close(true);
}

void BrowserWindow::TryClose()
{
    this->Close(false);
}

void BrowserWindow::OnTreeEntryClicked(CategoryTree& pTree, const DatIndexEntry& pEntry)
{
    this->ViewEntry(pEntry);
}

void BrowserWindow::OnTreeCategoryClicked(CategoryTree& pTree, const DatIndexCategory& pCategory)
{
    // TODO
}

void BrowserWindow::OnTreeCleared(CategoryTree& pTree)
{
    // TODO
}

void BrowserWindow::OnTreeExtractRaw(CategoryTree& pTree)
{
    Array<const DatIndexEntry*> entries = pTree.GetSelectedEntries();

    if (entries.GetSize()) {
        // If it's just one file, we could handle it here
        if (entries.GetSize() == 1) {
            const DatIndexEntry* entry = entries[0];
            Array<byte> entryData = mDatFile.ReadFile(entry->GetMftEntry());
            
            // Valid data?
            if (!entryData.GetSize()) {
                wxMessageBox(wxT("Failed to extract the file, most likely due to a decompression error."), wxT("Error"), wxOK | wxICON_ERROR);
                return;
            }

            // Ask for location
            wxFileDialog dialog(this, 
                wxString::Format(wxT("Extract %s..."), entry->GetName()), 
                wxEmptyString, 
                entry->GetName(), 
                wxFileSelectorDefaultWildcardStr,
                wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

            if (dialog.ShowModal() == wxID_OK) {
                wxFile file(dialog.GetPath(), wxFile::write);
                if (file.IsOpened()) {
                    file.Write(entryData.GetPointer(), entryData.GetSize());
                    file.Close();
                } else {
                    wxMessageBox(wxT("Failed to open the file for writing."), wxT("Error"), wxOK | wxICON_ERROR);
                }
            }
        }

        // More files than one
        else {
            wxDirDialog dialog(this, wxT("Select output folder"));
            if (dialog.ShowModal() == wxID_OK) {
                new ExtractFilesWindow(entries, mDatFile, dialog.GetPath(), ExtractFilesWindow::EM_Raw);
            }
        }
    }
}

void BrowserWindow::OnTreeExtractConverted(CategoryTree& pTree)
{
    Array<const DatIndexEntry*> entries = pTree.GetSelectedEntries();

    if (entries.GetSize()) {
        // If it's just one file, we could handle it here
        if (entries.GetSize() == 1) {
            const DatIndexEntry* entry = entries[0];
            Array<byte> entryData = mDatFile.ReadFile(entry->GetMftEntry());
            
            // Valid data?
            if (!entryData.GetSize()) {
                wxMessageBox(wxT("Failed to extract the file, most likely due to a decompression error."), wxT("Error"), wxOK | wxICON_ERROR);
                return;
            }
            // Convert to a usable format
            ANetFileType fileType = ANFT_Unknown;
            mDatFile.IdentifyFileType(entryData.GetPointer(), entryData.GetSize(), fileType);
            FileReader* reader = FileReader::GetReaderForData(entryData, fileType);

            wxString ext = wxEmptyString;
            if (reader) {
                entryData = reader->ConvertData();
                ext       = reader->GetExtension();
            }

            // Ask for location
            wxFileDialog dialog(this, 
                wxString::Format(wxT("Extract %s%s..."), entry->GetName(), ext), 
                wxEmptyString, 
                entry->GetName() + ext, 
                wxFileSelectorDefaultWildcardStr,
                wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

            if (dialog.ShowModal() == wxID_OK) {
                wxFile file(dialog.GetPath(), wxFile::write);
                if (file.IsOpened()) {
                    file.Write(entryData.GetPointer(), entryData.GetSize());
                    file.Close();
                } else {
                    wxMessageBox(wxT("Failed to open the file for writing."), wxT("Error"), wxOK | wxICON_ERROR);
                }
            }

            DeletePointer(reader);
        }

        // More files than one
        else {
            wxDirDialog dialog(this, wxT("Select output folder"));
            if (dialog.ShowModal() == wxID_OK) {
                new ExtractFilesWindow(entries, mDatFile, dialog.GetPath(), ExtractFilesWindow::EM_Converted);
            }
        }
    }
}

}; // namespace gw2b
