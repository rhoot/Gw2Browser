/** \file       ReadIndexTask.cpp
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

#include "stdafx.h"
#include "ReadIndexTask.h"

namespace gw2b
{

ReadIndexTask::ReadIndexTask(const std::shared_ptr<DatIndex>& pIndex, const wxString& pFilename, uint64 pDatTimeStamp)
    : mIndex(pIndex)
    , mReader(*pIndex)
    , mFilename(pFilename)
    , mErrorOccured(false)
    , mDatTimeStamp(pDatTimeStamp)
{
    Ensure::notNull(pIndex.get());
}

bool ReadIndexTask::init()
{
    mIndex->clear();
    mIndex->setDirty(false);

    bool result = mReader.open(mFilename);
    if (result) { result = (mIndex->datTimestamp() == mDatTimeStamp); }
    if (result) { this->setMaxProgress(mReader.numEntries() + mReader.numCategories()); }

    return result;
}

void ReadIndexTask::perform()
{
    if (!this->isDone()) {
        mErrorOccured = !(mReader.read(7) & DatIndexReader::RR_Success);
        if (mErrorOccured) { mIndex->clear(); }
        uint progress = mReader.currentEntry() + mReader.currentCategory();
        this->setCurrentProgress(progress);
        this->setText(wxT("Reading .dat index..."));
    }
}

void ReadIndexTask::abort()
{
    this->clean();
}

void ReadIndexTask::clean()
{
    mReader.close();
}

bool ReadIndexTask::isDone() const
{
    return (mErrorOccured || !mReader.isOpen() || mReader.isDone());
}

}; // namespace gw2b
