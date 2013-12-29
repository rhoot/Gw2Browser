/* \file       ExtractFilesWindow.h
*  \brief      Contains declaration of the file extraction window.
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

#ifndef EXTRACTFILESWINDOW_H_INCLUDED
#define EXTRACTFILESWINDOW_H_INCLUDED

#include <wx/filename.h>
#include <wx/progdlg.h>

namespace gw2b {
	class DatFile;
	class DatIndexCategory;
	class DatIndexEntry;

	/** Acts as a proxy for a progress dialog, since they cannot receive idle events... */
	class ExtractFilesWindow : public wxFrame {
	public:
		enum ExtractionMode {
			EM_Raw,
			EM_Converted,
		};
	private:
		DatFile&                    m_datFile;
		Array<const DatIndexEntry*> m_entries;
		wxProgressDialog*           m_progress;
		uint                        m_currentProgress;
		wxString                    m_path;
		ExtractionMode              m_mode;
	public:
		ExtractFilesWindow( const Array<const DatIndexEntry*>& p_entries, DatFile& p_datFile, const wxString& p_path, ExtractionMode p_mode );
	private:
		void onIdleEvt( wxIdleEvent& p_event );
		void extractFile( const DatIndexEntry& p_entry );
		void appendPaths( wxFileName& p_path, const DatIndexCategory& p_category );
	}; // class ExtractFilesWindow

}; // namespace gw2b

#endif // EXTRACTFILESWINDOW_H_INCLUDED
