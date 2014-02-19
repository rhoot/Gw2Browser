/* \file       BrowserWindow.cpp
*  \brief      Contains definition of the browser window.
*  \author     Rhoot
*/

/*
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>
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

#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "Imported/crc.h"
#include "BrowserWindow.h"

#include "AboutBox.h"
#include "DatIndexIO.h"
#include "ExtractFilesWindow.h"
#include "FileReader.h"
#include "ProgressStatusBar.h"
#include "PreviewPanel.h"

#include "Tasks/ReadIndexTask.h"
#include "Tasks/ScanDatTask.h"
#include "Tasks/WriteIndexTask.h"

#include "InitOpenGL.h"

namespace gw2b {

	BrowserWindow::BrowserWindow( const wxString& p_title )
		: wxFrame( nullptr, wxID_ANY, p_title, wxDefaultPosition, wxSize( 820, 512 ) )
		, m_index( std::make_shared<DatIndex>( ) )
		, m_progress( nullptr )
		, m_currentTask( nullptr )
		, m_splitter( nullptr )
		, m_catTree( nullptr )
		, m_previewPanel( nullptr ) {
		// Set window icon
		SetIcon( wxICON( aaaaGW2BROWSER_ICON ) );
		auto menuBar = new wxMenuBar( );
		// File menu
		auto fileMenu = new wxMenu( );
		wxAcceleratorEntry accel( wxACCEL_CTRL, 'O' );
		fileMenu->Append( wxID_OPEN, wxT( "&Open" ), wxT( "Open a file for browsing" ) )->SetAccel( &accel );
		fileMenu->AppendSeparator( );
		fileMenu->Append( wxID_EXIT, wxT( "E&xit\tAlt+F4" ) );
		// Help menu
		auto helpMenu = new wxMenu( );
		helpMenu->Append( wxID_ABOUT, wxT( "&About Gw2Browser" ) );
		// Attach menu
		menuBar->Append( fileMenu, wxT( "&File" ) );
		menuBar->Append( helpMenu, wxT( "&Help" ) );
		this->SetMenuBar( menuBar );

		// Setup statusbar
		m_progress = new ProgressStatusBar( this );
		this->SetStatusBar( m_progress );

		// Splitter
		m_splitter = new wxSplitterWindow( this );

		// Category tree
		m_catTree = new CategoryTree( m_splitter );
		m_catTree->setDatIndex( m_index );
		m_catTree->addListener( this );

		// Preview panel
		m_previewPanel = new PreviewPanel( m_splitter );
		m_previewPanel->Hide( );

		// Initialize splitter
		m_splitter->Initialize( m_catTree );

		// Initialize OpenGL
		InitOpenGL( );

		// Hook up events
		this->Connect( wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserWindow::onOpenEvt ) );
		this->Connect( wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserWindow::onExitEvt ) );
		this->Connect( wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserWindow::onAboutEvt ) );
		this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BrowserWindow::onCloseEvt ) );
	}

	//============================================================================/

	BrowserWindow::~BrowserWindow( ) {
		deletePointer( m_currentTask );
		glewDestroyContext( );
	}

	//============================================================================/

	bool BrowserWindow::performTask( Task* p_task ) {
		Ensure::notNull( p_task );

		// Already have a task running?
		if ( m_currentTask ) {
			if ( m_currentTask->canAbort( ) ) {
				m_currentTask->abort( );
				deletePointer( m_currentTask );
				this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( BrowserWindow::onPerformTaskEvt ) );
				m_progress->hideProgressBar( );
			} else {
				deletePointer( p_task );
				return false;
			}
		}

		// Initialize succeeded?
		m_currentTask = p_task;
		if ( !m_currentTask->init( ) ) {
			deletePointer( m_currentTask );
			return false;
		}

		this->Connect( wxEVT_IDLE, wxIdleEventHandler( BrowserWindow::onPerformTaskEvt ) );
		m_progress->setMaxValue( m_currentTask->maxProgress( ) );
		m_progress->showProgressBar( );
		return true;
	}

	//============================================================================/

	void BrowserWindow::openFile( const wxString& p_path ) {
		// Try to open the file
		if ( !m_datFile.open( p_path ) ) {
			wxMessageBox( wxString::Format( wxT( "Failed to open file: %s" ), p_path ),
				wxMessageBoxCaptionStr, wxOK | wxCENTER | wxICON_ERROR );
			return;
		}
		m_datPath = p_path;

		// Open the index file
		uint64 datTimeStamp = wxFileModificationTime( p_path );
		auto indexFile = this->findDatIndex( );
		auto readIndexTask = new ReadIndexTask( m_index, indexFile.GetFullPath( ), datTimeStamp );

		// Start reading the index
		readIndexTask->addOnCompleteHandler( [this] ( ) { this->onReadIndexComplete( ); } );
		if ( !this->performTask( readIndexTask ) ) {
			this->reIndexDat( );
		}
	}

	//============================================================================/

	void BrowserWindow::viewEntry( const DatIndexEntry& p_entry ) {
		if ( m_previewPanel->previewFile( m_datFile, p_entry ) ) {
			m_previewPanel->Show( );
			// Split it!
			m_splitter->SetMinimumPaneSize( 100 );
			m_splitter->SplitVertically( m_catTree, m_previewPanel, m_splitter->GetClientSize( ).x / 4 );
		}
	}

	//============================================================================/

	wxFileName BrowserWindow::findDatIndex( ) {

		wxStandardPathsBase& stdp = wxStandardPaths::Get( );
		auto configPath = stdp.GetDataDir( );

		auto datPathCrc = ::compute_crc( INITIAL_CRC, m_datPath.char_str( ), m_datPath.Length( ) );
		auto indexFileName = wxString::Format( wxT( "%x.idx" ), ( uint ) datPathCrc );

		return wxFileName( configPath, indexFileName );
	}

	//============================================================================/

	void BrowserWindow::reIndexDat( ) {
		m_index->clear( );
		m_index->setDatTimestamp( wxFileModificationTime( m_datPath ) );
		this->indexDat( );
	}

	//============================================================================/

	void BrowserWindow::indexDat( ) {
		auto scanTask = new ScanDatTask( m_index, m_datFile );
		scanTask->addOnCompleteHandler( [this] ( ) { this->onScanTaskComplete( ); } );
		this->performTask( scanTask );
	}

	//============================================================================/

	void BrowserWindow::onOpenEvt( wxCommandEvent& WXUNUSED( p_event ) ) {
		wxFileDialog dialog( this, wxFileSelectorPromptStr, wxT( "" ), wxT( "Gw2.dat" ),
			wxT( "Guild Wars 2 DAT|*.dat" ), wxFD_OPEN | wxFD_FILE_MUST_EXIST );

		if ( dialog.ShowModal( ) == wxID_OK ) {
			this->openFile( dialog.GetPath( ) );
		}
	}

	//============================================================================/

	void BrowserWindow::onExitEvt( wxCommandEvent& WXUNUSED( p_event ) ) {
		this->Close( true );
	}

	//============================================================================/

	void BrowserWindow::onAboutEvt( wxCommandEvent& WXUNUSED( p_event ) ) {
		AboutBox about( this );
		about.ShowModal( );
	}

	//============================================================================/

	void BrowserWindow::onCloseEvt( wxCloseEvent& p_event ) {
		// Drop out if we can't cancel the window closing
		if ( !p_event.CanVeto( ) ) {
			p_event.Skip( );
			return;
		}

		// Cancel current task if possible.
		if ( m_currentTask ) {
			if ( m_currentTask->canAbort( ) ) {
				m_currentTask->abort( );
				deletePointer( m_currentTask );
				this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( BrowserWindow::onPerformTaskEvt ) );
			} else {
				this->Disable( );
				m_currentTask->addOnCompleteHandler( [this] ( ) { this->tryClose( ); } );
				p_event.Veto( );
				return;
			}
		}

		// Add a write task if the index is dirty
		if ( !m_currentTask && m_index->isDirty( ) ) {
			auto indexPath = this->findDatIndex( );
			if ( !indexPath.DirExists( ) ) {
				indexPath.Mkdir( 511, wxPATH_MKDIR_FULL );
			}

			auto writeTask = new WriteIndexTask( m_index, indexPath.GetFullPath( ) );
			writeTask->addOnCompleteHandler( [this] ( ) { this->onWriteTaskCloseCompleted( ); } );
			if ( this->performTask( writeTask ) ) {
				this->Disable( );
				p_event.Veto( );
				return;
			}
		}

		p_event.Skip( );
	}

	//============================================================================/

	void BrowserWindow::onPerformTaskEvt( wxIdleEvent& p_event ) {
		Ensure::notNull( m_currentTask );
		m_currentTask->perform( );

		if ( !m_currentTask->isDone( ) ) {
			m_progress->update( m_currentTask->currentProgress( ), m_currentTask->text( ) );
			p_event.RequestMore( );
		} else {
			this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( BrowserWindow::onPerformTaskEvt ) );
			m_progress->SetStatusText( wxEmptyString );
			m_progress->hideProgressBar( );

			auto oldTask = m_currentTask;
			m_currentTask = nullptr;
			oldTask->invokeOnCompleteHandler( );
			deletePointer( oldTask );
		}
	}

	//============================================================================/

	void BrowserWindow::onReadIndexComplete( ) {
		// If it failed, it was cleared.
		if ( m_index->datTimestamp( ) == 0 || m_index->numEntries( ) == 0 ) {
			this->reIndexDat( );
			return;
		}

		// Was it complete?
		auto isComplete = ( m_index->highestMftEntry( ) == m_datFile.numFiles( ) );
		if ( !isComplete ) {
			this->indexDat( );
		}
	}

	//============================================================================/

	void BrowserWindow::onScanTaskComplete( ) {
		auto writeTask = new WriteIndexTask( m_index, this->findDatIndex( ).GetFullPath( ) );
		this->performTask( writeTask );
	}

	//============================================================================/

	void BrowserWindow::onWriteTaskCloseCompleted( ) {
		// Forcing this here causes the OnCloseEvt to not try to write the index
		// again. In case it failed the first time, it's likely to fail again and
		// we don't want to get stuck in an infinite loop.
		this->Close( true );
	}

	//============================================================================/

	void BrowserWindow::tryClose( ) {
		this->Close( false );
	}

	//============================================================================/

	void BrowserWindow::onTreeEntryClicked( CategoryTree& p_tree, const DatIndexEntry& p_entry ) {
		this->viewEntry( p_entry );
	}

	//============================================================================/

	void BrowserWindow::onTreeCategoryClicked( CategoryTree& p_tree, const DatIndexCategory& p_category ) {
		// TODO
	}

	//============================================================================/

	void BrowserWindow::onTreeCleared( CategoryTree& p_tree ) {
		// TODO
	}

	//============================================================================/

	void BrowserWindow::onTreeExtractRaw( CategoryTree& p_tree ) {
		auto entries = p_tree.getSelectedEntries( );

		if ( entries.GetSize( ) ) {
			// If it's just one file, we could handle it here
			if ( entries.GetSize( ) == 1 ) {
				auto entry = entries[0];
				auto entryData = m_datFile.readFile( entry->mftEntry( ) );

				// Valid data?
				if ( !entryData.GetSize( ) ) {
					wxMessageBox( wxT( "Failed to extract the file, most likely due to a decompression error." ), wxT( "Error" ), wxOK | wxICON_ERROR );
					return;
				}

				// Identify files extensions
				auto fileType = ANFT_Unknown;
				m_datFile.identifyFileType( entryData.GetPointer(), entryData.GetSize(), fileType );
				auto reader = FileReader::readerForFileType( entryData, fileType );

				auto ext = wxEmptyString;
				if ( reader ) {
					ext = reader->extension();
				}

				// Ask for location
				wxFileDialog dialog( this,
					wxString::Format( wxT( "Extract %s%s..." ), entry->name(), ext ),
					wxEmptyString,
					entry->name() + ext,
					wxFileSelectorDefaultWildcardStr,
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

				if ( dialog.ShowModal( ) == wxID_OK ) {
					wxFile file( dialog.GetPath( ), wxFile::write );
					if ( file.IsOpened( ) ) {
						file.Write( entryData.GetPointer( ), entryData.GetSize( ) );
						file.Close( );
					} else {
						wxMessageBox( wxT( "Failed to open the file for writing." ), wxT( "Error" ), wxOK | wxICON_ERROR );
					}
				}
			}

			// More files than one
			else {
				wxDirDialog dialog( this, wxT( "Select output folder" ) );
				if ( dialog.ShowModal( ) == wxID_OK ) {
					new ExtractFilesWindow( entries, m_datFile, dialog.GetPath( ), ExtractFilesWindow::EM_Raw );
				}
			}
		}
	}

	//============================================================================/

	void BrowserWindow::onTreeExtractConverted( CategoryTree& p_tree ) {
		auto entries = p_tree.getSelectedEntries( );

		if ( entries.GetSize( ) ) {
			// If it's just one file, we could handle it here
			if ( entries.GetSize( ) == 1 ) {
				auto entry = entries[0];
				auto entryData = m_datFile.readFile( entry->mftEntry( ) );

				// Valid data?
				if ( !entryData.GetSize( ) ) {
					wxMessageBox( wxT( "Failed to extract the file, most likely due to a decompression error." ), wxT( "Error" ), wxOK | wxICON_ERROR );
					return;
				}
				// Convert to a usable format
				auto fileType = ANFT_Unknown;
				m_datFile.identifyFileType( entryData.GetPointer( ), entryData.GetSize( ), fileType );
				auto reader = FileReader::readerForData( entryData, fileType );

				auto ext = wxEmptyString;
				if ( reader ) {
					entryData = reader->convertData( );
					ext = reader->extension( );
				}

				// Ask for location
				wxFileDialog dialog( this,
					wxString::Format( wxT( "Extract %s%s..." ), entry->name( ), ext ),
					wxEmptyString,
					entry->name( ) + ext,
					wxFileSelectorDefaultWildcardStr,
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

				if ( dialog.ShowModal( ) == wxID_OK ) {
					wxFile file( dialog.GetPath( ), wxFile::write );
					if ( file.IsOpened( ) ) {
						file.Write( entryData.GetPointer( ), entryData.GetSize( ) );
						file.Close( );
					} else {
						wxMessageBox( wxT( "Failed to open the file for writing." ), wxT( "Error" ), wxOK | wxICON_ERROR );
					}
				}

				deletePointer( reader );
			}

			// More files than one
			else {
				wxDirDialog dialog( this, wxT( "Select output folder" ) );
				if ( dialog.ShowModal( ) == wxID_OK ) {
					new ExtractFilesWindow( entries, m_datFile, dialog.GetPath( ), ExtractFilesWindow::EM_Converted );
				}
			}
		}
	}

}; // namespace gw2b
