/** \file       CategoryTree.h
 *  \brief      Contains declaration of the .dat structure tree control.
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

#ifndef CATEGORYTREE_H_INCLUDED
#define CATEGORYTREE_H_INCLUDED

#include <wx/treectrl.h>
#include <set>
#include "DatIndex.h"

namespace gw2b
{
class CategoryTree;

/** Represents the data associated with an item in the tree control. */
class CategoryTreeItem : public wxTreeItemData
{
public:
    /** Type of data this entry represents. */
    enum DataType {
        DT_Category,    /**< The entry is an index category. */
        DT_Entry,       /**< The entry is an index entry. */
    };
private:
    const void* mData;
    DataType    mDataType;
    bool        mIsDirty;
public:
    /** Constructor. Sets the properties of this object to the given values.
     *  \param[in]  pType   type of entry.
     *  \param[in]  pData   pointer to the data represented by this entry. */
    CategoryTreeItem(DataType pType, const void* pData) : mData(pData), mDataType(pType), mIsDirty(false) {}
    /** Destructor. */
    virtual ~CategoryTreeItem() {}
    /** Gets the type of data represented by this entry.
     *  \return DataType    type of data represented by the entry. */
    DataType GetDataType() const { return mDataType; }
    /** Gets the data represented by this entry.
     *  \return void*   pointer to the represented data. */
    const void* GetData() const { return mData; }
    /** Marks this object as dirty. Only used for categories that are collapsed
     *  but have had new entries added to it. Entries that are yet to be created.
     *  \param[in]  pDirty  new dirty flag. */
    void SetDirty(bool pDirty) { mIsDirty = pDirty; }
    /** Determines whether this object is dirty or not.
     *  \return bool    true if object is dirty, false if not. */
    bool IsDirty() const { return mIsDirty; }
}; // class CategoryTreeItem

/** Image list used to show icons in the category tree. */
class CategoryTreeImageList : public wxImageList
{
public:
    /** The various types of icons contained within this image list. */
    enum IconType
    {
        IT_OpenFolder       = 0,        /**< Open folder icon. */
        IT_UnknownFile      = 1,        /**< Unknown file icon. */
        IT_ClosedFolder     = 2,        /**< Closed folder icon. */
        IT_Executable       = 3,        /**< Executable file icon. */
        IT_Dll              = 4,        /**< DLL file icon. */
        IT_Image            = 5,        /**< Image file icon. */
        IT_Text             = 6,        /**< Text file icon. */
    };
public:
    /** Constructor. Loads the images needed by this image list. */
    CategoryTreeImageList();
    /** Destructor. */
    virtual ~CategoryTreeImageList();
}; // class CategoryTreeImageList

/** \interface  ICategoryTreeListener
 *  Receives events from the category tree. */
class ICategoryTreeListener
{
public:
    /** Raised when the user wants to extract raw files.
     *  \param[in]  pTree   category tree invoking the callback. */
    virtual void OnTreeExtractRaw(CategoryTree& pTree) {}
    /** Raised when the user wants to extract converted files.
     *  \param[in]  pTree   category tree invoking the callback. */
    virtual void OnTreeExtractConverted(CategoryTree& pTree) {}
    /** Raised whenever a non-category entry is clicked in the category tree. 
     *  \param[in]  pTree   category tree invoking the callback.
     *  \param[in]  pEntry  reference to the clicked entry. */
    virtual void OnTreeEntryClicked(CategoryTree& pTree, const DatIndexEntry& pEntry) {}
    /** Raised whenever a category entry is clicked in the category tree. 
     *  \param[in]  pTree       category tree invoking the callback.
     *  \param[in]  pCategory   reference to the clicked category. */
    virtual void OnTreeCategoryClicked(CategoryTree& pTree, const DatIndexCategory& pCategory) {}
    /** Raised when the category tree is being cleared.
     *  \param[in]  pTree   category tree invoking the callback. */
    virtual void OnTreeCleared(CategoryTree& pTree) {}
    /** Raised when the category tree is being destroyed.
     *  \param[in]  pTree   category tree invoking the callback. */
    virtual void OnTreeDestruction(CategoryTree& pTree) {}
};

/** Tree control containing categories and entries of a DatIndex. */
class CategoryTree : public wxTreeCtrl, public IDatIndexListener
{
    typedef std::set<ICategoryTreeListener*>    ListenerSet;
private:
    AutoPtr<DatIndex>   mIndex;
    ListenerSet         mListeners;
public:
    /** Constructor. Creates the tree control with the given parent.
     *  \param[in]  pParent     Parent of the control.
     *  \param[in]  pLocation   Optional location of the control. 
     *  \param[in]  pSize       Optional size of the control. */
    CategoryTree(wxWindow* pParent, const wxPoint& pLocation = wxDefaultPosition, const wxSize& pSize = wxDefaultSize);
    /** Destructor. */
    virtual ~CategoryTree();

    /** Adds an index entry to this tree.
     *  \param[in]  pEntry  Entry to add. */
    void AddEntry(const DatIndexEntry& pEntry);
    /** Ensures that the given category is part of the tree. Note that the
     *  category is \e not added if a parent category is collapsed. If it
     *  already exists, its id is returned and no category is added.
     *  \param[in]  pCategory   Category to add to the tree.
     *  \param[in]  pForce      Forcefully add the category, even if the parent
     *              is collapsed. */
    wxTreeItemId EnsureHasCategory(const DatIndexCategory& pCategory, bool pForce = false);
    /** Clears all entries from the tree. */
    void ClearEntries();
    /** Gets the currently selected objects.
     *  \return Array<DatIndexEntry*>  array of entries. */
    Array<const DatIndexEntry*> GetSelectedEntries() const;

    /** Gets the .dat file index represented by this tree. */
    DatIndex* GetIndex();
    /** Gets the .dat file index represented by this tree. */
    const DatIndex* GetIndex() const;
    /** Sets the .dat file index represented by this tree. 
     *  \param[in]  pIndex  Index this tree should represent. */
    void SetIndex(DatIndex* pIndex);

    /** Adds an event listener to this tree.
     *  \param  pListener   Pointer to the listener to add. */
    void AddListener(ICategoryTreeListener* pListener);
    /** Removes an event listener from this tree.
     *  \param  pListener   Pointer to the listener to remove. */
    void RemoveListener(ICategoryTreeListener* pListener);

    /** Called by the .dat index when an entry is added.
     *  \param[in]  pIndex  Reference to the index that had a file added to it.
     *  \param[in]  pEntry  Reference to the newly added entry. */
    virtual void OnIndexFileAdded(DatIndex& pIndex, const DatIndexEntry& pEntry);
    /** Called by the .dat index when it is cleared.
     *  \param[in]  pIndex  Reference to the index being cleared. */
    virtual void OnIndexCleared(DatIndex& pIndex);
    /** Called by the .dat index when it is destroyed.
     *  \param[in]  pIndex  Reference to the index being destroyed. */
    virtual void OnIndexDestruction(DatIndex& pIndex);
private:
    void AddCategoryEntriesToArray(Array<const DatIndexEntry*>& pArray, uint& pIndex, const DatIndexCategory& pCategory) const;

    /** Helper method to add an entry to the tree at the right spot, for sorting.
     *  \param[in]  pParent     Category to add the entry to.
     *  \param[in]  pEntry      Entry to add. */
    wxTreeItemId AddEntry(const wxTreeItemId& pParent, const DatIndexEntry& pEntry);
    /** Helper method to add an entry to the tree at the right spot, for sorting.
     *  \param[in]  pParent     Category to add the entry to.
     *  \param[in]  pEntry      Entry to add.
     *  \param[in]  pName       Name of the entry, as an integer. */
    wxTreeItemId AddNumberEntry(const wxTreeItemId& pParent, const DatIndexEntry& pEntry, uint pName);
    /** Helper method to add an entry to the tree at the right spot, for sorting.
     *  \param[in]  pParent     Category to add the entry to.
     *  \param[in]  pEntry      Entry to add. */
    wxTreeItemId AddTextEntry(const wxTreeItemId& pParent, const DatIndexEntry& pEntry);
    /** Helper method to add a category to the tree at the right spot, for sorting.
     *  \param[in]  pParent         Parent category to add the category to.
     *  \param[in]  pDisplayName    Name of the category to add. */
    wxTreeItemId AddCategoryEntry(const wxTreeItemId& pParent, const wxString& pDisplayName);

    /** Gets the index for the image that should represent the given entry.
     *  \param[in]  pEntry  Entry in need of an icon. */
    int GetImageForEntry(const DatIndexEntry& pEntry);
    /** Event raised when an item is being expanded.
     *  \param[in]  pEvent  Event object handed to us by wxWidgets. */
    void OnItemExpanding(wxTreeEvent& pEvent);
    /** Event raised when an item is being collapsed.
     *  \param[in]  pEvent  Event object handed to us by wxWidgets. */
    void OnItemCollapsing(wxTreeEvent& pEvent);
    /** Event raised when an item is being selected.
     *  \param[in]  pEvent  Event object handed to us by wxWidgets. */
    void OnSelChanged(wxTreeEvent& pEvent);
    /** Event raised when an item has been right clicked on.
     *  \param[in]  pEvent  Event object handed to us by wxWidgets. */
    void OnContextMenu(wxTreeEvent& pEvent);
    /** Event raised when the user wants to extract raw files.
     *  \param[in]  pEvent  Event object handed to us by wxWidgets. */
    void OnExtractRawFiles(wxCommandEvent& pEvent);
    /** Event raised when the user wants to extract converted files.
     *  \param[in]  pEvent  Event object handed to us by wxWidgets. */
    void OnExtractConvertedFiles(wxCommandEvent& pEvent);
}; // class CategoryTree

}; // namespace gw2b

#endif // CATEGORYTREE_H_INCLUDED
