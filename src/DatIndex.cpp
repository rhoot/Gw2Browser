/** \file       DatIndex.cpp
 *  \brief      Contains the definition for the index model class.
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
#include <wx/file.h>
#include <new>

#include "DatIndex.h"

namespace gw2b
{

//----------------------------------------------------------------------------
//      DatIndexEntry
//----------------------------------------------------------------------------

DatIndexEntry::DatIndexEntry(DatIndex& pOwner)
    : mOwner(&pOwner)
    , mFileId(0)
    , mBaseId(0)
    , mMftEntry(0)
    , mFileType(ANFT_Unknown)
    , mCategory(NULL)
{
    Ensure::NotNull(&pOwner);
}

void DatIndexEntry::OnAddedToCategory(DatIndexCategory* pCategory)
{
    mCategory = pCategory;
}

void DatIndexEntry::FinalizeAdd()
{
    mOwner->OnEntryAddComplete(*this);
}

//----------------------------------------------------------------------------
//      DatIndexCategory
//----------------------------------------------------------------------------

DatIndexCategory::DatIndexCategory(DatIndex& pOwner, const wxString& pName, int pIndex)
    : mOwner(&pOwner)
    , mIndex(pIndex)
    , mName(pName)
    , mParent(NULL)
{
    Ensure::NotNull(&pOwner);
}

DatIndexCategory* DatIndexCategory::FindSubCategory(const wxString& pName)
{
    for (uint i = 0; i < mSubCategories.GetSize(); i++) {
        if (mSubCategories[i]->GetName() == pName) {
            return mSubCategories[i];
        }
    }
    return NULL;
}

DatIndexCategory* DatIndexCategory::FindOrAddSubCategory(const wxString& pName)
{
    DatIndexCategory* subCat = this->FindSubCategory(pName);
    if (!subCat) {
        subCat = mOwner->AddIndexCategory(pName);
        this->AddSubCategory(subCat);
    }
    return subCat;
}

uint DatIndexCategory::GetNumEntries(bool pRecursive) const
{
    uint count = mEntries.GetSize();

    if (pRecursive) {
        for (uint i = 0; i < mSubCategories.GetSize(); i++) {
            count += mSubCategories[i]->GetNumEntries(pRecursive);
        }
    } 

    return count;
}

void DatIndexCategory::AddEntry(DatIndexEntry* pEntry)
{
    mEntries.Add(pEntry);
    pEntry->OnAddedToCategory(this);
}

void DatIndexCategory::AddSubCategory(DatIndexCategory* pSubCategory)
{
    Ensure::NotNull(pSubCategory);
    assert(!pSubCategory->GetParent());

    mSubCategories.Add(pSubCategory);
    pSubCategory->OnAddedToCategory(this);
}

void DatIndexCategory::OnAddedToCategory(DatIndexCategory* pParent)
{
    Ensure::IsNull(mParent);
    mParent = pParent;
}

//----------------------------------------------------------------------------
//      DatIndex
//----------------------------------------------------------------------------

DatIndex::DatIndex()
    : mDatTimestamp(0)
    , mHighestMftEntry(-1)
    , mIsDirty(false)
    , mNumEntries(0)
    , mNumCategories(0)
{
}

DatIndex::~DatIndex()
{
    this->Clear();

    // Notify listeners, so they can clear pointers etc
    for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
        (*it)->OnIndexDestruction(*this);
    }
}

void DatIndex::Clear()
{
    // destruct all entries before clearing their memory
    for (uint i = 0; i < mNumEntries; i++) {
        delete mEntries[i];
    }
    mEntries.Clear();
    // also destruct all categories before clearing their memory
    for (uint i = 0; i < mNumCategories; i++) {
        delete mCategories[i];
    }
    mCategories.Clear();

    mDatTimestamp       = 0;
    mHighestMftEntry    = -1;
    mIsDirty            = false;
    mNumEntries         = 0;
    mNumCategories      = 0;

    // Notify listeners
    for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
        (*it)->OnIndexCleared(*this);
    }
}

DatIndexEntry* DatIndex::AddIndexEntry(bool pSetDirty)
{
    if (mNumEntries == mEntries.GetSize()) {
        if (!ReserveEntries(1)) { return NULL; }
    }

    uint index = mNumEntries++;
    mEntries[index] = new DatIndexEntry(*this);

    mIsDirty = (mIsDirty || pSetDirty);
    return mEntries[index];
}

DatIndexCategory* DatIndex::FindCategory(const wxString& pName, bool pRootsOnly) 
{
    for (uint i = 0; i < mNumCategories; i++) {
        if (!pRootsOnly || !(mCategories[i]->GetParent())) {
            if (mCategories[i]->GetName() == pName) {
                return mCategories[i];
            }
        }
    }
    return NULL;
}

DatIndexCategory* DatIndex::AddIndexCategory(const wxString& pName, bool pSetDirty)
{
    if (mNumCategories == mCategories.GetSize()) {
        if (!ReserveCategories(1)) { return NULL; }
    }

    uint index = mNumCategories++;
    mCategories[index] = new DatIndexCategory(*this, pName, index);
    DatIndexCategory& category = *mCategories[index];

    // Notify listeners
    for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
        (*it)->OnIndexCategoryAdded(*this, category);
    }

    mIsDirty = (mIsDirty || pSetDirty);
    return &category;
}

DatIndexCategory* DatIndex::FindOrAddCategory(const wxString& pName, bool pSetDirty)
{
    DatIndexCategory* category = this->FindCategory(pName, true);
    if (!category) {
        category = this->AddIndexCategory(pName, pSetDirty);
    }
    return category;
}

bool DatIndex::ReserveEntries(uint pAdditionalEntries)
{
    if ((UINT_MAX - mEntries.GetSize()) < pAdditionalEntries) { return false; }
    mEntries.SetSize(mEntries.GetSize() + pAdditionalEntries);
    return true;
}

bool DatIndex::ReserveCategories(uint pAdditionalCategories)
{
    if ((UINT_MAX - mCategories.GetSize()) < pAdditionalCategories) { return false; }
    mCategories.SetSize(mCategories.GetSize() + pAdditionalCategories);
    return true;
}

void DatIndex::AddListener(IDatIndexListener* pListener)
{
    mListeners.insert(pListener);
}

void DatIndex::RemoveListener(IDatIndexListener* pListener)
{
    mListeners.erase(pListener);
}

void DatIndex::OnEntryAddComplete(DatIndexEntry& pEntry)
{
    if ((int)pEntry.GetMftEntry() > mHighestMftEntry) {
        mHighestMftEntry = (int)pEntry.GetMftEntry();
    }

    // Notify listeners
    for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
        (*it)->OnIndexFileAdded(*this, pEntry);
    }
}

};
