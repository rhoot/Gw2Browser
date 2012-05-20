/** \file       ExtractFilesWindow.h
 *  \brief      Contains declaration of the file extraction window.
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

#ifndef EXTRACTFILESWINDOW_H_INCLUDED
#define EXTRACTFILESWINDOW_H_INCLUDED

#include <wx/filename.h>
#include <wx/progdlg.h>

namespace gw2b
{
class DatFile;
class DatIndexCategory;
class DatIndexEntry;

/** Acts as a proxy for a progress dialog, since they cannot receive idle events... */
class ExtractFilesWindow : public wxFrame
{
public:
    enum ExtractionMode
    {
        EM_Raw,
        EM_Converted,
    };
private:
    DatFile&                    mDatFile;
    Array<const DatIndexEntry*> mEntries;
    wxProgressDialog*           mProgress;
    uint                        mCurrentProgress;
    wxString                    mPath;
    ExtractionMode              mMode;
public:
    ExtractFilesWindow(const Array<const DatIndexEntry*>& pEntries, DatFile& pDatFile, const wxString& pPath, ExtractionMode pMode);
private:
    void OnIdleEvt(wxIdleEvent& pEvent);
    void ExtractFile(const DatIndexEntry& pEntry);
    void AppendPaths(wxFileName& pPath, const DatIndexCategory& pCategory);
}; // class ExtractFilesWindow

}; // namespace gw2b

#endif // EXTRACTFILESWINDOW_H_INCLUDED
