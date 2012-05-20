/** \file       CategoryTree.h
 *  \brief      Contains definition of the .dat structure tree control.
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
#include "CategoryTree.h"

#include "Data.h"

namespace gw2b
{

//----------------------------------------------------------------------------
//      CategoryTreeImageList
//----------------------------------------------------------------------------

CategoryTreeImageList::CategoryTreeImageList()
    : wxImageList(16, 16, true, 2)
{
    this->Add(data::LoadPNG(data::open_folder_png,   data::open_folder_png_size));
    this->Add(data::LoadPNG(data::unknown_png,       data::unknown_png_size));
    this->Add(data::LoadPNG(data::closed_folder_png, data::closed_folder_png_size));
    this->Add(data::LoadPNG(data::exe_png,           data::exe_png_size));
    this->Add(data::LoadPNG(data::dll_png,           data::dll_png_size));
    this->Add(data::LoadPNG(data::image_png,         data::image_png_size));
    this->Add(data::LoadPNG(data::text_png,          data::text_png_size));
}

CategoryTreeImageList::~CategoryTreeImageList()
{
}

//----------------------------------------------------------------------------
//      CategoryTree
//----------------------------------------------------------------------------

CategoryTree::CategoryTree(wxWindow* pParent, const wxPoint& pLocation, const wxSize& pSize)
    : wxTreeCtrl(pParent, wxID_ANY, pLocation, pSize, wxTR_HIDE_ROOT | wxTR_TWIST_BUTTONS | wxTR_DEFAULT_STYLE | wxTR_MULTIPLE)
{
    // Initialize tree
    CategoryTreeImageList* images = new CategoryTreeImageList();
    this->AssignImageList(images);
    this->AddRoot(wxT("Root"));

    // Hookup events
    this->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(CategoryTree::OnItemExpanding));
    this->Connect(wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler(CategoryTree::OnItemCollapsing));
    this->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(CategoryTree::OnSelChanged));
    this->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(CategoryTree::OnContextMenu));
    this->Connect(wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CategoryTree::OnExtractConvertedFiles));
    this->Connect(wxID_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CategoryTree::OnExtractRawFiles));
}

CategoryTree::~CategoryTree()
{
    if (mIndex) {
        mIndex->RemoveListener(this);
    }

    for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
        (*it)->OnTreeDestruction(*this);
    }
}

void CategoryTree::AddEntry(const DatIndexEntry& pEntry)
{
    wxTreeItemId category = this->EnsureHasCategory(*pEntry.GetCategory());
    if (category.IsOk()) {
        if (this->IsExpanded(category)) {
            wxTreeItemId node = this->AddEntry(category, pEntry);
            this->SetItemData(node, new CategoryTreeItem(CategoryTreeItem::DT_Entry, &pEntry));
        } else {
            CategoryTreeItem* itemData = (CategoryTreeItem*)this->GetItemData(category);
            if (itemData->GetDataType() != CategoryTreeItem::DT_Category) { return; }
            itemData->SetDirty(true);
        }
    }
}

wxTreeItemId CategoryTree::EnsureHasCategory(const DatIndexCategory& pCategory, bool pForce) 
{
    wxTreeItemId parent;
    bool parentIsRoot = false;

    // Determine parent node
    if (pCategory.GetParent()) {
        parent = this->EnsureHasCategory(*pCategory.GetParent());
    } else {
        parent = this->GetRootItem();
        parentIsRoot = true;
    }

    if (!pForce) {
        // If parent is invalid, it means the tree isn't expanded and we shouldn't add this
        if (!parentIsRoot && (!parent.IsOk() || !this->IsExpanded(parent))) {
            return wxTreeItemId();
        }
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = this->GetFirstChild(parent, cookie);

    // Scan for existing node
    while (child.IsOk()) {
        CategoryTreeItem* data = (CategoryTreeItem*)this->GetItemData(child);
        if (data->GetDataType() != CategoryTreeItem::DT_Category) { break; }
        if (data->GetData() == &pCategory) { return child; }
        child = this->GetNextChild(parent, cookie);
    }

    // Node does not exist, add it
    wxTreeItemId thisNode = this->AddCategoryEntry(parent, pCategory.GetName());
    CategoryTreeItem* itemData = new CategoryTreeItem(CategoryTreeItem::DT_Category, &pCategory);
    itemData->SetDirty(true);
    this->SetItemData(thisNode, itemData);
    // All category nodes have children
    this->SetItemHasChildren(thisNode);
    return thisNode;
}

wxTreeItemId CategoryTree::AddEntry(const wxTreeItemId& pParent, const DatIndexEntry& pEntry)
{
    if (pEntry.GetName().IsNumber()) {
        ulong number;
        pEntry.GetName().ToULong(&number);
        return this->AddNumberEntry(pParent, pEntry, number);
    }

    // NaN :)
    return this->AddTextEntry(pParent, pEntry);
}

wxTreeItemId CategoryTree::AddNumberEntry(const wxTreeItemId& pParent, const DatIndexEntry& pEntry, uint pName) {
    wxTreeItemIdValue cookie;
    wxTreeItemId child = this->GetFirstChild(pParent, cookie);
    wxTreeItemId previous;

    while (child.IsOk()) {
        wxString text = this->GetItemText(child);
        if (!text.IsNumber()) { break; }
        // Convert to uint
        ulong number;
        text.ToULong(&number);
        // Compare
        if (number > pName) { break; }
        // Move to next
        previous = child;
        child    = this->GetNextChild(pParent, cookie);
    }

    // This item should be first if there *is* something in this list, but previous is nothing
    if (child.IsOk() && !previous.IsOk()) {
        return this->InsertItem(pParent, 0, pEntry.GetName(), this->GetImageForEntry(pEntry));
    }
    // This item should be squashed in if both child and previous are ok
    if (child.IsOk() && previous.IsOk()) {
        return this->InsertItem(pParent, previous, pEntry.GetName(), this->GetImageForEntry(pEntry));
    }
    // If the above fails, it means we went through the entire list without finding a proper spot
    return this->AppendItem(pParent, pEntry.GetName(), this->GetImageForEntry(pEntry));
}

wxTreeItemId CategoryTree::AddTextEntry(const wxTreeItemId& pParent, const DatIndexEntry& pEntry)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = this->GetFirstChild(pParent, cookie);
    wxTreeItemId previous;

    while (child.IsOk()) {
        wxString text = this->GetItemText(child);
        // Compare
        if (text > pEntry.GetName()) { break; }
        // Move to next
        previous = child;
        child    = this->GetNextChild(pParent, cookie);
    }

    // This item should be first if there *is* something in this list, but previous is nothing
    if (child.IsOk() && !previous.IsOk()) {
        return this->InsertItem(pParent, 0, pEntry.GetName(), this->GetImageForEntry(pEntry));
    }
    // This item should be squashed in if both child and previous are ok
    if (child.IsOk() && previous.IsOk()) {
        return this->InsertItem(pParent, previous, pEntry.GetName(), this->GetImageForEntry(pEntry));
    }
    // If the above fails, it means we went through the entire list without finding a proper spot
    return this->AppendItem(pParent, pEntry.GetName(), this->GetImageForEntry(pEntry));
}

wxTreeItemId CategoryTree::AddCategoryEntry(const wxTreeItemId& pParent, const wxString& pDisplayName)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = this->GetFirstChild(pParent, cookie);
    wxTreeItemId previous;

    while (child.IsOk()) {
        wxString text = this->GetItemText(child);
        // Compare
        if (text > pDisplayName) { break; }
        // Categories have NULL item data
        CategoryTreeItem* data = (CategoryTreeItem*)this->GetItemData(child);
        if (data->GetDataType() != CategoryTreeItem::DT_Category) { break; }
        // Move to next
        previous = child;
        child    = this->GetNextChild(pParent, cookie);
    }

    // This item should be first if there *is* something in this list, but previous is nothing
    if (child.IsOk() && !previous.IsOk()) {
        return this->InsertItem(pParent, 0, pDisplayName, CategoryTreeImageList::IT_ClosedFolder);
    }
    // This item should be squashed in if both child and previous are ok
    if (child.IsOk() && previous.IsOk()) {
        return this->InsertItem(pParent, previous, pDisplayName, CategoryTreeImageList::IT_ClosedFolder);
    }
    // If the above fails, it means we went through the entire list without finding a proper spot
    return this->AppendItem(pParent, pDisplayName, CategoryTreeImageList::IT_ClosedFolder);
}

void CategoryTree::ClearEntries()
{
    this->DeleteAllItems();
    this->AddRoot(wxT("Root"));

    for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
        (*it)->OnTreeCleared(*this);
    }
}

Array<const DatIndexEntry*>* CategoryTree::GetSelectedEntries() const
{
    wxArrayTreeItemIds ids;
    this->GetSelections(ids);
    if (ids.GetCount() == 0) { return new Array<const DatIndexEntry*>(); }

    // Doing this in two steps since reallocating takes far longer than iterating

    // Start with counting the total amount of entries
    uint count = 0;
    for (uint i = 0; i < ids.Count(); i++) {
        const CategoryTreeItem* itemData = (const CategoryTreeItem*)this->GetItemData(ids[i]);
        if (itemData->GetDataType() == CategoryTreeItem::DT_Entry) {
            count++;
        } else if (itemData->GetDataType() == CategoryTreeItem::DT_Category) {
            count += ((const DatIndexCategory*)itemData->GetData())->GetNumEntries(true);
        }
    }

    // Create and populate the array to return
    Array<const DatIndexEntry*>* retval = new Array<const DatIndexEntry*>(count);
    if (count) {
        uint index = 0;
        for (uint i = 0; i < ids.Count(); i++) {
            CategoryTreeItem* itemData = (CategoryTreeItem*)this->GetItemData(ids[i]);
            if (itemData->GetDataType() == CategoryTreeItem::DT_Entry) {
                (*retval)[index++] = (const DatIndexEntry*)itemData->GetData();
            } else if (itemData->GetDataType() == CategoryTreeItem::DT_Category) {
                this->AddCategoryEntriesToArray(retval, index, *(const DatIndexCategory*)itemData->GetData());
            }
        }
        wxASSERT(index == count);
    }

    return retval;
}

void CategoryTree::AddCategoryEntriesToArray(Array<const DatIndexEntry*>* pArray, uint& pIndex, const DatIndexCategory& pCategory) const
{
    // Loop through subcategories
    for (uint i = 0; i < pCategory.GetNumSubCategories(); i++) {
        this->AddCategoryEntriesToArray(pArray, pIndex, *pCategory.GetSubCategory(i));
    }

    // Loop through entries
    for (uint i = 0; i < pCategory.GetNumEntries(); i++) {
        (*pArray)[pIndex++] = pCategory.GetEntry(i);
    }
}

void CategoryTree::SetIndex(DatIndex* pIndex)
{
    if (mIndex) { mIndex->RemoveListener(this); }

    this->ClearEntries();
    mIndex = pIndex;

    if (mIndex) {
        mIndex->AddListener(this);

        for (uint i = 0; i < mIndex->GetNumEntries(); i++) {
            this->AddEntry(*mIndex->GetEntry(i));
        }
    }
}

DatIndex* CategoryTree::GetIndex()
{
    return mIndex;
}

const DatIndex* CategoryTree::GetIndex() const
{
    return mIndex;
}

void CategoryTree::AddListener(ICategoryTreeListener* pListener)
{
    mListeners.insert(pListener);
}

void CategoryTree::RemoveListener(ICategoryTreeListener* pListener)
{
    mListeners.erase(pListener);
}

int CategoryTree::GetImageForEntry(const DatIndexEntry& pEntry)
{
    switch (pEntry.GetFileType()) {
    case ANFT_ATEX:
    case ANFT_ATTX:
    case ANFT_ATEC:
    case ANFT_ATEP:
    case ANFT_ATEU:
    case ANFT_ATET:
    case ANFT_DDS:
        return CategoryTreeImageList::IT_Image;
    case ANFT_EXE:
        return CategoryTreeImageList::IT_Executable;
    case ANFT_DLL:
        return CategoryTreeImageList::IT_Dll;
    case ANFT_EULA:
    case ANFT_StringFile:
        return CategoryTreeImageList::IT_Text;
    default:
        return CategoryTreeImageList::IT_UnknownFile;
    }
}

void CategoryTree::OnItemExpanding(wxTreeEvent& pEvent)
{
    wxTreeItemId id = pEvent.GetItem();
    CategoryTreeItem* itemData = (CategoryTreeItem*)this->GetItemData(id);

    // If this is not a category, skip it
    if (itemData && itemData->GetDataType() != CategoryTreeItem::DT_Category) {
        return;
    }

    // Give it the open folder icon instead
    this->SetItemImage(id, CategoryTreeImageList::IT_OpenFolder);

    // Skip if the category isn't dirty
    if (!itemData->IsDirty()) { 
        return; 
    } else {
        this->DeleteChildren(id);
    }

    // Fetch the category info
    DatIndexCategory* category = (DatIndexCategory*)itemData->GetData();
    if (!category) { return; }

    // Add all contained entries
    for (uint i = 0; i < category->GetNumEntries(); i++) {
        DatIndexEntry* entry = category->GetEntry(i);
        if (!entry) { continue; }
        //this->AddEntry(id, *entry);
        this->AppendItem(id, entry->GetName(), this->GetImageForEntry(*entry), -1, new CategoryTreeItem(CategoryTreeItem::DT_Entry, entry));
    }
    this->SortChildren(id);

    // Add sub-categories last
    for (uint i = 0; i < category->GetNumSubCategories(); i++) {
        DatIndexCategory* subcategory = category->GetSubCategory(i);
        if (!subcategory) { continue; }
        this->EnsureHasCategory(*subcategory, true);
    }

    // Un-dirty!
    itemData->SetDirty(false);
}

void CategoryTree::OnItemCollapsing(wxTreeEvent& pEvent)
{
    wxTreeItemId id = pEvent.GetItem();
    CategoryTreeItem* itemData = (CategoryTreeItem*)this->GetItemData(id);

    // Skip if this is not a category
    if (!itemData || itemData->GetDataType() != CategoryTreeItem::DT_Category) {
        return;
    }

    // Set icon to the closed folder
    this->SetItemImage(id, CategoryTreeImageList::IT_ClosedFolder);
}

void CategoryTree::OnSelChanged(wxTreeEvent& pEvent)
{
    wxArrayTreeItemIds ids;
    this->GetSelections(ids);
    
    // Only raise events if only one entry was selected
    if (ids.Count() == 1) {
        CategoryTreeItem* itemData = (CategoryTreeItem*)this->GetItemData(ids[0]);
        if (!itemData) { return; }

        // raise the correct event
        switch (itemData->GetDataType()) {
        case CategoryTreeItem::DT_Category:
            for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
                (*it)->OnTreeCategoryClicked(*this, *(const DatIndexCategory*)itemData->GetData());
            }
            break;
        case CategoryTreeItem::DT_Entry:
            for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
                (*it)->OnTreeEntryClicked(*this, *(const DatIndexEntry*)itemData->GetData());
            }
        }
    }
}

void CategoryTree::OnContextMenu(wxTreeEvent& pEvent)
{
    wxArrayTreeItemIds ids;
    this->GetSelections(ids);

    if (ids.Count() > 0) {
        // Start with counting the total amount of entries
        uint count = 0;
        const DatIndexEntry* firstEntry = NULL;
        for (uint i = 0; i < ids.Count(); i++) {
            const CategoryTreeItem* itemData = (const CategoryTreeItem*)this->GetItemData(ids[i]);
            if (itemData->GetDataType() == CategoryTreeItem::DT_Entry) {
                if (!firstEntry) { firstEntry = (const DatIndexEntry*)itemData->GetData(); }
                count++;
            } else if (itemData->GetDataType() == CategoryTreeItem::DT_Category) {
                const DatIndexCategory* category = (const DatIndexCategory*)itemData->GetData();
                count += category->GetNumEntries(true);
                if (!firstEntry && category->GetNumEntries()) { firstEntry = category->GetEntry(0); }
            }
        }
        
        // Create the menu
        if (count > 0) {
            wxMenu newMenu;
            if (count == 1) {
                newMenu.Append(wxID_SAVE, wxString::Format(wxT("Extract file %s..."), firstEntry->GetName()));
                newMenu.Append(wxID_SAVEAS, wxString::Format(wxT("Extract file %s (raw)..."), firstEntry->GetName()));
            } else {
                newMenu.Append(wxID_SAVE, wxString::Format(wxT("Extract %d files..."), count));
                newMenu.Append(wxID_SAVEAS, wxString::Format(wxT("Extract %d files (raw)..."), count));
            }
            this->PopupMenu(&newMenu);
        }
    }
}

void CategoryTree::OnExtractRawFiles(wxCommandEvent& pEvent)
{
    for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
        (*it)->OnTreeExtractRaw(*this);
    }
}

void CategoryTree::OnExtractConvertedFiles(wxCommandEvent& pEvent)
{
    for (ListenerSet::iterator it = mListeners.begin(); it != mListeners.end(); it++) {
        (*it)->OnTreeExtractConverted(*this);
    }
}

void CategoryTree::OnIndexFileAdded(DatIndex& pIndex, const DatIndexEntry& pEntry)
{
    Ensure::NotNull(&pEntry);
    this->AddEntry(pEntry);
}

void CategoryTree::OnIndexCleared(DatIndex& pIndex)
{
    wxASSERT(&pIndex == mIndex);
    this->ClearEntries();
}

void CategoryTree::OnIndexDestruction(DatIndex& pIndex)
{
    wxASSERT(&pIndex == mIndex);
    mIndex = NULL;
    this->ClearEntries();
}

}; // namespace gw2b
