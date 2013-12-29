/* \file       Viewers/ModelViewer.cpp
*  \brief      Contains the definition of the model viewer class.
*  \author     Rhoot
*/

/*
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

	namespace {

		const uint32 c_vertexFVF = ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

		bool doesUseAlpha( const wxString& p_shaderName ) {
			if ( p_shaderName.StartsWith( wxT( "AmatShader" ) ) ) {
				auto ordinal = p_shaderName.Mid( ::strlen( "AmatShader" ) );
				return ( ordinal != "15" ) &&
					( ordinal != "4N" ) &&
					( ordinal != "2" );
			}
			return false;
		}

	};

	ModelViewer::ModelViewer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
		: Viewer( p_parent, p_pos, p_size )
		, m_lastMousePos( std::numeric_limits<int>::min( ), std::numeric_limits<int>::min( ) ) {
		::memset( &m_presentParams, 0, sizeof( m_presentParams ) );
		m_presentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		m_presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_presentParams.Windowed = TRUE;
		m_presentParams.BackBufferCount = 1;
		m_presentParams.BackBufferWidth = wxSystemSettings::GetMetric( wxSYS_SCREEN_X );
		m_presentParams.BackBufferHeight = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );
		m_presentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
		m_presentParams.EnableAutoDepthStencil = TRUE;
		m_presentParams.AutoDepthStencilFormat = D3DFMT_D16;

		// Init Direct3D
		IDirect3DDevice9* device = nullptr;
		m_d3d.reset( ::Direct3DCreate9( D3D_SDK_VERSION ) );
		m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->GetHandle( ), D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE, &m_presentParams, &device );
		m_device.reset( device );

		// Load shader
		ID3DXEffect* effect = nullptr;
		ID3DXBuffer* errors = nullptr;
		uint32 shaderFlags = ifDebug( D3DXSHADER_DEBUG, 0 );
		auto result = ::D3DXCreateEffect( m_device.get( ), data::model_view_hlsl, data::model_view_hlsl_size, nullptr, nullptr, shaderFlags, nullptr, &effect, &errors );
		m_effect.reset( effect );

		if ( FAILED( result ) ) {
			wxMessageBox( wxString::Format( wxT( "Error 0x%08x while loading shader: %s" ), result, static_cast<const char*>( errors->GetBufferPointer( ) ) ) );
		}

		// Load font
		ID3DXFont* font = nullptr;
		::D3DXCreateFontA( m_device.get( ), 18, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI", &font );
		m_font.reset( font );

		// Hook up events
		this->Connect( wxEVT_PAINT, wxPaintEventHandler( ModelViewer::onPaintEvt ) );
		this->Connect( wxEVT_MOTION, wxMouseEventHandler( ModelViewer::onMotionEvt ) );
		this->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( ModelViewer::onMouseWheelEvt ) );
		this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ModelViewer::onKeyDownEvt ) );
	}

	ModelViewer::~ModelViewer( ) {
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
	}

	void ModelViewer::clear( ) {
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
		Viewer::clear( );
	}

	void ModelViewer::setReader( FileReader* p_reader ) {
		Ensure::isOfType<ModelReader>( p_reader );
		Viewer::setReader( p_reader );

		// Load model
		auto reader = this->modelReader( );
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

		// Re-focus and re-render
		this->focus( );
		this->render( );
	}

	bool ModelViewer::createBuffers( MeshCache& p_cache, uint p_vertexCount, uint p_vertexSize, uint p_indexCount, uint p_indexSize ) {
		p_cache.indexBuffer = nullptr;
		p_cache.vertexBuffer = nullptr;

		// 0 indices or 0 vertices, either is an empty mesh
		if ( !p_vertexCount || !p_indexCount ) {
			return false;
		}

		// Allocate vertex buffer and bail if it fails
		if ( FAILED( m_device->CreateVertexBuffer( p_vertexCount * p_vertexSize, D3DUSAGE_WRITEONLY, c_vertexFVF,
			D3DPOOL_DEFAULT, &p_cache.vertexBuffer, nullptr ) ) ) {
			return false;
		}

		// Allocate index buffer and bail if it fails
		if ( FAILED( m_device->CreateIndexBuffer( p_indexCount * p_indexSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
			D3DPOOL_DEFAULT, &p_cache.indexBuffer, nullptr ) ) ) {
			p_cache.vertexBuffer->Release( );
			p_cache.vertexBuffer = nullptr;
			p_cache.indexBuffer = nullptr;
			return false;
		}

		return true;
	}

	bool ModelViewer::populateBuffers( const Mesh& p_mesh, MeshCache& p_cache ) {
		uint vertexCount = p_mesh.vertices.GetSize( );
		uint vertexSize = sizeof( Vertex );
		uint indexCount = p_mesh.triangles.GetSize( ) * 3;
		uint indexSize = sizeof( uint16 );

		// Lock vertex buffer
		Vertex* vertices;
		if ( FAILED( p_cache.vertexBuffer->Lock( 0, vertexCount * vertexSize, reinterpret_cast<void**>( &vertices ), 0 ) ) ) {
			return false;
		}

		// Populate vertex buffer
		std::copy_n( &p_mesh.vertices[0], vertexCount, &vertices[0] );

		// Unlock vertex buffer
		Assert( SUCCEEDED( p_cache.vertexBuffer->Unlock( ) ) );

		// Lock index buffer
		uint16* indices;
		if ( FAILED( p_cache.indexBuffer->Lock( 0, indexCount * indexSize, reinterpret_cast<void**>( &indices ), 0 ) ) ) {
			return false;
		}
		// Copy index buffer
		std::copy_n( &p_mesh.triangles[0].index1, indexCount, &indices[0] );
		Assert( SUCCEEDED( p_cache.indexBuffer->Unlock( ) ) );

		return true;
	}

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

	void ModelViewer::render( ) {
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
	}

	void ModelViewer::onPaintEvt( wxPaintEvent& p_event ) {
		this->render( );
	}

	void ModelViewer::drawMesh( uint p_meshIndex ) {
		auto& mesh = m_meshCache[p_meshIndex];

		// No mesh to draw?
		if ( mesh.indexBuffer == nullptr || mesh.vertexBuffer == nullptr ) {
			return;
		}

		// Count vertices / primitives
		uint vertexCount = m_model.mesh( p_meshIndex ).vertices.GetSize( );
		uint primitiveCount = m_model.mesh( p_meshIndex ).triangles.GetSize( );
		int  materialIndex = m_model.mesh( p_meshIndex ).materialIndex;

		// Set buffers
		if ( FAILED( m_device->SetFVF( c_vertexFVF ) ) ) {
			return;
		}
		if ( FAILED( m_device->SetStreamSource( 0, mesh.vertexBuffer, 0, sizeof( Vertex ) ) ) ) {
			return;
		}
		if ( FAILED( m_device->SetIndices( mesh.indexBuffer ) ) ) {
			return;
		}

		// Alpha blending, for alpha support
		if ( doesUseAlpha( m_model.mesh( p_meshIndex ).materialName ) ) {
			m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			// Alpha testing, so we can still render behind transparent pixels
			D3DCAPS9 caps;
			m_device->GetDeviceCaps( &caps );
			if ( caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL ) {
				m_device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				m_device->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				m_device->SetRenderState( D3DRS_ALPHAREF, 0x7f );
			}
		} else {
			m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			m_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		}

		// Begin drawing
		uint numPasses;
		m_effect->SetTechnique( "RenderScene" );
		m_effect->Begin( &numPasses, 0 );

		// Update texture
		if ( materialIndex >= 0 && m_textureCache[materialIndex].diffuseMap ) {
			m_effect->SetTexture( "g_DiffuseTex", m_textureCache[materialIndex].diffuseMap );
		}

		// Draw each shader pass
		for ( uint i = 0; i < numPasses; i++ ) {
			m_effect->BeginPass( i );
			m_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, primitiveCount );
			m_effect->EndPass( );
		}

		// End
		m_effect->End( );
	}

	void ModelViewer::drawText( uint p_x, uint p_y, const wxString& p_text ) {
		if ( m_font.get( ) ) {
			RECT outRect;
			::SetRect( &outRect, p_x, p_y, p_x + 0x200, p_y + 0x14 );
			m_font->DrawTextW( NULL, p_text.wchar_str( ), -1, &outRect, DT_LEFT | DT_NOCLIP, 0xFFFFFFFF );
		}
	}


	void ModelViewer::updateMatrices( ) {
		// All models are located at 0,0,0 with no rotation, so no world matrix is needed

		// Calculate minZ/maxZ
		auto bounds = m_model.bounds( );
		auto size = bounds.size( );
		auto distance = m_camera.distance( );
		auto extents = XMFLOAT3( size.x * 0.5f, size.y * 0.5f, size.z * 0.5f );

		auto maxSize = ::sqrt( extents.x * extents.x + extents.y * extents.y + extents.z * extents.z );
		auto maxZ = ( maxSize + distance ) * 1.1f;
		auto minZ = maxZ * 0.0001f;

		// View matrix
		auto viewMatrix = m_camera.calculateViewMatrix( );

		// Projection matrix
		wxSize clientSize = this->GetClientSize( );
		float aspectRatio = ( static_cast<float>( clientSize.x ) / static_cast<float>( clientSize.y ) );
		auto projMatrix = ::XMMatrixPerspectiveFovLH( ( 5.0f / 12.0f ) * XM_PI, aspectRatio, minZ, maxZ );

		// WorldViewProjection matrix
		XMFLOAT4X4 worldViewProjMatrix;
		::XMStoreFloat4x4( &worldViewProjMatrix, ::XMMatrixMultiply( viewMatrix, projMatrix ) );

		m_effect->SetMatrix( "g_WorldViewProjMatrix", reinterpret_cast<D3DXMATRIX*>( &worldViewProjMatrix ) );
	}

	IDirect3DTexture9* ModelViewer::loadTexture( uint p_fileId ) {
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
		IDirect3DTexture9* texture = nullptr;
		::D3DXCreateTextureFromFileInMemory( m_device.get( ), pngData.GetPointer( ), pngData.GetSize( ), &texture );

		// Delete reader and return
		deletePointer( reader );
		return texture;
	}

	void ModelViewer::focus( ) {
		float fov = ( 5.0f / 12.0f ) * XM_PI;
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
		m_camera.setPivot( bounds.center( ) );
		m_camera.setDistance( distance );
		this->render( );
	}

	void ModelViewer::onMotionEvt( wxMouseEvent& p_event ) {
		if ( m_lastMousePos.x == std::numeric_limits<int>::min( ) &&
			m_lastMousePos.y == std::numeric_limits<int>::min( ) ) {
			m_lastMousePos = p_event.GetPosition( );
		}

		// Yaw/Pitch
		if ( p_event.LeftIsDown( ) ) {
			float rotateSpeed = 0.5f * ( XM_PI / 180.0f );   // 0.5 degrees per pixel
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
	}

	void ModelViewer::onMouseWheelEvt( wxMouseEvent& p_event ) {
		float zoomSteps = static_cast<float>( p_event.GetWheelRotation( ) ) / static_cast<float>( p_event.GetWheelDelta( ) );
		m_camera.multiplyDistance( -zoomSteps );
		this->render( );
	}

	void ModelViewer::onKeyDownEvt( wxKeyEvent& p_event ) {
		if ( p_event.GetKeyCode( ) == 'F' ) {
			this->focus( );
		}
	}

}; // namespace gw2b
