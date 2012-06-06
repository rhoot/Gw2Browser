/** \file       ProgressStatusBar.h
 *  \brief      Contains declaration of a status bar with a built-in progress bar.
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

#ifndef PROGRESSSTATUSBAR_H_INCLUDED
#define PROGRESSSTATUSBAR_H_INCLUDED

namespace gw2b
{

/** Status bar with a progress bar built in. */
class ProgressStatusBar : public wxStatusBar
{
    wxGauge     m_progress;
public:
    /** Constructor. Creates the status bar with the given parent.
     *  \param[in]  p_parent     Parent of the control. */
    ProgressStatusBar(wxWindow* p_parent);

    /** Shows the progress bar. */
    void showProgressBar();
    /** Hides the progress bar. */
    void hideProgressBar();

    /** Gets the current max value for the progress bar.
     *  \return uint    current max value. */
    uint maxValue() const;
    /** Sets the current max value for the progress bar.
     *  \param[in]  p_value  current max value. */
    void setMaxValue(uint p_value);
    /** Gets the current value for the progress bar.
     *  \return uint    current value. */
    uint currentValue() const;
    /** Sets the current value for the progress bar.
     *  \param[in]  p_value  current value. */
    void setCurrentValue(uint p_value);

    /** Updates the progress bar with the given value and sets the status text
     *  to the given label.
     *  \param[in]  p_value  Value to update to.
     *  \param[in]  p_label  Text to use as status text. */
    void update(uint p_value, const wxString& p_label);
private:
    /** Resizes and repositions the progress bar to stay within the second pane.
     *  \param[in]  p_event  Event object handed to us by wxWidgets. */
    void onSize(wxSizeEvent& p_event);
};

}; // namespace gw2b

#endif // PROGRESSSTATUSBAR_H_INCLUDED
