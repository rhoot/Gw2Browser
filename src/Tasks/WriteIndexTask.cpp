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

WriteIndexTask::WriteIndexTask(const std::shared_ptr<DatIndex>& pIndex, const wxFileName& pFilename)
    : mIndex(pIndex)
    , mWriter(*pIndex)
    , mFilename(pFilename)
    , mErrorOccured(false)
{
    Ensure::notNull(pIndex.get());
}

bool WriteIndexTask::init()
{
    if (!mFilename.DirExists()) {
        mFilename.Mkdir(511, wxPATH_MKDIR_FULL);
    }

    if (mIndex->isDirty()) {
        bool result = mWriter.open(mFilename.GetFullPath());
        if (result) { this->setMaxProgress(mWriter.numEntries() + mWriter.numCategories()); }
        return result;
    }
    return false;
}

void WriteIndexTask::perform()
{
    if (!this->isDone()) {
        mErrorOccured = !mWriter.write(7);
        uint progress = mWriter.currentEntry() + mWriter.currentCategory();
        this->setCurrentProgress(progress);
        this->setText(wxT("Saving .dat index..."));
        // If something went wrong, we should delete the file again since it's half-complete
        wxString path = mFilename.GetFullPath();
        if (mErrorOccured && wxFile::Exists(path)) {
            wxRemoveFile(path);
        }
        // If done, remove the dirty flag from the index
        if (this->isDone()) {
            mIndex->setDirty(false);
        }
    }
}

void WriteIndexTask::abort()
{
    mWriter.close();
    // Remove the file again
    wxString path = mFilename.GetFullPath();
    if (wxFile::Exists(path)) {
        wxRemoveFile(path);
    }
}

void WriteIndexTask::clean()
{
    mWriter.close();
}

bool WriteIndexTask::isDone() const
{
    return (mWriter.isDone() || mErrorOccured);
}

}; // namespace gw2b
