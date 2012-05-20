/** \file       Viewers/ImageViewer.h
 *  \brief      Contains declaration of the image viewer.
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

#ifndef VIEWERS_IMAGEVIEWER_H_INCLUDED
#define VIEWERS_IMAGEVIEWER_H_INCLUDED

#include "Viewer.h"

namespace gw2b
{
class ImageControl;
class ImageReader;

class ImageViewer : public Viewer
{
    ImageControl*       mImageControl;
    wxImage*            mImage;
    Array<wxWindowID>   mToolbarButtonIds;
    Array<wxBitmap>     mToolbarButtonIcons;
    Array<wxToolBarToolBase*>   mToolbarButtons;
public:
    ImageViewer(wxWindow* pParent, const wxPoint& pPos = wxDefaultPosition, const wxSize& pSize = wxDefaultSize);
    virtual ~ImageViewer();

    virtual void Clear();
    virtual void SetReader(FileReader* pReader);
    /** Gets the image reader containing the data displayed by this viewer.
     *  \return ImageReader*    Reader containing the data. */
    ImageReader* GetImageReader()               { return (ImageReader*)this->GetReader(); }
    /** Gets the image reader containing the data displayed by this viewer.
     *  \return ImageReader*    Reader containing the data. */
    const ImageReader* GetImageReader() const   { return (const ImageReader*)this->GetReader(); }
private:
    wxToolBar* BuildToolbar();
    void OnToolbarClickedEvt(wxCommandEvent& pEvent);
}; // class ImageViewer

}; // namespace gw2b

#endif // VIEWERS_IMAGEVIEWER_H_INCLUDED
