/** \file       Viewers/BinaryViewer.h
 *  \brief      Contains declaration of the hex viewer.
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

#ifndef VIEWERS_BINARYVIEWER_H_INCLUDED
#define VIEWERS_BINARYVIEWER_H_INCLUDED

#include "Viewer.h"

namespace gw2b
{
class HexControl;

class BinaryViewer : public Viewer
{
    HexControl*     mHexControl;
    Array<byte>     mBinaryData;
public:
    BinaryViewer(wxWindow* pParent, const wxPoint& pPos = wxDefaultPosition, const wxSize& pSize = wxDefaultSize);
    virtual ~BinaryViewer();

    virtual void Clear();
    virtual void SetReader(FileReader* pReader);
}; // class BinaryViewer

}; // namespace gw2b

#endif // VIEWERS_BINARYVIEWER_H_INCLUDED
