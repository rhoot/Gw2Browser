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

#include "ANetStructs.h"

namespace gw2b
{
class DatIndex;
class DatIndexEntry;
class DatIndexCategory;

/** Represents an entry in the .dat index. */
class DatIndexEntry
{
    DatIndex*           m_owner;
    uint32              m_fileId;
    uint32              m_baseId;
    uint32              m_mftEntry;
    ANetFileType        m_fileType;
    DatIndexCategory*   m_category;
    wxString            m_displayName;
public:
    /** Constructor. Sets the various internal values to the given values. */
    DatIndexEntry(DatIndex& p_owner);
    /** Gets the category this entry is contained in.
     *  \return DatIndexCategory*   pointer to the category containing this entry. */
    DatIndexCategory* category()                            { return m_category; }
    /** Gets the const category this entry is contained in.
     *  \return DatIndexCategory*   pointer to the category containing this entry. */
    const DatIndexCategory* category() const                { return m_category; }

    /** Gets this entry's file ID.
     *  \return uint32  file ID associated with entry. */
    uint32 fileId() const                                   { return m_fileId; }
    /** Gets this entry's base ID.
     *  \return uint32  base ID associated with entry. */
    uint32 baseId() const                                   { return m_baseId; }
    /** Gets this entry's MFT entry number.
     *  \return uint32  MFT entry number associated with entry. */
    uint32 mftEntry() const                                 { return m_mftEntry; }
    /** Gets this entry's file type.
     *  \return ANetFileType  file type associated with entry. */
    ANetFileType fileType() const                           { return m_fileType; }
    /** Gets this entry's owner.
     *  \return DatIndex&   owner of this entry. */
    DatIndex& owner()                                       { return *m_owner; }
    /** Gets this entry's owner.
     *  \return DatIndex&   owner of this entry. */
    const DatIndex& owner() const                           { return *m_owner; }
    /** Gets this entry's name.
     *  \return DatIndex&   name of this entry. */
    const wxString& name() const                            { return m_displayName; }

    /** Sets this entry's file ID.
     *  \param[in]  p_fileId     File ID associated with entry. 
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& setFileId(uint32 p_fileId)               { m_fileId = p_fileId; return *this; }
    /** Sets this entry's base ID.
     *  \param[in]  p_baseId     Base ID associated with entry. 
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& setBaseId(uint32 p_baseId)                { m_baseId = p_baseId; return *this; }
    /** Sets this entry's MFT entry number.
     *  \param[in]  p_mftEntry   MFT entry number associated with entry. 
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& setMftEntry(uint32 p_mftEntry)            { m_mftEntry = p_mftEntry; return *this; }
    /** Sets this entry's file type.
     *  \param[in]  p_fileType   File type associated with entry. 
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& setFileType(ANetFileType p_fileType)      { m_fileType = p_fileType; return *this; }
    /** Sets this entry's name.
     *  \param[in]  p_name   name of this entry.
     *  \return DatIndexEntry&  reference to this object. */
    DatIndexEntry& setName(const wxString& p_name)           { m_displayName = p_name; return *this; }

    /** Completes the add operation by notifying the index, so it can notify
     *  its listeners. */
    void finalizeAdd();
    /** Called by pCategory when this entry is added to that category.
     *  \param[in]  p_category   New parent category. */
    void onAddedToCategory(DatIndexCategory* p_category);
};

/** Represents a category of entries and other categories. */
class DatIndexCategory
{
    DatIndex*           m_owner;
    int                 m_index;
    wxString            m_name;
    DatIndexCategory*   m_parent;
    Array<DatIndexCategory*,0x3>  m_subCategories;
    Array<DatIndexEntry*,0x3>     m_entries;
public:
    /** Constructor. Creates a category with the given name and index. 
     *  \param[in]  p_owner  owner index.
     *  \param[in]  p_name   category name.
     *  \param[in]  p_index  category index. */
    DatIndexCategory(DatIndex& p_owner, const wxString& p_name, int p_index);

    /** Gets the number of subcategories this category has.
     *  \return uint    amount of subcategories. */
    uint numSubCategories() const                           { return m_subCategories.GetSize(); }
    /** Gets the subcategory with the given index.
     *  \param[in]  p_index  index of the subcategory to get.
     *  \return DatIndexCategory*   the subcategory with the given index. */
    DatIndexCategory* subCategory(uint p_index)             { if (p_index >= m_subCategories.GetSize()) { return nullptr; } return m_subCategories[p_index]; }
    /** Gets the subcategory with the given index.
     *  \param[in]  p_index  index of the subcategory to get.
     *  \return DatIndexCategory*   the subcategory with the given index. */
    const DatIndexCategory* subCategory(uint p_index) const { if (p_index >= m_subCategories.GetSize()) { return nullptr; } return m_subCategories[p_index]; }
    /** Finds the sub category with the given name and returns it.
     *  \param[in]  p_name   Name of the sub category.
     *  \return DatIndexCategory*   pointer to sub category or nullptr if not found. */
    DatIndexCategory* findSubCategory(const wxString& p_name);
    /** Looks for the category with the given name (top-level only) and 
     *  returns it if found. If not, creates a new category with the name.
     *  \param[in]  p_name   Name of the category.
     *  \return DatIndexCategory*   pointer to the found/new category. */
    DatIndexCategory* findOrAddSubCategory(const wxString& p_name);

    /** Gets the number of entries this category has.
     *  \param[in]  p_recursive  Performs a recursive count, if true.
     *  \return uint    amount of entries. */
    uint numEntries(bool p_recursive = false) const;
    /** Gets the entry with the given index.
     *  \param[in]  p_index  index of the entry to get.
     *  \return DatIndexEntry*  the entry with the given index. */
    DatIndexEntry* entry(uint p_index)                      { if (p_index >= m_entries.GetSize()) { return nullptr; } return m_entries[p_index]; }
    /** Gets the entry with the given index.
     *  \param[in]  p_index  index of the entry to get.
     *  \return DatIndexEntry*  the entry with the given index. */
    const DatIndexEntry* entry(uint p_index) const          { if (p_index >= m_entries.GetSize()) { return nullptr; } return m_entries[p_index]; }
    /** Adds an entry to this category. 
     *  \param[in]  p_entry  Entry to add. */
    void addEntry(DatIndexEntry* p_entry);
    /** Adds a new sub category to this category.
     *  \param[in]  p_subCategory    Category to add. */
    void addSubCategory(DatIndexCategory* p_subCategory);

    /** Sets the name of this category.
     *  \param[in]  p_name   name of the category. */
    void setName(const wxString& p_name)        { m_name = p_name; }
    /** Sets this category's owner.
     *  \param[in]  p_owner  Owner of this category. */
    void setOwner(DatIndex& p_owner)            { m_owner = &p_owner; }
    /** Sets this category's index.
     *  \param[in]  p_index  index of this category. */
    void setIndex(int p_index)                  { m_index = p_index; }
    /** Gets the name of this category.
     *  \return wxString&   name of the category. */
    const wxString& name() const                { return m_name; }
    /** Gets the owner of this category.
     *  \return DatIndex&   owner of the category. */
    DatIndex& owner()                           { return *m_owner; }
    /** Gets the owner of this category.
     *  \return DatIndex&   owner of the category. */
    const DatIndex& owner() const               { return *m_owner; }

    /** Gets this category's parent category, or nullptr if it is top-level.
     *  \return DatIndexCategory*   this category's parent. */
    DatIndexCategory* parent()                  { return m_parent; }
    /** Gets this category's parent category, or nullptr if it is top-level.
     *  \return DatIndexCategory*   this category's parent. */
    const DatIndexCategory* parent() const      { return m_parent; }
    /** Gets this category's index.
     *  \return uint    this category's index. */
    int index() const                           { return m_index; }

    /** Called by the parent category when this category is added to one.
     *  \param[in]  p_category   New parent category. */
    void onAddedToCategory(DatIndexCategory* p_category);
};

/** \interface  IDatIndexListener
 *  Provides callbacks of things happening with the index. */
class IDatIndexListener
{
public:
    /** Raised when an entry was added to the index.
     *  \param[in]  p_index  Reference to the index that had a file added to it.
     *  \param[in]  p_entry  Reference to the newly added entry. */
    virtual void onIndexFileAdded(DatIndex& p_index, const DatIndexEntry& p_entry) {}
    /** Raised when a category was added to the index.
     *  \param[in]  p_index      Reference to the index that had a file added to it.
     *  \param[in]  p_category   Reference to the newly added category. */
    virtual void onIndexCategoryAdded(DatIndex& p_index, const DatIndexCategory& p_category) {}
    /** Raised when the index is cleared.
     *  \param[in]  p_index  Reference to the index being cleared. */
    virtual void onIndexCleared(DatIndex& p_index) {}
    /** Raised when the index is destroyed.
     *  \param[in]  p_index  Reference to the index being destroyed. */
    virtual void onIndexDestruction(DatIndex& p_index) {}
};

/** Represents a .dat index, for faster lookup. */
class DatIndex
{
    typedef Array<DatIndexCategory*>        CategoryArray;
    typedef Array<DatIndexEntry*>           EntryArray;
    typedef std::set<IDatIndexListener*>    ListenerSet;
private:
    CategoryArray       m_categories;
    uint64              m_datTimestamp;
    EntryArray          m_entries;
    int                 m_highestMftEntry;
    bool                m_isDirty;
    ListenerSet         m_listeners;
    uint                m_numEntries;
    uint                m_numCategories;
public:
    /** Constructor. Initializes internals. */
    DatIndex();
    /** Destructor. Clears all data. */
    virtual ~DatIndex();

    /** Clears all data. */
    void clear();
    /** Adds an entry to this index.
     *  \param[in]  p_setDirty   true to flag this index as dirty, false to not.
     *  \return DatIndexEntry&  the newly added entry. */
    DatIndexEntry* addIndexEntry(bool p_setDirty = true);
    /** Looks for the given category and returns it if found. 
     *  \param[in]  p_name       Name of the category to find.
     *  \param[in]  p_rootsOnly  Only find parent-less categories if this is true.
     *  \return DatIndexCategory*   pointer to found category, or nullptr if not found. */
    DatIndexCategory* findCategory(const wxString& p_name, bool p_rootsOnly = false);
    /** Creates a new category and returns it.
     *  \param[in]  p_name       Name of the category to create.
     *  \param[in]  p_setDirty   true to flag this index as dirty, false to not.
     *  \return DatIndexCategory*   pointer to the new category. */
    DatIndexCategory* addIndexCategory(const wxString& p_name, bool p_setDirty = true);
    /** Looks for the category with the given name (top-level only) and 
     *  returns it if found. If not, creates a new category with the name.
     *  \param[in]  p_name       Name of the category.
     *  \param[in]  p_setDirty   true to flag this index as dirty, false to not.
     *  \return DatIndexCategory*   pointer to the found/new category. */
    DatIndexCategory* findOrAddCategory(const wxString& p_name, bool p_setDirty = true);
    /** Reserves memory for a given amount of entries.
     *  \param[in]  p_additionalEntries  How many additional entries to reserve
     *                                  memory for.
     *  \return bool    true if successful, false if not. */
    bool reserveEntries(uint p_additionalEntries);
    /** Reserves memory for a given amount of categories.
     *  \param[in]  p_additionalCategories  How many additional entries to reserve
     *              memory for.
     *  \return bool    true if successful, false if not. */
    bool reserveCategories(uint p_additionalCategories);

    /** Gets the amount of entries in this index.
     *  \return uint    Amount of entries. */
    uint numEntries() const                             { return m_numEntries; }
    /** Gets the amount of categories in this index.
     *  \return uint    Amount of categories. */
    uint numCategories() const                          { return m_numCategories; }
    /** Gets the entry with the given index.
     *  \param[in]  p_index  Index of the entry to get.
     *  \return DatIndexEntry*  Const pointer to the entry if valid, nullptr if not. */
    const DatIndexEntry* entry(uint p_index) const      { if (p_index >= m_numEntries) { return nullptr; } return m_entries[p_index]; }
    /** Gets the category with the given index.
     *  \param[in]  p_index  Index of the category to get.
     *  \return DatIndexCategory*   pointer to the category if valid, nullptr if not. */
    DatIndexCategory* category(uint p_index)            { if (p_index >= m_numCategories) { return nullptr; } return m_categories[p_index]; }
    /** Gets the category with the given index.
     *  \param[in]  p_index  Index of the category to get.
     *  \return DatIndexCategory*   Const pointer to the category if valid, nullptr if not. */
    const DatIndexCategory* category(uint p_index) const { if (p_index >= m_numCategories) { return nullptr; } return m_categories[p_index]; }

    /** Return the highest available MFT entry found in the index.
     *  \return uint    Highest MFT entry found in the table. */
    int highestMftEntry() const                 { return m_highestMftEntry; }
    /** Returns whether or not the data has been changed since writing to file.
     *  \return bool    true if data is dirty, false if not. */
    bool isDirty() const                        { return m_isDirty; }
    /** Sets the dirty flag for this index.
     *  \param[in]  p_isDirty    New dirty flag. */
    void setDirty(bool p_isDirty)               { m_isDirty = p_isDirty; }

    /** Gets the .dat timestamp stored for this index. 
     *  \return uint64  .dat file timestamp. */
    uint64 datTimestamp() const                 { return m_datTimestamp; }
    /** Sets the .dat timestamp stored for this index.
     *  \param[in]  p_timestamp  .dat file timestamp. */
    void setDatTimestamp(uint64 p_timestamp)    { m_datTimestamp = p_timestamp; }

    /** Adds an event listener to this object.
     *  \param[in]  p_listener   Listener to attach to this object. */
    void addListener(IDatIndexListener* p_listener);
    /** Removes an event listener from this object.
     *  \param[in]  p_listener   Listener to remove from this object. */
    void removeListener(IDatIndexListener* p_listener);

    /** Called by DatIndexEntry upon calling FinalizeAdd(). Notifies this
     *  index's listeners.
     *  \param[in]  p_entry  Entry that was just added. */
    void onEntryAddComplete(DatIndexEntry& p_entry);
}; // class DatIndex

}; // namespace gw2b

#endif // DATINDEX_H_INCLUDED
