/* \file       Viewers/ModelViewer.h
*  \brief      Contains the declaration of the model viewer class.
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

#pragma once

#ifndef VIEWERS_MODELVIEWER_H_INCLUDED
#define VIEWERS_MODELVIEWER_H_INCLUDED

#include "Camera.h"
#include "Readers/ModelReader.h"
#include "Viewer.h"

namespace gw2b {

	struct MeshCache {
		IDirect3DIndexBuffer9*  indexBuffer;
		IDirect3DVertexBuffer9* vertexBuffer;
	};

	struct TextureCache {
		IDirect3DTexture9*  diffuseMap;
	};

	struct AutoReleaser {
		template <typename T>
		void operator()( T* p_pointer ) const {
			p_pointer->Release( );
		}
	};

	class ModelViewer : public Viewer, public INeedDatFile {
		std::unique_ptr<IDirect3D9, AutoReleaser>       m_d3d;
		std::unique_ptr<IDirect3DDevice9, AutoReleaser> m_device;
		std::unique_ptr<ID3DXEffect, AutoReleaser>      m_effect;
		std::unique_ptr<ID3DXFont, AutoReleaser>        m_font;

		D3DPRESENT_PARAMETERS       m_presentParams;
		Model                       m_model;
		Array<MeshCache>            m_meshCache;
		Array<TextureCache>         m_textureCache;
		Camera                      m_camera;
		wxPoint                     m_lastMousePos;
		float                       m_minDistance;
		float                       m_maxDistance;
	public:
		ModelViewer( wxWindow* p_parent, const wxPoint& p_pos = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
		virtual ~ModelViewer( );

		virtual void clear( ) override;
		virtual void setReader( FileReader* p_reader ) override;
		/** Gets the image reader containing the data displayed by this viewer.
		*  \return ModelReader*    Reader containing the data. */
		ModelReader* modelReader( ) {
			return reinterpret_cast<ModelReader*>( this->reader( ) );
		}       // already asserted with a dynamic_cast
		/** Gets the image reader containing the data displayed by this viewer.
		*  \return ModelReader*    Reader containing the data. */
		const ModelReader* modelReader( ) const {
			return reinterpret_cast<const ModelReader*>( this->reader( ) );
		} // already asserted with a dynamic_cast

		void focus( );
		void drawMesh( uint p_meshIndex );
		void drawText( uint p_x, uint p_y, const wxString& p_text );

	private:
		void onPaintEvt( wxPaintEvent& p_event );
		void onMotionEvt( wxMouseEvent& p_event );
		void onMouseWheelEvt( wxMouseEvent& p_event );
		void onKeyDownEvt( wxKeyEvent& p_event );
		void beginFrame( uint32 p_clearColor );
		void endFrame( );
		void render( );
		bool createBuffers( MeshCache& p_cache, uint p_vertexCount, uint p_vertexSize, uint p_indexCount, uint p_indexSize );
		bool populateBuffers( const Mesh& p_mesh, MeshCache& p_cache );
		void updateMatrices( );
		IDirect3DTexture9* loadTexture( uint p_fileId );
	}; // class ImageViewer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_H_INCLUDED
