/** \file       WriteIndexTask.h
 *  \brief      Contains definition of the WriteIndexTask class.
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
#include "WriteIndexTask.h"

namespace gw2b
{

WriteIndexTask::WriteIndexTask(DatIndex* pIndex, const wxString& pFilename)
    : mIndex(pIndex, true)
    , mWriter(*pIndex)
    , mFilename(pFilename)
    , mErrorOccured(false)
{
    Ensure::NotNull(pIndex);
}

bool WriteIndexTask::Init()
{
    if (mIndex->IsDirty()) {
        bool result = mWriter.Open(mFilename);
        if (result) { this->SetMaxProgress(mWriter.GetNumEntries() + mWriter.GetNumCategories()); }
        return result;
    }
    return false;
}

void WriteIndexTask::Perform()
{
    if (!this->IsDone()) {
        mErrorOccured = !mWriter.Write(7);
        uint progress = mWriter.GetCurrentEntry() + mWriter.GetCurrentCategory();
        this->SetCurrentProgress(progress);
        this->SetText(wxT("Saving .dat index..."));
        // If something went wrong, we should delete the file again since it's half-complete
        if (mErrorOccured && wxFile::Exists(mFilename)) {
            wxRemoveFile(mFilename);
        }
        // If done, remove the dirty flag from the index
        if (this->IsDone()) {
            mIndex->SetDirty(false);
        }
    }
}

void WriteIndexTask::Abort()
{
    mWriter.Close();
    // Remove the file again
    if (wxFile::Exists(mFilename)) {
        wxRemoveFile(mFilename);
    }
}

void WriteIndexTask::Clean()
{
    mWriter.Close();
}

bool WriteIndexTask::IsDone() const
{
    return (mWriter.IsDone() || mErrorOccured);
}

}; // namespace gw2b
