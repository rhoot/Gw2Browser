/** \file       ReadIndexTask.h
 *  \brief      Contains declaration of the ReadIndexTask class.
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

#ifndef TASKS_READINDEXTASK_H_INCLUDED
#define TASKS_READINDEXTASK_H_INCLUDED

#include "DatIndexIO.h"
#include "Task.h"

namespace gw2b
{
class DatIndex;

class ReadIndexTask : public Task
{
    AutoPtr<DatIndex>   mIndex;
    DatIndexReader      mReader;
    wxString            mFilename;
    bool                mErrorOccured;
    uint64              mDatTimeStamp;
public:
    ReadIndexTask(DatIndex* pIndex, const wxString& pFilename, uint64 pDatTimeStamp);

    virtual bool Init();
    virtual void Perform();
    virtual void Abort();
    virtual void Clean();

    virtual bool IsDone() const;
}; // class ReadIndexTask

}; // namespace gw2b

#endif // TASKS_READINDEXTASK_H_INCLUDED