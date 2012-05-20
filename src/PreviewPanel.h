/** \file       PreviewPanel.h
 *  \brief      Contains declaration of the preview panel control.
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

#ifndef PREVIEWPANEL_H_INCLUDED
#define PREVIEWPANEL_H_INCLUDED

#include "FileReader.h"
#include "Viewer.h"

namespace gw2b
{
class DatFile;
class DatIndexEntry;

/** Panel control used to preview files from the .dat. */
class PreviewPanel : public wxPanel
{
    Viewer*                 mCurrentView;
    FileReader::DataType    mCurrentDataType;
public:
    /** Constructor. Creates the preview panel with the given parent.
     *  \param[in]  pParent     Parent of the control.
     *  \param[in]  pLocation   Optional location of the control. 
     *  \param[in]  pSize       Optional size of the control. */
    PreviewPanel(wxWindow* pParent, const wxPoint& pLocation = wxDefaultPosition, const wxSize& pSize = wxDefaultSize);
    /** Destructor. */
    ~PreviewPanel();
    /** Tells this panel to preview a file.
     *  \param[in]  pDatFile    .dat file containing the file to preview.
     *  \param[in]  pEntry      Entry to preview.
     *  \return bool    true if successful, false if not. */
    bool PreviewFile(DatFile& pDatFile, const DatIndexEntry& pEntry);
private:
    /** Helper method to create a viewer control to handle the given data type.
     *  The caller is responsible for freeing the viewer.
     *  \param[in]  pDataType   Type of data to create a viewer for.
     *  \return Viewer* Newly created viewer. */
    Viewer* CreateViewerForDataType(FileReader::DataType pDataType);
}; // class PreviewPanel

}; // namespace gw2b

#endif // PREVIEWPANEL_H_INCLUDED
