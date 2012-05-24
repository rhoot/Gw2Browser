/** \file       Viewers/ImageViewer.cpp
 *  \brief      Contains definition of the image viewer.
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
#include "ImageViewer.h"

#include "Viewers/ImageViewer/ImageControl.h"
#include "Readers/ImageReader.h"
#include "Data.h"

namespace gw2b
{

ImageViewer::ImageViewer(wxWindow* pParent, const wxPoint& pPos, const wxSize& pSize)
    : Viewer(pParent, pPos, pSize)
    , mImageControl(NULL)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Toolbar
    wxToolBar* toolbar = this->BuildToolbar();
    sizer->Add(toolbar, wxSizerFlags().Expand().FixedMinSize());

    // Hex control
    mImageControl = new ImageControl(this);
    sizer->Add(mImageControl, wxSizerFlags().Expand().Proportion(1));

    // Layout
    this->SetSizer(sizer);
    this->Layout();
}

ImageViewer::~ImageViewer() 
{
}

void ImageViewer::Clear()
{
    mImageControl->SetImage(wxImage());
    Viewer::Clear();
}

void ImageViewer::SetReader(FileReader* pReader)
{
    Ensure::IsOfType<ImageReader>(pReader);
    Viewer::SetReader(pReader);

    if (pReader) {
        mImage = GetImageReader()->GetImage();
        mImageControl->SetImage(mImage);
    }
}

wxToolBar* ImageViewer::BuildToolbar()
{
    wxToolBar* toolbar = new wxToolBar(this, wxID_ANY);
    wxWindowID id      = this->NewControlId(4);

    // Add the newly generated IDs
    for (uint i = 0; i < 4; i++) {
        mToolbarButtonIds.Add(id++);
    }

    // Load all toolbar button icons
    toolbar->SetToolBitmapSize(wxSize(16, 16));
    mToolbarButtonIcons.push_back(data::LoadPNG(data::toggle_red_png,   data::toggle_red_png_size));
    mToolbarButtonIcons.push_back(data::LoadPNG(data::toggle_green_png, data::toggle_green_png_size));
    mToolbarButtonIcons.push_back(data::LoadPNG(data::toggle_blue_png,  data::toggle_blue_png_size));
    mToolbarButtonIcons.push_back(data::LoadPNG(data::toggle_alpha_png, data::toggle_alpha_png_size));

    // Toggle channel buttons
    for (uint i = 0; i < 4; i++) {
        wxToolBarToolBase* button = new wxToolBarToolBase(toolbar, mToolbarButtonIds[i], wxEmptyString, mToolbarButtonIcons[i], mToolbarButtonIcons[i], wxITEM_CHECK);
        toolbar->AddTool(button);
        button->Toggle(true);
        mToolbarButtons.Add(button);
        this->Connect(mToolbarButtonIds[i], wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ImageViewer::OnToolbarClickedEvt));
    }

    toolbar->Realize();
    return toolbar;
}

void ImageViewer::OnToolbarClickedEvt(wxCommandEvent& pEvent)
{
    wxWindowID id = pEvent.GetId();

    // Toggle red
    if (id == mToolbarButtonIds[0]) {
        mImageControl->ToggleChannel(ImageControl::IC_Red, mToolbarButtons[0]->IsToggled());
    // Toggle green
    } else if (id == mToolbarButtonIds[1]) {
        mImageControl->ToggleChannel(ImageControl::IC_Green, mToolbarButtons[1]->IsToggled());
    // Toggle blue
    } else if (id == mToolbarButtonIds[2]) {
        mImageControl->ToggleChannel(ImageControl::IC_Blue, mToolbarButtons[2]->IsToggled());        
    // Toggle alpha
    } else if (id == mToolbarButtonIds[3]) {
        mImageControl->ToggleChannel(ImageControl::IC_Alpha, mToolbarButtons[3]->IsToggled());        
    } else {
        pEvent.Skip();
    }
}

}; // namespace gw2b
