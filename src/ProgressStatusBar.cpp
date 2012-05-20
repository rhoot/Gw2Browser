/** \file       ProgressStatusBar.h
 *  \brief      Contains definition of a status bar with a built-in progress bar.
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
#include "ProgressStatusBar.h"

namespace gw2b
{

ProgressStatusBar::ProgressStatusBar(wxWindow* pParent)
    : wxStatusBar(pParent, wxID_ANY, wxST_SIZEGRIP)
{
    mProgress.Create(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL | wxGA_SMOOTH);
    mProgress.Hide();
    this->Connect(wxEVT_SIZE, wxSizeEventHandler(ProgressStatusBar::OnSize));
}

void ProgressStatusBar::ShowProgressBar()
{
    wxRect progressRect;
    this->SetFieldsCount(2);
    this->GetFieldRect(1, progressRect);

    mProgress.SetPosition(progressRect.GetPosition());
    mProgress.SetSize(progressRect.GetSize());
    mProgress.Show();
}

void ProgressStatusBar::HideProgressBar()
{
    mProgress.Hide();
    this->SetFieldsCount(1);
}

uint ProgressStatusBar::GetMaxValue() const
{
    return mProgress.GetRange();
}

void ProgressStatusBar::SetMaxValue(uint pValue)
{
    mProgress.SetRange(pValue);
}

uint ProgressStatusBar::GetCurrentValue() const
{
    return mProgress.GetValue();
}

void ProgressStatusBar::SetCurrentValue(uint pValue)
{
    mProgress.SetValue(wxMin(pValue, mProgress.GetRange()));
}

void ProgressStatusBar::Update(uint pValue, const wxString& pLabel)
{
    this->SetCurrentValue(pValue);
    this->SetStatusText(pLabel);
}


void ProgressStatusBar::OnSize(wxSizeEvent& WXUNUSED(pEvent))
{
    if (mProgress.IsShown()) {
        wxRect progressRect;
        this->GetFieldRect(1, progressRect);
        mProgress.SetPosition(progressRect.GetPosition());
        mProgress.SetSize(progressRect.GetSize());
    }
}

}; // namespace gw2b
