/* \file       Viewers/ModelViewer.cpp
*  \brief      Contains the definition of the model viewer class.
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

#include "ModelViewer.h"

#include "Readers/ImageReader.h"
#include "DatFile.h"
#include "Data.h"

namespace gw2b {

	ModelViewer::ModelViewer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
		: Viewer( p_parent, p_pos, p_size ) {

		// Load shader
		/*
		{
			 wxMessageBox( wxString( "GLEW version " ) + wxString( glewGetString( GLEW_VERSION ) + wxString( "\n" ) +
				 wxString( "OpenGL version " ) + wxString( glGetString( GL_VERSION ) ) + wxString( "\n" ) +
				 wxString( "Running on a " ) + wxString( glGetString( GL_RENDERER ) ) + wxString( " " ) + wxString( glGetString( GL_VENDOR ) ) + wxString( "\n" )
				 ), _( "" ), wxOK );
		 }
		 */
		// Hook up events
		this->Connect( wxEVT_PAINT, wxPaintEventHandler( ModelViewer::onPaintEvt ) );
		this->Connect( wxEVT_MOTION, wxMouseEventHandler( ModelViewer::onMotionEvt ) );
		this->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( ModelViewer::onMouseWheelEvt ) );
		this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ModelViewer::onKeyDownEvt ) );
	}

	ModelViewer::~ModelViewer( ) {
		/*
		for ( uint i = 0; i < m_meshCache.GetSize( ); i++ ) {
			if ( m_meshCache[i].indexBuffer ) {
				m_meshCache[i].indexBuffer->Release( );
			}
			if ( m_meshCache[i].vertexBuffer ) {
				m_meshCache[i].vertexBuffer->Release( );
			}
		}
		for ( uint i = 0; i < m_textureCache.GetSize( ); i++ ) {
			if ( m_textureCache[i].diffuseMap ) {
				m_textureCache[i].diffuseMap->Release( );
			}
		}
		*/
	}

	void ModelViewer::clear( ) {
		/*
		for ( uint i = 0; i < m_meshCache.GetSize( ); i++ ) {
			if ( m_meshCache[i].indexBuffer ) {
				m_meshCache[i].indexBuffer->Release( );
			}
			if ( m_meshCache[i].vertexBuffer ) {
				m_meshCache[i].vertexBuffer->Release( );
			}
		}
		for ( uint i = 0; i < m_textureCache.GetSize( ); i++ ) {
			if ( m_textureCache[i].diffuseMap ) {
				m_textureCache[i].diffuseMap->Release( );
			}
		}
		m_textureCache.Clear( );
		m_meshCache.Clear( );
		m_model = Model( );
		*/
		Viewer::clear( );
	}

	void ModelViewer::setReader( FileReader* p_reader ) {
		Ensure::isOfType<ModelReader>( p_reader );
		Viewer::setReader( p_reader );

		// Load model
		auto reader = this->modelReader( );
		/*
		m_model = reader->getModel( );

		// Create DX mesh cache
		m_meshCache.SetSize( m_model.numMeshes( ) );

		// Load meshes
		for ( uint i = 0; i < m_model.numMeshes( ); i++ ) {
			auto& mesh = m_model.mesh( i );
			auto& cache = m_meshCache[i];

			// Create and populate the buffers
			uint vertexCount = mesh.vertices.GetSize( );
			uint vertexSize = sizeof( Vertex );
			uint indexCount = mesh.triangles.GetSize( ) * 3;
			uint indexSize = sizeof( uint16 );

			if ( !this->createBuffers( cache, vertexCount, vertexSize, indexCount, indexSize ) ) {
				continue;
			}
			if ( !this->populateBuffers( mesh, cache ) ) {
				releasePointer( cache.indexBuffer );
				releasePointer( cache.vertexBuffer );
				continue;
			}
		}

		// Create DX texture cache
		m_textureCache.SetSize( m_model.numMaterialData( ) );

		// Load textures
		for ( uint i = 0; i < m_model.numMaterialData( ); i++ ) {
			auto& material = m_model.materialData( i );
			auto& cache = m_textureCache[i];

			// Load diffuse texture
			if ( material.diffuseMap ) {
				cache.diffuseMap = this->loadTexture( material.diffuseMap );
			} else {
				cache.diffuseMap = nullptr;
			}
		}
		*/
		// Re-focus and re-render
		this->focus( );
		this->render( );
	}
	/*
	void ModelViewer::beginFrame( uint32 p_clearColor ) {
		wxSize clientSize = this->GetClientSize( );

		D3DVIEWPORT9 viewport;
		::memset( &viewport, 0, sizeof( viewport ) );
		viewport.Width = clientSize.x;
		viewport.Height = clientSize.y;
		viewport.MaxZ = 1;
		viewport.MinZ = 0;

		m_device->SetViewport( &viewport );
		m_device->BeginScene( );
		m_device->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, p_clearColor, 1, 0 );
	}

	void ModelViewer::endFrame( ) {
		D3DVIEWPORT9 viewport;
		m_device->GetViewport( &viewport );

		RECT sourceRect;
		::memset( &sourceRect, 0, sizeof( sourceRect ) );
		sourceRect.right = viewport.Width;
		sourceRect.bottom = viewport.Height;

		m_device->EndScene( );
		m_device->Present( &sourceRect, nullptr, nullptr, nullptr );
	}
	*/
	void ModelViewer::render( ) {
		/*
		// No culling as we don't really care about render performance
		m_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		this->updateMatrices( );

		uint vertexCount = 0;
		uint triangleCount = 0;

		this->beginFrame( 0x353535 );
		for ( uint i = 0; i < m_model.numMeshes( ); i++ ) {
			this->drawMesh( i );
			vertexCount += m_model.mesh( i ).vertices.GetSize( );
			triangleCount += m_model.mesh( i ).triangles.GetSize( );
		}

		this->drawText( 0, 0, wxString::Format( wxT( "Meshes: %d" ), m_model.numMeshes( ) ) );
		this->drawText( 0, 0x14, wxString::Format( wxT( "Vertices: %d" ), vertexCount ) );
		this->drawText( 0, 0x28, wxString::Format( wxT( "Triangles: %d" ), triangleCount ) );

		wxSize clientSize = this->GetClientSize( );
		this->drawText( 0, clientSize.y - 0x50, wxT( "Focus: F button" ) );
		this->drawText( 0, clientSize.y - 0x3c, wxT( "Pan: Middle mouse button" ) );
		this->drawText( 0, clientSize.y - 0x28, wxT( "Rotate: Left mouse button" ) );
		this->drawText( 0, clientSize.y - 0x14, wxT( "Zoom: Scroll wheel" ) );

		this->endFrame( );
*/
	}

	void ModelViewer::onPaintEvt( wxPaintEvent& p_event ) {
		this->render( );
	}

	void ModelViewer::drawMesh( uint p_meshIndex ) {
	}

	void ModelViewer::drawText( uint p_x, uint p_y, const wxString& p_text ) {
	}

	int* ModelViewer::loadTexture( uint p_fileId ) {
		auto entryNumber = this->datFile( )->entryNumFromFileId( p_fileId );
		auto fileData = this->datFile( )->readEntry( entryNumber );

		// Bail if read failed
		if ( fileData.GetSize( ) == 0 ) {
			return nullptr;
		}

		// Convert to image
		ANetFileType fileType;
		this->datFile( )->identifyFileType( fileData.GetPointer( ), fileData.GetSize( ), fileType );
		auto reader = FileReader::readerForData( fileData, fileType );

		// Bail if not an image
		auto imgReader = dynamic_cast<ImageReader*>( reader );
		if ( !imgReader ) {
			deletePointer( reader );
			return nullptr;
		}

		// Convert to PNG and bail if invalid
		auto pngData = imgReader->convertData( );
		if ( pngData.GetSize( ) == 0 ) {
			deletePointer( reader );
			return nullptr;
		}

		// Finally, load texture from in-memory PNG.
		//IDirect3DTexture9* texture = nullptr;
		int* texture = nullptr;
		//::D3DXCreateTextureFromFileInMemory( m_device.get( ), pngData.GetPointer( ), pngData.GetSize( ), &texture );

		// Delete reader and return
		deletePointer( reader );
		return texture;
	}

	void ModelViewer::focus( ) {
		/*
		float fov = ( 5.0f / 12.0f ) * M_PI;
		uint meshCount = m_model.numMeshes( );

		if ( !meshCount ) {
			return;
		}

		// Calculate complete bounds
		Bounds bounds = m_model.bounds( );
		float height = bounds.max.z - bounds.min.z;
		if ( height <= 0 ) {
			return;
		}

		float distance = bounds.min.y - ( ( height * 0.5f ) / ::tanf( fov * 0.5f ) );
		if ( distance < 0 ) {
			distance *= -1;
		}

		// Update camera and render
		//m_camera.setPivot( bounds.center( ) );
		m_camera.setDistance( distance );
		*/
		this->render( );
	}

	void ModelViewer::onMotionEvt( wxMouseEvent& p_event ) {
		/*
		if ( m_lastMousePos.x == std::numeric_limits<int>::min( ) &&
			m_lastMousePos.y == std::numeric_limits<int>::min( ) ) {
			m_lastMousePos = p_event.GetPosition( );
		}

		// Yaw/Pitch
		if ( p_event.LeftIsDown( ) ) {
			float rotateSpeed = 0.5f * ( M_PI / 180.0f );   // 0.5 degrees per pixel
			m_camera.addYaw( rotateSpeed * -( p_event.GetX( ) - m_lastMousePos.x ) );
			m_camera.addPitch( rotateSpeed * ( p_event.GetY( ) - m_lastMousePos.y ) );
			this->render( );
		}

		// Pan
		if ( p_event.MiddleIsDown( ) ) {
			float xPan = -( p_event.GetX( ) - m_lastMousePos.x );
			float yPan = -( p_event.GetY( ) - m_lastMousePos.y );
			m_camera.pan( xPan, yPan );
			this->render( );
		}

		m_lastMousePos = p_event.GetPosition( );
		*/
	}

	void ModelViewer::onMouseWheelEvt( wxMouseEvent& p_event ) {
		float zoomSteps = static_cast<float>( p_event.GetWheelRotation( ) ) / static_cast<float>( p_event.GetWheelDelta( ) );
		//m_camera.multiplyDistance( -zoomSteps );
		this->render( );
	}

	void ModelViewer::onKeyDownEvt( wxKeyEvent& p_event ) {
		if ( p_event.GetKeyCode( ) == 'F' ) {
			this->focus( );
		}
	}

}; // namespace gw2b
