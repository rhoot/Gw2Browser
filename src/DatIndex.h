/** \file       DatIndex.h
 *  \brief      Contains the declaration for the index model class.
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

#ifndef DATINDEX_H_INCLUDED
#define DATINDEX_H_INCLUDED

#include <wx/filename.h>
#include <set>

#include "Util/RefCounted.h"
#include "Util/Array.h"
#include "ANetStructs.h"

namespace gw2b
{
class DatIndex;
class DatIndexEntry;
class DatIndexCategory;

/** Represents an entry in the .dat index. */
class DatIndexEntry
{
    DatIndex*           mOwner;
    uint32              mFileId;
    uint32              mBaseId;
    uint32              mMftEntry;
    ANetFileType        mFileType;
    DatIndexCategory*   mCategory;
    wxString            mDisplayName;
public:
    /** Constructor. Sets the various internal values to the given values. */
    DatIndexEntry(DatIndex& pOwner);
    /** Gets the category this entry is contained in.
     *  \return DatIndexCategory*   pointer to the category containing this entry. */
    DatIndexCategory* GetCategory()                         { return mCategory; }
    /** Gets the const category this entry is contained in.
     *  \return DatIndexCategory*   pointer to the category containing this entry. */
    const DatIndexCategory* GetCategory() const             { return mCategory; }

    /** Gets this entry's file ID.
     *  \return uint32  file ID associated with entry. */
    uint32 GetFileId() const                                { return mFileId; }
    /** Gets this entry's base ID.
     *  \return uint32  base ID associated with entry. */
    uint32 GetBaseId() const                                { return mBaseId; }
    /** Gets this entry's MFT entry number.
     *  \return uint32  MFT entry number associated with entry. */
    uint32 GetMftEntry() const                              { return mMftEntry; }
    /** Gets this entry's file type.
     *  \return ANetFileType  file type associated with entry. */
    ANetFileType GetFileType() const                        { return mFileType; }
    /** Gets this entry's owner.
     *  \return DatIndex&   owner of this entry. */
    DatIndex& GetOwner()                                    { return *mOwner; }
    /** Gets this entry's owner.
     *  \return DatIndex&   owner of this entry. */
    const DatIndex& GetOwner() const                        { return *mOwner; }
    /** Gets this entry's name.
     *  \return DatIndex&   name of this entry. */
    const wxString& GetName() const                         { return mDisplayName; }

    /** Sets this entry's file ID.
     *  \param[in]  pFileId     File ID associated with entry. 
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& SetFileId(uint32 pFileId)                { mFileId = pFileId; return *this; }
    /** Sets this entry's base ID.
     *  \param[in]  pBaseId     Base ID associated with entry. 
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& SetBaseId(uint32 pBaseId)                { mBaseId = pBaseId; return *this; }
    /** Sets this entry's MFT entry number.
     *  \param[in]  pMftEntry   MFT entry number associated with entry. 
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& SetMftEntry(uint32 pMftEntry)            { mMftEntry = pMftEntry; return *this; }
    /** Sets this entry's file type.
     *  \param[in]  pFileType   File type associated with entry. 
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& SetFileType(ANetFileType pFileType)      { mFileType = pFileType; return *this; }
    /** Sets this entry's name.
     *  \param[in]  pName   name of this entry.
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& SetName(const wxString& pName)           { mDisplayName = pName; return *this; }

    /** Completes the add operation by notifying the index, so it can notify
     *  its listeners. */
    void FinalizeAdd();
    /** Called by pCategory when this entry is added to that category.
     *  \param[in]  pCategory   New parent category. */
    void OnAddedToCategory(DatIndexCategory* pCategory);
};

/** Represents a category of entries and other categories. */
class DatIndexCategory
{
    DatIndex*           mOwner;
    int                 mIndex;
    wxString            mName;
    DatIndexCategory*   mParent;
    Array<DatIndexCategory*,0x3>  mSubCategories;
    Array<DatIndexEntry*,0x3>     mEntries;
public:
    /** Constructor. Creates a category with the given name and index. 
     *  \param[in]  pOwner  owner index.
     *  \param[in]  pName   category name.
     *  \param[in]  pIndex  category index. */
    DatIndexCategory(DatIndex& pOwner, const wxString& pName, int pIndex);

    /** Gets the number of subcategories this category has.
     *  \return uint    amount of subcategories. */
    uint GetNumSubCategories() const                { return mSubCategories.GetSize(); }
    /** Gets the subcategory with the given index.
     *  \param[in]  pIndex  index of the subcategory to get.
     *  \return DatIndexCategory*   the subcategory with the given index. */
    DatIndexCategory* GetSubCategory(uint pIndex)   { if (pIndex >= mSubCategories.GetSize()) { return NULL; } return mSubCategories[pIndex]; }
    /** Gets the subcategory with the given index.
     *  \param[in]  pIndex  index of the subcategory to get.
     *  \return DatIndexCategory*   the subcategory with the given index. */
    const DatIndexCategory* GetSubCategory(uint pIndex) const { if (pIndex >= mSubCategories.GetSize()) { return NULL; } return mSubCategories[pIndex]; }
    /** Finds the sub category with the given name and returns it.
     *  \param[in]  pName   Name of the sub category.
     *  \return DatIndexCategory*   pointer to sub category or NULL if not found. */
    DatIndexCategory* FindSubCategory(const wxString& pName);
    /** Looks for the category with the given name (top-level only) and 
     *  returns it if found. If not, creates a new category with the name.
     *  \param[in]  pName   Name of the category.
     *  \return DatIndexCategory*   pointer to the found/new category. */
    DatIndexCategory* FindOrAddSubCategory(const wxString& pName);

    /** Gets the number of entries this category has.
     *  \param[in]  pRecursive  Performs a recursive count, if true.
     *  \return uint    amount of entries. */
    uint GetNumEntries(bool pRecursive = false) const;
    /** Gets the entry with the given index.
     *  \param[in]  pIndex  index of the entry to get.
     *  \return DatIndexEntry*  the entry with the given index. */
    DatIndexEntry* GetEntry(uint pIndex)                { if (pIndex >= mEntries.GetSize()) { return NULL; } return mEntries[pIndex]; }
    /** Gets the entry with the given index.
     *  \param[in]  pIndex  index of the entry to get.
     *  \return DatIndexEntry*  the entry with the given index. */
    const DatIndexEntry* GetEntry(uint pIndex) const    { if (pIndex >= mEntries.GetSize()) { return NULL; } return mEntries[pIndex]; }
    /** Adds an entry to this category. 
     *  \param[in]  pEntry  Entry to add. */
    void AddEntry(DatIndexEntry* pEntry);
    /** Adds a new sub category to this category.
     *  \param[in]  pSubCategory    Category to add. */
    void AddSubCategory(DatIndexCategory* pSubCategory);

    /** Sets the name of this category.
     *  \param[in]  pName   name of the category. */
    void SetName(const wxString& pName)         { mName = pName; }
    /** Sets this category's owner.
     *  \param[in]  pOwner  Owner of this category. */
    void SetOwner(DatIndex& pOwner)             { mOwner = &pOwner; }
    /** Sets this category's index.
     *  \param[in]  pIndex  index of this category. */
    void SetIndex(int pIndex)                   { mIndex = pIndex; }
    /** Gets the name of this category.
     *  \return wxString&   name of the category. */
    const wxString& GetName() const             { return mName; }
    /** Gets the owner of this category.
     *  \return DatIndex&   owner of the category. */
    DatIndex& GetOwner()                        { return *mOwner; }
    /** Gets the owner of this category.
     *  \return DatIndex&   owner of the category. */
    const DatIndex& GetOwner() const            { return *mOwner; }

    /** Gets this category's parent category, or NULL if it is top-level.
     *  \return DatIndexCategory*   this category's parent. */
    DatIndexCategory* GetParent()               { return mParent; }
    /** Gets this category's parent category, or NULL if it is top-level.
     *  \return DatIndexCategory*   this category's parent. */
    const DatIndexCategory* GetParent() const   { return mParent; }
    /** Gets this category's index.
     *  \return uint    this category's index. */
    int GetIndex() const                        { return mIndex; }

    /** Called by the parent category when this category is added to one.
     *  \param[in]  pCategory   New parent category. */
    void OnAddedToCategory(DatIndexCategory* pCategory);
};

/** \interface  IDatIndexListener
 *  Provides callbacks of things happening with the index. */
class IDatIndexListener
{
public:
    /** Raised when an entry was added to the index.
     *  \param[in]  pIndex  Reference to the index that had a file added to it.
     *  \param[in]  pEntry  Reference to the newly added entry. */
    virtual void OnIndexFileAdded(DatIndex& pIndex, const DatIndexEntry& pEntry) {}
    /** Raised when a category was added to the index.
     *  \param[in]  pIndex      Reference to the index that had a file added to it.
     *  \param[in]  pCategory   Reference to the newly added category. */
    virtual void OnIndexCategoryAdded(DatIndex& pIndex, const DatIndexCategory& pCategory) {}
    /** Raised when the index is cleared.
     *  \param[in]  pIndex  Reference to the index being cleared. */
    virtual void OnIndexCleared(DatIndex& pIndex) {}
    /** Raised when the index is destroyed.
     *  \param[in]  pIndex  Reference to the index being destroyed. */
    virtual void OnIndexDestruction(DatIndex& pIndex) {}
};

/** Represents a .dat index, for faster lookup. */
class DatIndex : public RefCounted
{
    typedef Array<DatIndexCategory*>        CategoryArray;
    typedef Array<DatIndexEntry*>           EntryArray;
    typedef std::set<IDatIndexListener*>    ListenerSet;
private:
    CategoryArray       mCategories;
    uint64              mDatTimestamp;
    EntryArray          mEntries;
    int                 mHighestMftEntry;
    bool                mIsDirty;
    ListenerSet         mListeners;
    uint                mNumEntries;
    uint                mNumCategories;
public:
    /** Constructor. Initializes internals. */
    DatIndex();
    /** Destructor. Clears all data. */
    virtual ~DatIndex();

    /** Clears all data. */
    void Clear();
    /** Adds an entry to this index.
     *  \param[in]  pSetDirty   true to flag this index as dirty, false to not.
     *  \return DatIndexEntry&  the newly added entry. */
    DatIndexEntry* AddIndexEntry(bool pSetDirty = true);
    /** Looks for the given category and returns it if found. 
     *  \param[in]  pName       Name of the category to find.
     *  \param[in]  pRootsOnly  Only find parent-less categories if this is true.
     *  \return DatIndexCategory*   pointer to found category, or NULL if not found. */
    DatIndexCategory* FindCategory(const wxString& pName, bool pRootsOnly = false);
    /** Creates a new category and returns it.
     *  \param[in]  pName       Name of the category to create.
     *  \param[in]  pSetDirty   true to flag this index as dirty, false to not.
     *  \return DatIndexCategory*   pointer to the new category. */
    DatIndexCategory* AddIndexCategory(const wxString& pName, bool pSetDirty = true);
    /** Looks for the category with the given name (top-level only) and 
     *  returns it if found. If not, creates a new category with the name.
     *  \param[in]  pName       Name of the category.
     *  \param[in]  pSetDirty   true to flag this index as dirty, false to not.
     *  \return DatIndexCategory*   pointer to the found/new category. */
    DatIndexCategory* FindOrAddCategory(const wxString& pName, bool pSetDirty = true);
    /** Reserves memory for a given amount of entries.
     *  \param[in]  pAdditionalEntries  How many additional entries to reserve
     *                                  memory for.
     *  \return bool    true if successful, false if not. */
    bool ReserveEntries(uint pAdditionalEntries);
    /** Reserves memory for a given amount of categories.
     *  \param[in]  pAdditionalCategories  How many additional entries to reserve
     *              memory for.
     *  \return bool    true if successful, false if not. */
    bool ReserveCategories(uint pAdditionalCategories);

    /** Gets the amount of entries in this index.
     *  \return uint    Amount of entries. */
    uint GetNumEntries() const                  { return mNumEntries; }
    /** Gets the amount of categories in this index.
     *  \return uint    Amount of categories. */
    uint GetNumCategories() const               { return mNumCategories; }
    /** Gets the entry with the given index.
     *  \param[in]  pIndex  Index of the entry to get.
     *  \return DatIndexEntry*  Const pointer to the entry if valid, NULL if not. */
    const DatIndexEntry* GetEntry(uint pIndex) const { if (pIndex >= mNumEntries) { return NULL; } return mEntries[pIndex]; }
    /** Gets the category with the given index.
     *  \param[in]  pIndex  Index of the category to get.
     *  \return DatIndexCategory*   pointer to the category if valid, NULL if not. */
    DatIndexCategory* GetCategory(uint pIndex)  { if (pIndex >= mNumCategories) { return NULL; } return mCategories[pIndex]; }
    /** Gets the category with the given index.
     *  \param[in]  pIndex  Index of the category to get.
     *  \return DatIndexCategory*   Const pointer to the category if valid, NULL if not. */
    const DatIndexCategory* GetCategory(uint pIndex) const { if (pIndex >= mNumCategories) { return NULL; } return mCategories[pIndex]; }

    /** Return the highest available MFT entry found in the index.
     *  \return uint    Highest MFT entry found in the table. */
    int GetHighestMftEntry() const              { return mHighestMftEntry; }
    /** Returns whether or not the data has been changed since writing to file.
     *  \return bool    true if data is dirty, false if not. */
    bool IsDirty() const                        { return mIsDirty; }
    /** Sets the dirty flag for this index.
     *  \param[in]  pIsDirty    New dirty flag. */
    void SetDirty(bool pIsDirty)                { mIsDirty = pIsDirty; }

    /** Gets the .dat timestamp stored for this index. 
     *  \return uint64  .dat file timestamp. */
    uint64 GetDatTimeStamp() const              { return mDatTimestamp; }
    /** Sets the .dat timestamp stored for this index.
     *  \param[in]  pTimestamp  .dat file timestamp. */
    void SetDatTimeStamp(uint64 pTimestamp)     { mDatTimestamp = pTimestamp; }

    /** Adds an event listener to this object.
     *  \param[in]  pListener   Listener to attach to this object. */
    void AddListener(IDatIndexListener* pListener);
    /** Removes an event listener from this object.
     *  \param[in]  pListener   Listener to remove from this object. */
    void RemoveListener(IDatIndexListener* pListener);

    /** Called by DatIndexEntry upon calling FinalizeAdd(). Notifies this
     *  index's listeners.
     *  \param[in]  pEntry  Entry that was just added. */
    void OnEntryAddComplete(DatIndexEntry& pEntry);
}; // class DatIndex

}; // namespace gw2b

#endif // DATINDEX_H_INCLUDED
