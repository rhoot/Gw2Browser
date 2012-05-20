/** \file       Viewer.h
 *  \brief      Contains declaration of the viewer base class.
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

#ifndef VIEWER_H_INCLUDED
#define VIEWER_H_INCLUDED

namespace gw2b
{
class FileReader;

/** Panel used to view the contents of a file. */
class Viewer : public wxPanel
{
    FileReader*     mReader;
public:
    /** Constructor. Creates the viewer with the given parent.
     *  \param[in]  pParent Parent of the control.
     *  \param[in]  pPos    Optional location of the control. 
     *  \param[in]  pSize   Optional size of the control. */
    Viewer(wxWindow* pParent, const wxPoint& pPos = wxDefaultPosition, const wxSize& pSize = wxDefaultSize);
    /** Destructor. */
    virtual ~Viewer();

    /** Clears all data in this viewer. */
    virtual void Clear();

    /** Sets the reader containing the data displayed by this viewer.
     *  \param[in]  pReader     Reader to get data from. */
    virtual void SetReader(FileReader* pReader);
    /** Gets the reader containing the data displayed by this viewer.
     *  \return FileReader*     Reader containing the data. */
    FileReader* GetReader()                 { return mReader; }
    /** Gets the reader containing the data displayed by this viewer.
     *  \return FileReader*     Reader containing the data. */
    const FileReader* GetReader() const     { return mReader; }
}; // class Viewer

}; // namespace gw2b

#endif // VIEWER_H_INCLUDED
