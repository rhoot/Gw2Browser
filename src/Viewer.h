/* \file       Viewer.h
*  \brief      Contains declaration of the viewer base class.
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

#ifndef VIEWER_H_INCLUDED
#define VIEWER_H_INCLUDED

namespace gw2b {
	class DatFile;
	class FileReader;


	class INeedDatFile {
		DatFile* m_datFile;
	protected:
		INeedDatFile( ) {
		}
	public:
		virtual ~INeedDatFile( ) {
		}
		// I know interfaces per definition aren't supposed to have implementation, but screw that. /rebelyell
		DatFile* datFile( ) {
			return m_datFile;
		}
		const DatFile* datFile( ) const {
			return m_datFile;
		}
		void setDatFile( DatFile* p_datFile ) {
			m_datFile = p_datFile;
		}
	};

	/** Panel used to view the contents of a file. */
	class Viewer : public wxPanel {
		FileReader*     m_reader;
	public:
		/** Constructor. Creates the viewer with the given parent.
		*  \param[in]  p_parent Parent of the control.
		*  \param[in]  p_pos    Optional location of the control.
		*  \param[in]  p_size   Optional size of the control. */
		Viewer( wxWindow* p_parent, const wxPoint& p_pos = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
		/** Destructor. */
		virtual ~Viewer( );

		/** Clears all data in this viewer. */
		virtual void clear( );

		/** Sets the reader containing the data displayed by this viewer.
		*  \param[in]  p_reader     Reader to get data from. */
		virtual void setReader( FileReader* p_reader );
		/** Gets the reader containing the data displayed by this viewer.
		*  \return FileReader*     Reader containing the data. */
		FileReader* reader( ) {
			return m_reader;
		}
		/** Gets the reader containing the data displayed by this viewer.
		*  \return FileReader*     Reader containing the data. */
		const FileReader* reader( ) const {
			return m_reader;
		}
	}; // class Viewer

}; // namespace gw2b

#endif // VIEWER_H_INCLUDED
