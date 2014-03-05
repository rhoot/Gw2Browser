/* \file       CategoryTree.h
*  \brief      Contains definition of the .dat structure tree control.
*  \author     Rhoot
*/

/*
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>
Copyright (C) 2013 Till034 <https://github.com/Till034>
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

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

namespace gw2b {

	//----------------------------------------------------------------------------
	//      CategoryTreeImageList
	//----------------------------------------------------------------------------

	CategoryTreeImageList::CategoryTreeImageList( )
		: wxImageList( 16, 16, true, 2 ) {
		this->Add( data::loadPNG( data::open_folder_png, data::open_folder_png_size ) );
		this->Add( data::loadPNG( data::unknown_png, data::unknown_png_size ) );
		this->Add( data::loadPNG( data::closed_folder_png, data::closed_folder_png_size ) );
		this->Add( data::loadPNG( data::exe_png, data::exe_png_size ) );
		this->Add( data::loadPNG( data::dll_png, data::dll_png_size ) );
		this->Add( data::loadPNG( data::image_png, data::image_png_size ) );
		this->Add( data::loadPNG( data::text_png, data::text_png_size ) );
		this->Add( data::loadPNG( data::sound_png, data::sound_png_size ) );
		this->Add( data::loadPNG( data::font_png, data::font_png_size ) );
		this->Add( data::loadPNG( data::video_png, data::video_png_size ) );
	}

	//============================================================================/

	CategoryTreeImageList::~CategoryTreeImageList( ) {
	}

	//----------------------------------------------------------------------------
	//      CategoryTree
	//----------------------------------------------------------------------------

	wxIMPLEMENT_DYNAMIC_CLASS( CategoryTree, wxTreeCtrl );

	CategoryTree::CategoryTree() {
	}

	CategoryTree::CategoryTree( wxWindow* p_parent, const wxPoint& p_location, const wxSize& p_size)
		: wxTreeCtrl( p_parent, wxID_ANY, p_location, p_size, wxTR_HIDE_ROOT | wxTR_TWIST_BUTTONS | wxTR_DEFAULT_STYLE | wxTR_MULTIPLE ) {
		// Initialize tree
		auto images = new CategoryTreeImageList( );
		this->AssignImageList( images );
		this->AddRoot( wxT( "Root" ) );

		// Hookup events
		this->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( CategoryTree::onItemExpanding ) );
		this->Connect( wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler( CategoryTree::onItemCollapsing ) );
		this->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CategoryTree::onSelChanged ) );
		this->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( CategoryTree::onContextMenu ) );
		this->Connect( wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CategoryTree::onExtractConvertedFiles ) );
		this->Connect( wxID_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CategoryTree::onExtractRawFiles ) );
	}

	//============================================================================/

	CategoryTree::~CategoryTree( ) {
		if ( m_index ) {
			m_index->removeListener( this );
		}

		for ( auto it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
			( *it )->onTreeDestruction( *this );
		}
	}

	//============================================================================/

	int CategoryTree::OnCompareItems( const wxTreeItemId& p_item1, const wxTreeItemId& p_item2 ) {
		int Result = 0;
		auto itemData1 = static_cast<const CategoryTreeItem*>( this->GetItemData( p_item1 ) );
		auto itemData2 = static_cast<const CategoryTreeItem*>( this->GetItemData( p_item2 ) );

		if ( itemData1->dataType() == CategoryTreeItem::DT_Entry && itemData2->dataType() == CategoryTreeItem::DT_Entry ) {
			auto itemEntry1 = static_cast<const DatIndexEntry*>( itemData1->data() );
			auto itemEntry2 = static_cast<const DatIndexEntry*>( itemData2->data() );

			if ( itemEntry1->baseId() > itemEntry2->baseId() ) {
				Result = 1;
			} else if ( itemEntry1->baseId() < itemEntry2->baseId() ) {
				Result = -1;
			}
		}
		return Result;
	}

	//============================================================================/

	void CategoryTree::addEntry( const DatIndexEntry& p_entry ) {
		auto category = this->ensureHasCategory( *p_entry.category( ) );
		if ( category.IsOk( ) ) {
			if ( this->IsExpanded( category ) ) {
				auto node = this->addEntry( category, p_entry );
				this->SetItemData( node, new CategoryTreeItem( CategoryTreeItem::DT_Entry, &p_entry ) );
			} else {
				auto itemData = static_cast<CategoryTreeItem*>( this->GetItemData( category ) );
				if ( itemData->dataType( ) != CategoryTreeItem::DT_Category ) {
					return;
				}
				itemData->setDirty( true );
			}
		}
	}

	//============================================================================/

	wxTreeItemId CategoryTree::ensureHasCategory( const DatIndexCategory& p_category, bool p_force ) {
		wxTreeItemId parent;
		auto parentIsRoot = false;

		// Determine parent node
		if ( p_category.parent( ) ) {
			parent = this->ensureHasCategory( *p_category.parent( ) );
		} else {
			parent = this->GetRootItem( );
			parentIsRoot = true;
		}

		if ( !p_force ) {
			// If parent is invalid, it means the tree isn't expanded and we shouldn't add this
			if ( !parentIsRoot && ( !parent.IsOk( ) || !this->IsExpanded( parent ) ) ) {
				return wxTreeItemId( );
			}
		}

		wxTreeItemIdValue cookie;
		auto child = this->GetFirstChild( parent, cookie );

		// Scan for existing node
		while ( child.IsOk( ) ) {
			auto data = static_cast<const CategoryTreeItem*>( this->GetItemData( child ) );
			if ( data->dataType( ) != CategoryTreeItem::DT_Category ) {
				break;
			}
			if ( data->data( ) == &p_category ) {
				return child;
			}
			child = this->GetNextChild( parent, cookie );
		}

		// Node does not exist, add it
		auto thisNode = this->addCategoryEntry( parent, p_category.name( ) );
		auto itemData = new CategoryTreeItem( CategoryTreeItem::DT_Category, &p_category );
		itemData->setDirty( true );
		this->SetItemData( thisNode, itemData );
		// All category nodes have children
		this->SetItemHasChildren( thisNode );
		return thisNode;
	}

	//============================================================================/

	wxTreeItemId CategoryTree::addEntry( const wxTreeItemId& p_parent, const DatIndexEntry& p_entry ) {
		if ( p_entry.name( ).IsNumber( ) ) {
			ulong number;
			p_entry.name( ).ToULong( &number );
			return this->addNumberEntry( p_parent, p_entry, number );
		}

		// NaN :)
		return this->addTextEntry( p_parent, p_entry );
	}

	//============================================================================/

	wxTreeItemId CategoryTree::addNumberEntry( const wxTreeItemId& p_parent, const DatIndexEntry& p_entry, uint p_name ) {
		wxTreeItemIdValue cookie;
		auto child = this->GetFirstChild( p_parent, cookie );
		wxTreeItemId previous;

		while ( child.IsOk( ) ) {
			wxString text = this->GetItemText( child );
			if ( !text.IsNumber( ) ) {
				break;
			}
			// Convert to uint
			ulong number;
			text.ToULong( &number );
			// Compare
			if ( number > p_name ) {
				break;
			}
			// Move to next
			previous = child;
			child = this->GetNextChild( p_parent, cookie );
		}

		// This item should be first if there *is* something in this list, but previous is nothing
		if ( child.IsOk( ) && !previous.IsOk( ) ) {
			return this->InsertItem( p_parent, 0, p_entry.name( ), this->getImageForEntry( p_entry ) );
		}
		// This item should be squashed in if both child and previous are ok
		if ( child.IsOk( ) && previous.IsOk( ) ) {
			return this->InsertItem( p_parent, previous, p_entry.name( ), this->getImageForEntry( p_entry ) );
		}
		// If the above fails, it means we went through the entire list without finding a proper spot
		return this->AppendItem( p_parent, p_entry.name( ), this->getImageForEntry( p_entry ) );
	}

	//============================================================================/

	wxTreeItemId CategoryTree::addTextEntry( const wxTreeItemId& p_parent, const DatIndexEntry& p_entry ) {
		wxTreeItemIdValue cookie;
		wxTreeItemId previous;
		auto child = this->GetFirstChild( p_parent, cookie );

		while ( child.IsOk( ) ) {
			auto text = this->GetItemText( child );
			// Compare
			if ( text > p_entry.name( ) ) {
				break;
			}
			// Move to next
			previous = child;
			child = this->GetNextChild( p_parent, cookie );
		}

		// This item should be first if there *is* something in this list, but previous is nothing
		if ( child.IsOk( ) && !previous.IsOk( ) ) {
			return this->InsertItem( p_parent, 0, p_entry.name( ), this->getImageForEntry( p_entry ) );
		}
		// This item should be squashed in if both child and previous are ok
		if ( child.IsOk( ) && previous.IsOk( ) ) {
			return this->InsertItem( p_parent, previous, p_entry.name( ), this->getImageForEntry( p_entry ) );
		}
		// If the above fails, it means we went through the entire list without finding a proper spot
		return this->AppendItem( p_parent, p_entry.name( ), this->getImageForEntry( p_entry ) );
	}

	//============================================================================/

	wxTreeItemId CategoryTree::addCategoryEntry( const wxTreeItemId& p_parent, const wxString& p_displayName ) {
		wxTreeItemIdValue cookie;
		wxTreeItemId previous;
		auto child = this->GetFirstChild( p_parent, cookie );

		while ( child.IsOk( ) ) {
			auto text = this->GetItemText( child );
			// Compare
			if ( text > p_displayName ) {
				break;
			}
			// Categories have nullptr item data
			auto data = static_cast<const CategoryTreeItem*>( this->GetItemData( child ) );
			if ( data->dataType( ) != CategoryTreeItem::DT_Category ) {
				break;
			}
			// Move to next
			previous = child;
			child = this->GetNextChild( p_parent, cookie );
		}

		// This item should be first if there *is* something in this list, but previous is nothing
		if ( child.IsOk( ) && !previous.IsOk( ) ) {
			return this->InsertItem( p_parent, 0, p_displayName, CategoryTreeImageList::IT_ClosedFolder );
		}
		// This item should be squashed in if both child and previous are ok
		if ( child.IsOk( ) && previous.IsOk( ) ) {
			return this->InsertItem( p_parent, previous, p_displayName, CategoryTreeImageList::IT_ClosedFolder );
		}
		// If the above fails, it means we went through the entire list without finding a proper spot
		return this->AppendItem( p_parent, p_displayName, CategoryTreeImageList::IT_ClosedFolder );
	}

	//============================================================================/

	void CategoryTree::clearEntries( ) {
		this->DeleteAllItems( );
		this->AddRoot( wxT( "Root" ) );

		for ( ListenerSet::iterator it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
			( *it )->onTreeCleared( *this );
		}
	}

	//============================================================================/

	Array<const DatIndexEntry*> CategoryTree::getSelectedEntries( ) const {
		wxArrayTreeItemIds ids;
		this->GetSelections( ids );
		if ( ids.GetCount( ) == 0 ) {
			return Array<const DatIndexEntry*>( );
		}

		// Doing this in two steps since reallocating takes far longer than iterating

		// Start with counting the total amount of entries
		uint count = 0;
		for ( uint i = 0; i < ids.Count( ); i++ ) {
			auto itemData = static_cast<const CategoryTreeItem*>( this->GetItemData( ids[i] ) );
			if ( itemData->dataType( ) == CategoryTreeItem::DT_Entry ) {
				count++;
			} else if ( itemData->dataType( ) == CategoryTreeItem::DT_Category ) {
				count += static_cast<const DatIndexCategory*>( itemData->data( ) )->numEntries( true );
			}
		}

		// Create and populate the array to return
		Array<const DatIndexEntry*> retval( count );
		if ( count ) {
			uint index = 0;
			for ( uint i = 0; i < ids.Count( ); i++ ) {
				auto itemData = static_cast<const CategoryTreeItem*>( this->GetItemData( ids[i] ) );
				if ( itemData->dataType( ) == CategoryTreeItem::DT_Entry ) {
					retval[index++] = static_cast<const DatIndexEntry*>( itemData->data( ) );
				} else if ( itemData->dataType( ) == CategoryTreeItem::DT_Category ) {
					this->addCategoryEntriesToArray( retval, index, *static_cast<const DatIndexCategory*>( itemData->data( ) ) );
				}
			}
			Assert( index == count );
		}

		return retval;
	}

	//============================================================================/

	void CategoryTree::addCategoryEntriesToArray( Array<const DatIndexEntry*>& p_array, uint& p_index, const DatIndexCategory& p_category ) const {
		// Loop through subcategories
		for ( uint i = 0; i < p_category.numSubCategories( ); i++ ) {
			this->addCategoryEntriesToArray( p_array, p_index, *p_category.subCategory( i ) );
		}

		// Loop through entries
		for ( uint i = 0; i < p_category.numEntries( ); i++ ) {
			p_array[p_index++] = p_category.entry( i );
		}
	}

	//============================================================================/

	void CategoryTree::setDatIndex( const std::shared_ptr<DatIndex>& p_index ) {
		if ( m_index ) {
			m_index->removeListener( this );
		}

		this->clearEntries( );
		m_index = p_index;

		if ( m_index ) {
			m_index->addListener( this );

			for ( uint i = 0; i < m_index->numEntries( ); i++ ) {
				this->addEntry( *m_index->entry( i ) );
			}
		}
	}

	//============================================================================/

	std::shared_ptr<DatIndex> CategoryTree::datIndex( ) const {
		return m_index;
	}

	//============================================================================/

	void CategoryTree::addListener( ICategoryTreeListener* p_listener ) {
		m_listeners.insert( p_listener );
	}

	//============================================================================/

	void CategoryTree::removeListener( ICategoryTreeListener* p_listener ) {
		m_listeners.erase( p_listener );
	}

	//============================================================================/

	int CategoryTree::getImageForEntry( const DatIndexEntry& p_entry ) {
		switch ( p_entry.fileType( ) ) {
		case ANFT_ATEX:
		case ANFT_ATTX:
		case ANFT_ATEC:
		case ANFT_ATEP:
		case ANFT_ATEU:
		case ANFT_ATET:
		case ANFT_DDS:
		case ANFT_JPEG:
		case ANFT_WEBP:
			return CategoryTreeImageList::IT_Image;
		case ANFT_EXE:
			return CategoryTreeImageList::IT_Executable;
		case ANFT_DLL:
			return CategoryTreeImageList::IT_Dll;
		case ANFT_EULA:
		case ANFT_StringFile:
			return CategoryTreeImageList::IT_Text;
		case ANFT_Bank:
		case ANFT_Sound:
		case ANFT_ID3:
			return CategoryTreeImageList::IT_Sound;
		case ANFT_FontFile:
			return CategoryTreeImageList::IT_Font;
		case ANFT_Bink2Video:
			return CategoryTreeImageList::IT_Video;
		default:
			return CategoryTreeImageList::IT_UnknownFile;
		}
	}

	//============================================================================/

	void CategoryTree::onItemExpanding( wxTreeEvent& p_event ) {
		auto id = p_event.GetItem( );
		auto itemData = static_cast<CategoryTreeItem*>( this->GetItemData( id ) );

		// If this is not a category, skip it
		if ( itemData && itemData->dataType( ) != CategoryTreeItem::DT_Category ) {
			return;
		}

		// Give it the open folder icon instead
		this->SetItemImage( id, CategoryTreeImageList::IT_OpenFolder );

		// Skip if the category isn't dirty
		if ( !itemData->isDirty( ) ) {
			return;
		} else {
			this->DeleteChildren( id );
		}

		// Fetch the category info
		auto category = static_cast<const DatIndexCategory*>( itemData->data( ) );
		if ( !category ) {
			return;
		}

		// Add all contained entries
		for ( uint i = 0; i < category->numEntries( ); i++ ) {
			auto entry = category->entry( i );
			if ( !entry ) {
				continue;
			}
			//this->AddEntry(id, *entry);
			this->AppendItem( id, entry->name( ), this->getImageForEntry( *entry ), -1, new CategoryTreeItem( CategoryTreeItem::DT_Entry, entry ) );
		}
		this->SortChildren( id );

		// Add sub-categories last
		for ( uint i = 0; i < category->numSubCategories( ); i++ ) {
			auto subcategory = category->subCategory( i );
			if ( !subcategory ) {
				continue;
			}
			this->ensureHasCategory( *subcategory, true );
		}

		// Un-dirty!
		itemData->setDirty( false );
	}

	//============================================================================/

	void CategoryTree::onItemCollapsing( wxTreeEvent& p_event ) {
		auto id = p_event.GetItem( );
		auto* itemData = static_cast<const CategoryTreeItem*>( this->GetItemData( id ) );

		// Skip if this is not a category
		if ( !itemData || itemData->dataType( ) != CategoryTreeItem::DT_Category ) {
			return;
		}

		// Set icon to the closed folder
		this->SetItemImage( id, CategoryTreeImageList::IT_ClosedFolder );
	}

	//============================================================================/

	void CategoryTree::onSelChanged( wxTreeEvent& pEvent ) {
		wxArrayTreeItemIds ids;
		this->GetSelections( ids );

		// Only raise events if only one entry was selected
		if ( ids.Count( ) == 1 ) {
			auto itemData = static_cast<const CategoryTreeItem*>( this->GetItemData( ids[0] ) );
			if ( !itemData ) {
				return;
			}

			// raise the correct event
			switch ( itemData->dataType( ) ) {
			case CategoryTreeItem::DT_Category:
				for ( ListenerSet::iterator it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
					( *it )->onTreeCategoryClicked( *this, *static_cast<const DatIndexCategory*>( itemData->data( ) ) );
				}
				break;
			case CategoryTreeItem::DT_Entry:
				for ( ListenerSet::iterator it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
					( *it )->onTreeEntryClicked( *this, *static_cast<const DatIndexEntry*>( itemData->data( ) ) );
				}
			}
		}
	}

	//============================================================================/

	void CategoryTree::onContextMenu( wxTreeEvent& p_event ) {
		wxArrayTreeItemIds ids;
		this->GetSelections( ids );

		if ( ids.Count( ) > 0 ) {
			// Start with counting the total amount of entries
			uint count = 0;
			const DatIndexEntry* firstEntry = nullptr;
			for ( uint i = 0; i < ids.Count( ); i++ ) {
				auto itemData = static_cast<const CategoryTreeItem*>( this->GetItemData( ids[i] ) );
				if ( itemData->dataType( ) == CategoryTreeItem::DT_Entry ) {
					if ( !firstEntry ) {
						firstEntry = ( const DatIndexEntry* ) itemData->data( );
					}
					count++;
				} else if ( itemData->dataType( ) == CategoryTreeItem::DT_Category ) {
					auto category = static_cast<const DatIndexCategory*>( itemData->data( ) );
					count += category->numEntries( true );
					if ( !firstEntry && category->numEntries( ) ) {
						firstEntry = category->entry( 0 );
					}
				}
			}

			// Create the menu
			if ( count > 0 ) {
				wxMenu newMenu;
				if ( count == 1 ) {
					newMenu.Append( wxID_SAVE, wxString::Format( wxT( "Extract file %s..." ), firstEntry->name( ) ) );
					newMenu.Append( wxID_SAVEAS, wxString::Format( wxT( "Extract file %s (raw)..." ), firstEntry->name( ) ) );
				} else {
					newMenu.Append( wxID_SAVE, wxString::Format( wxT( "Extract %d files..." ), count ) );
					newMenu.Append( wxID_SAVEAS, wxString::Format( wxT( "Extract %d files (raw)..." ), count ) );
				}
				this->PopupMenu( &newMenu );
			}
		}
	}

	//============================================================================/

	void CategoryTree::onExtractRawFiles( wxCommandEvent& p_event ) {
		for ( ListenerSet::iterator it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
			( *it )->onTreeExtractRaw( *this );
		}
	}

	//============================================================================/

	void CategoryTree::onExtractConvertedFiles( wxCommandEvent& p_event ) {
		for ( ListenerSet::iterator it = m_listeners.begin( ); it != m_listeners.end( ); it++ ) {
			( *it )->onTreeExtractConverted( *this );
		}
	}

	//============================================================================/

	void CategoryTree::onIndexFileAdded( DatIndex& p_index, const DatIndexEntry& p_entry ) {
		Ensure::notNull( &p_entry );
		this->addEntry( p_entry );
	}

	//============================================================================/

	void CategoryTree::onIndexCleared( DatIndex& p_index ) {
		Assert( &p_index == m_index.get( ) );
		this->clearEntries( );
	}

	//============================================================================/

	void CategoryTree::onIndexDestruction( DatIndex& p_index ) {
		Assert( &p_index == m_index.get( ) );
		m_index = nullptr;
		this->clearEntries( );
	}

}; // namespace gw2b
