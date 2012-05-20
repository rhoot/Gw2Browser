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

ReadIndexTask::ReadIndexTask(DatIndex* pIndex, const wxString& pFilename, uint64 pDatTimeStamp)
    : mIndex(pIndex, true)
    , mReader(*pIndex)
    , mFilename(pFilename)
    , mErrorOccured(false)
    , mDatTimeStamp(pDatTimeStamp)
{
    Ensure::NotNull(pIndex);
}

bool ReadIndexTask::Init()
{
    mIndex->Clear();
    mIndex->SetDirty(false);

    bool result = mReader.Open(mFilename);
    if (result) { result = (mIndex->GetDatTimeStamp() == mDatTimeStamp); }
    if (result) { this->SetMaxProgress(mReader.GetNumEntries() + mReader.GetNumCategories()); }

    return result;
}

void ReadIndexTask::Perform()
{
    if (!this->IsDone()) {
        mErrorOccured = !(mReader.Read(7) & DatIndexReader::RR_Success);
        if (mErrorOccured) { mIndex->Clear(); }
        uint progress = mReader.GetCurrentEntry() + mReader.GetCurrentCategory();
        this->SetCurrentProgress(progress);
        this->SetText(wxT("Reading .dat index..."));
    }
}

void ReadIndexTask::Abort()
{
    this->Clean();
}

void ReadIndexTask::Clean()
{
    mReader.Close();
}

bool ReadIndexTask::IsDone() const
{
    return (mErrorOccured || !mReader.IsOpen() || mReader.IsDone());
}

}; // namespace gw2b
