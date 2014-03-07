/* \file       BrowserWindow.h
*  \brief      Contains declaration of the browser window.
*  \author     Rhoot
*/

/*
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

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

namespace gw2b {
	class DatIndex;
	class PreviewPanel;
	class ProgressStatusBar;
	class Task;

	/** Represents the browser's main window. */
	class BrowserWindow : public wxFrame, public ICategoryTreeListener {
		wxString                    m_datPath;
		DatFile                     m_datFile;
		std::shared_ptr<DatIndex>   m_index;
		ProgressStatusBar*          m_progress;
		Task*                       m_currentTask;
		wxSplitterWindow*           m_splitter;
		CategoryTree*               m_catTree;
		PreviewPanel*               m_previewPanel;
	public:
		/** Constructs the frame with the given title.
		*  \param[in]  p_title  Title of window. */
		BrowserWindow( const wxString& p_title );
		/** Destructor. */
		~BrowserWindow( );
		/** Opens the given .dat file for browsing.
		*  \param[in]  p_path   Path to the .dat file to open. */
		void openFile( const wxString& p_path );
		/** Tries to close this window, but does not force it (same as calling
		*  Close(false)). */
		void tryClose( );
		/** Opens the preview pane with the given entry's contents in it.
		*  \param[in]  p_entry  entry to view. */
		void viewEntry( const DatIndexEntry& p_entry );
	private:
		/** Performs the given task periodically, until it is done.
		*  \param[in]  p_task   Task to perform. Ownership is taken.
		*  \return bool    true if the task's init succeeded, false if not. */
		bool performTask( Task* p_task );

		/** Hashes the internally stored .dat file path and determines where its
		*   index file should be located.
		*   \return wxFileName containing the path to the index file. */
		wxFileName findDatIndex( );
		/** Resumes indexing the loaded .dat file. */
		void indexDat( );
		/** Re-indexes the loaded .dat file. */
		void reIndexDat( );

		/** Executed when the user clicks <em>File -> Open</em> in the menu.
		*  \param[in]  p_event  Unused event object handed to us by wxWidgets. */
		void onOpenEvt( wxCommandEvent& p_event );
		/** Executed when the user clicks <em>File -> Exit</em> in the menu.
		*  \param[in]  p_event  Unused event object handed to us by wxWidgets. */
		void onExitEvt( wxCommandEvent& p_event );
		/** Executed when the user clicks <em>Help -> About</em> in the menu.
		*  \param[in]  p_event  Unused event object handed to us by wxWidgets. */
		void onAboutEvt( wxCommandEvent& p_event );
		/** Executed when the window is closing.
		*  \param[in]  p_event  Unused event object handed to us by wxWidgets. */
		void onCloseEvt( wxCloseEvent& p_event );
		/** Performs the currently active task repeatedly until it is complete.
		*  \param[in]  p_event  Idle event object used to request more idle events. */
		void onPerformTaskEvt( wxIdleEvent& p_event );

		/** Raised when the index has been read. */
		void onReadIndexComplete( );
		/** Raised when the .dat has finished indexing. */
		void onScanTaskComplete( );
		/** Raised when the write task has finished, if invoked from onCloseEvt. */
		void onWriteTaskCloseCompleted( );

		/** Raised when the user clicks an item in the category tree.
		*  \param[in]  p_tree   tree that raised the event.
		*  \param[in]  p_entry  entry that was clicked. */
		virtual void onTreeEntryClicked( CategoryTree& p_tree, const DatIndexEntry& p_entry ) override;
		/** Raised when the user clicks a category in the category tree.
		*  \param[in]  p_tree       tree that raised the event.
		*  \param[in]  p_category   category that was clicked. */
		virtual void onTreeCategoryClicked( CategoryTree& p_tree, const DatIndexCategory& p_category ) override;
		/** Raised when the category tree was cleared.
		*  \param[in]  p_tree   tree that was cleared. */
		virtual void onTreeCleared( CategoryTree& p_tree ) override;
		virtual void onTreeExtractRaw( CategoryTree& p_tree ) override;
		virtual void onTreeExtractConverted( CategoryTree& p_tree ) override;
	}; // class BrowserWindow

}; // namespace gw2b

#endif // BROWSERWINDOW_H_INCLUDED
