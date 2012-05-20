/** \file       DatIndexIO.cpp
 *  \brief      Contains the definition for the index reader and writer classes.
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
#include "DatIndexIO.h"

namespace gw2b
{

//----------------------------------------------------------------------------
//      DatIndexReader
//----------------------------------------------------------------------------

DatIndexReader::DatIndexReader(DatIndex& pIndex)
    : mIndex(pIndex)
{
    Ensure::NotNull(&pIndex);
    ::memset(&mHeader, 0, sizeof(mHeader));
}

DatIndexReader::~DatIndexReader()
{
    this->Close();
}

bool DatIndexReader::Open(const wxString& pFilename)
{
    this->Close();
    if (!wxFile::Exists(pFilename)) { return false; }

    mFile.Open(pFilename);
    if (mFile.IsOpened() && mFile.Length() > sizeof(mHeader)) {
        mFile.Read(&mHeader, sizeof(mHeader));
        if (mHeader.mMagicInteger != DatIndex_Magic) { this->Close(); return false; }
        if (mHeader.mVersion != DatIndex_Version) { this->Close(); return false; }
        mIndex.Clear(); // always start with a fresh index
        mIndex.SetDatTimeStamp(mHeader.mDatTimeStamp);
        mIndex.ReserveEntries(mHeader.mNumEntries);
        mIndex.ReserveCategories(mHeader.mNumEntries);
        return true;
    }

    return false;
}

void DatIndexReader::Close()
{
    mFile.Close();
    ::memset(&mHeader, 0, sizeof(mHeader));
}

bool DatIndexReader::IsDone() const
{
    return (mIndex.GetNumCategories() == mHeader.mNumCategories) 
        && (mIndex.GetNumEntries() == mHeader.mNumEntries);
}

DatIndexReader::ReadResult DatIndexReader::Read(uint pAmount)
{
    ReadResult result = RR_Failure;

    for (uint i = 0; i < pAmount; i++) {
        ssize_t bytesRead;

        // First read all categories, one at a time
        if (mIndex.GetNumCategories() < mHeader.mNumCategories) {
            // Read fixed-width fields
            DatIndexCategoryFields fields;
            bytesRead = mFile.Read(&fields, sizeof(fields));
            if (bytesRead < sizeof(fields)) { result = RR_CorruptFile; goto READ_FAILED; }
            // Read name
            Array<char> nameData(fields.mNameLength);
            bytesRead = mFile.Read(nameData.GetPointer(), nameData.GetSize());
            if (bytesRead < (ssize_t)nameData.GetSize()) { result = RR_CorruptFile; goto READ_FAILED; }
            // Add category
            wxString name = wxString::FromUTF8Unchecked(nameData.GetPointer(), nameData.GetSize());
            DatIndexCategory* category = mIndex.AddIndexCategory(name, false);
            // Set parent
            if (fields.mParent != DatIndex_RootCategory) {
                DatIndexCategory* parent = mIndex.GetCategory(fields.mParent);
                if (parent) { parent->AddSubCategory(category); }
            }
        }

        // If all categories are read, start reading the files instead (note the 'else')
        else if (mIndex.GetNumEntries() < mHeader.mNumEntries) {
            // Read fixed-width fields
            DatIndexEntryFields fields;
            bytesRead = mFile.Read(&fields, sizeof(fields));
            if (bytesRead < sizeof(fields)) { result = RR_CorruptFile; goto READ_FAILED; }
            // Read name
            Array<char> nameData(fields.mNameLength);
            bytesRead = mFile.Read(nameData.GetPointer(), nameData.GetSize());
            if (bytesRead < (ssize_t)nameData.GetSize()) { result = RR_CorruptFile; goto READ_FAILED; }
            // Add entry
            wxString name = wxString::FromUTF8Unchecked(nameData.GetPointer(), nameData.GetSize());
            DatIndexEntry& newEntry = mIndex.AddIndexEntry(false)
                ->SetBaseId(fields.mBaseId)
                .SetFileId(fields.mFileId)
                .SetMftEntry(fields.mMftEntry)
                .SetFileType((ANetFileType)fields.mFileType)
                .SetName(name);
            DatIndexCategory* category = mIndex.GetCategory(fields.mCategory);
            if (!category) { result = RR_CorruptFile; goto READ_FAILED; }
            category->AddEntry(&newEntry);
            newEntry.FinalizeAdd();
        }

        // If both are done we can skip this loop
        else {
            break;
        }
    }

    return RR_Success;

READ_FAILED:
    return result;
}

//----------------------------------------------------------------------------
//      DatIndexWriter
//----------------------------------------------------------------------------

DatIndexWriter::DatIndexWriter(DatIndex& pIndex)
    : mIndex(pIndex)
    , mCategoriesWritten(0)
    , mEntriesWritten(0)
{
    Ensure::NotNull(&pIndex);
}

DatIndexWriter::~DatIndexWriter()
{
    this->Close();
}

bool DatIndexWriter::Open(const wxString& pFilename)
{
    this->Close();

    mFile.Open(pFilename, wxFile::write);
    if (mFile.IsOpened()) {
        DatIndexHead header;
        header.mMagicInteger  = DatIndex_Magic;
        header.mVersion       = DatIndex_Version;
        header.mDatTimeStamp  = mIndex.GetDatTimeStamp();
        header.mNumEntries    = mIndex.GetNumEntries();
        header.mNumCategories = mIndex.GetNumCategories();

        ssize_t bytesWritten = mFile.Write(&header, sizeof(header));
        if (bytesWritten < sizeof(header)) { this->Close(); return false; }

        return true;
    }

    return false;
}

void DatIndexWriter::Close()
{
    mFile.Close();
    mCategoriesWritten = 0;
    mEntriesWritten    = 0;
}

bool DatIndexWriter::IsDone() const
{
    return (mIndex.GetNumEntries() == mEntriesWritten)
        && (mIndex.GetNumCategories() == mCategoriesWritten);
}

bool DatIndexWriter::Write(uint pAmount)
{
    for (uint i = 0; i < pAmount; i++) {
        ssize_t bytesWritten;

        // First write categories, one at a time
        if (mCategoriesWritten < mIndex.GetNumCategories()) {
            DatIndexCategory* category    = mIndex.GetCategory(mCategoriesWritten);
            DatIndexCategory* parent      = category->GetParent();
            wxScopedCharBuffer nameBuffer = category->GetName().ToUTF8();
            // Fixed-width fields
            DatIndexCategoryFields fields;
            fields.mParent     = (parent ? parent->GetIndex() : -1);
            fields.mNameLength = nameBuffer.length();
            bytesWritten = mFile.Write(&fields, sizeof(fields));
            if (bytesWritten < sizeof(fields)) { return false; }
            // Name
            bytesWritten = mFile.Write(nameBuffer, fields.mNameLength);
            if (bytesWritten < fields.mNameLength) { return false; }
            // Increase the counter
            mCategoriesWritten++;
        }

        // Then, write entries one at a time (note the 'else')
        else if (mEntriesWritten < mIndex.GetNumEntries()) {
            const DatIndexEntry*    entry    = mIndex.GetEntry(mEntriesWritten);
            const DatIndexCategory* category = entry->GetCategory();
            wxScopedCharBuffer nameBuffer    = entry->GetName().ToUTF8();
            // Fixed-width fields
            DatIndexEntryFields fields;
            fields.mCategory   = category->GetIndex();
            fields.mBaseId     = entry->GetBaseId();
            fields.mFileId     = entry->GetFileId();
            fields.mMftEntry   = entry->GetMftEntry();
            fields.mFileType   = entry->GetFileType();
            fields.mNameLength = nameBuffer.length();
            bytesWritten = mFile.Write(&fields, sizeof(fields));
            if (bytesWritten < sizeof(fields)) { return false; }
            // Name
            bytesWritten = mFile.Write(nameBuffer, fields.mNameLength);
            if (bytesWritten < fields.mNameLength) { return false; }
            // Increase the counter
            mEntriesWritten++;
        }

        // Both done = ditch this loop
        else {
            break;
        }
    }

    // Remove dirty flag if everything is saved
    if (this->IsDone()) {
        mIndex.SetDirty(false);
    }

    return true;
}

}; // namespace gw2b
