/** \file       Gw2Browser.h
 *  Declares the application object.
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

#ifndef GW2BROWSER_H_INCLUDED
#define GW2BROWSER_H_INCLUDED

#include <wx/progdlg.h>
#include "DatFile.h"

namespace gw2b
{

/** Represents the Gw2Browser application. */
class Gw2Browser : public wxApp
{
    wxString    mDatPath;
public:
    /** Initializes the application (acts as the application entry-point). */
    virtual bool OnInit();
private:
    /** Parses the command line arguments for valid switches. */
    void ParseArguments();
}; // class Gw2Browser

}; // namespace gw2b

#endif // GW2BROWSER_H_INCLUDED
