/** \file       Gw2Browser.cpp
 *  \brief      Defines the application object. This is the entry point.
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
#include "Gw2Browser.h"

#include <vld.h>

#include "BrowserWindow.h"

namespace gw2b
{
    
wxIMPLEMENT_APP(Gw2Browser);

//============================================================================/

namespace
{
    
    struct ArgumentOptions
    {
        wxString datPath;
    };

    ArgumentOptions parseArguments(int argc, wchar_t** argv)
    {
        ArgumentOptions options;
        if (argc > 1) {
            options.datPath = argv[1];
        }
        return options;
    }

}; // anon namespace

//============================================================================/

bool Gw2Browser::OnInit()
{
    ::wxInitAllImageHandlers();

    auto window = new BrowserWindow(wxT("Gw2Browser"));
    window->Show();

    auto options = parseArguments(this->argc, this->argv);
    if (!options.datPath.IsEmpty()) {
        window->openFile(options.datPath);
    }

    return true;
}

//============================================================================/

}; // namespace gw2b
