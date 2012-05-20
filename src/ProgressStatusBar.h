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
    wxGauge     mProgress;
public:
    /** Constructor. Creates the status bar with the given parent.
     *  \param[in]  pParent     Parent of the control. */
    ProgressStatusBar(wxWindow* pParent);

    /** Shows the progress bar. */
    void ShowProgressBar();
    /** Hides the progress bar. */
    void HideProgressBar();

    /** Gets the current max value for the progress bar.
     *  \return uint    current max value. */
    uint GetMaxValue() const;
    /** Sets the current max value for the progress bar.
     *  \param[in]  pValue  current max value. */
    void SetMaxValue(uint pValue);
    /** Gets the current value for the progress bar.
     *  \return uint    current value. */
    uint GetCurrentValue() const;
    /** Sets the current value for the progress bar.
     *  \param[in]  pValue  current value. */
    void SetCurrentValue(uint pValue);

    /** Updates the progress bar with the given value and sets the status text
     *  to the given label.
     *  \param[in]  pValue  Value to update to.
     *  \param[in]  pLabel  Text to use as status text. */
    void Update(uint pValue, const wxString& pLabel);
private:
    /** Resizes and repositions the progress bar to stay within the second pane.
     *  \param[in]  pEvent  Event object handed to us by wxWidgets. */
    void OnSize(wxSizeEvent& pEvent);
};

}; // namespace gw2b

#endif // PROGRESSSTATUSBAR_H_INCLUDED
