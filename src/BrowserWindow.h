/** \file       BrowserWindow.h
 *  \brief      Contains declaration of the browser window.
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

#ifndef BROWSERWINDOW_H_INCLUDED
#define BROWSERWINDOW_H_INCLUDED

#include <wx/filename.h>
#include <wx/splitter.h>

#include "CategoryTree.h"
#include "DatFile.h"

namespace gw2b
{
class DatIndex;
class PreviewPanel;
class ProgressStatusBar;
class Task;

/** Represents the browser's main window. */
class BrowserWindow : public wxFrame, public ICategoryTreeListener
{
    wxString            mDatPath;
    DatFile             mDatFile;
    AutoPtr<DatIndex>   mIndex;
    ProgressStatusBar*  mProgress;
    Task*               mCurrentTask;
    wxSplitterWindow*   mSplitter;
    CategoryTree*       mCatTree;
    PreviewPanel*       mPreviewPanel;
public:
    /** Constructs the frame with the given title.
     *  \param[in]  pTitle  Title of window. */
    BrowserWindow(const wxString& pTitle);
    /** Destructor. */
    ~BrowserWindow();
    /** Opens the given .dat file for browsing.
     *  \param[in]  pPath   Path to the .dat file to open. */
    void OpenFile(const wxString& pPath);
    /** Tries to close this window, but does not force it (same as calling 
     *  Close(false)). */
    void TryClose();
    /** Opens the preview pane with the given entry's contents in it.
     *  \param[in]  pEntry  entry to view. */
    void ViewEntry(const DatIndexEntry& pEntry);
private:
    /** Performs the given task periodically, until it is done.
     *  \param[in]  pTask   Task to perform. Ownership is taken. 
     *  \return bool    true if the task's init succeeded, false if not. */
    bool PerformTask(Task* pTask);

    /** Hashes the internally stored .dat file path and determines where its 
    *   index file should be located. 
    *   \return wxFileName containing the path to the index file. */
    wxFileName FindDatIndex();
    /** Resumes indexing the loaded .dat file. 
     *  \param[in]  pFirstEntry     First entry to scan. */
    void IndexDat();
    /** Reindexes the loaded .dat file. */
    void ReIndexDat();

    /** Executed when the user clicks <em>File -> Open</em> in the menu. 
     *  \param[in]  pEvent  Unused event object handed to us by wxWidgets. */
    void OnOpenEvt(wxCommandEvent& pEvent);
    /** Executed when the user clicks <em>File -> Exit</em> in the menu. 
     *  \param[in]  pEvent  Unused event object handed to us by wxWidgets. */
    void OnExitEvt(wxCommandEvent& pEvent);
    /** Executed when the user clicks <em>Help -> About</em> in the menu. 
     *  \param[in]  pEvent  Unused event object handed to us by wxWidgets. */
    void OnAboutEvt(wxCommandEvent& pEvent);
    /** Executed when the window is closing.
     *  \param[in]  pEvent  Unused event object handed to us by wxWidgets. */
    void OnCloseEvt(wxCloseEvent& pEvent);
    /** Performs the currently active task repeatedly until it is complete.
     *  \param[in]  pEvent  Idle event object used to request more idle events. */
    void OnPerformTaskEvt(wxIdleEvent& pEvent);

    /** Raised when the index has been read. */
    void OnReadIndexComplete();
    /** Raised when the .dat has finished indexing. */
    void OnScanTaskComplete();
    /** Raised when the write task has finished, if invoked from OnCloseEvt. */
    void OnWriteTaskCloseCompleted();

    /** Raised when the user clicks an item in the category tree.
     *  \param[in]  pTree   tree that raised the event.
     *  \param[in]  pEntry  entry that was clicked. */
    virtual void OnTreeEntryClicked(CategoryTree& pTree, const DatIndexEntry& pEntry);
    /** Raised when the user clicks a category in the category tree.
     *  \param[in]  pTree       tree that raised the event.
     *  \param[in]  pCategory   category that was clicked. */
    virtual void OnTreeCategoryClicked(CategoryTree& pTree, const DatIndexCategory& pCategory);
    /** Raised when the category tree was cleared.
     *  \param[in]  pTree   tree that was cleared. */
    virtual void OnTreeCleared(CategoryTree& pTree);
    virtual void OnTreeExtractRaw(CategoryTree& pTree);
    virtual void OnTreeExtractConverted(CategoryTree& pTree);
}; // class BrowserWindow

}; // namespace gw2b

#endif // BROWSERWINDOW_H_INCLUDED
