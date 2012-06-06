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

ProgressStatusBar::ProgressStatusBar(wxWindow* p_parent)
    : wxStatusBar(p_parent, wxID_ANY, wxST_SIZEGRIP)
{
    m_progress.Create(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL | wxGA_SMOOTH);
    m_progress.Hide();
    this->Connect(wxEVT_SIZE, wxSizeEventHandler(ProgressStatusBar::onSize));
}

void ProgressStatusBar::showProgressBar()
{
    wxRect progressRect;
    this->SetFieldsCount(2);
    this->GetFieldRect(1, progressRect);

    m_progress.SetPosition(progressRect.GetPosition());
    m_progress.SetSize(progressRect.GetSize());
    m_progress.Show();
}

void ProgressStatusBar::hideProgressBar()
{
    m_progress.Hide();
    this->SetFieldsCount(1);
}

uint ProgressStatusBar::maxValue() const
{
    return m_progress.GetRange();
}

void ProgressStatusBar::setMaxValue(uint p_value)
{
    m_progress.SetRange(p_value);
}

uint ProgressStatusBar::currentValue() const
{
    return m_progress.GetValue();
}

void ProgressStatusBar::setCurrentValue(uint p_value)
{
    m_progress.SetValue(wxMin(p_value, m_progress.GetRange()));
}

void ProgressStatusBar::update(uint p_value, const wxString& p_label)
{
    this->setCurrentValue(p_value);
    this->SetStatusText(p_label);
}


void ProgressStatusBar::onSize(wxSizeEvent& WXUNUSED(p_event))
{
    if (m_progress.IsShown()) {
        wxRect progressRect;
        this->GetFieldRect(1, progressRect);
        m_progress.SetPosition(progressRect.GetPosition());
        m_progress.SetSize(progressRect.GetSize());
    }
}

}; // namespace gw2b
