/** \file       Viewers/ModelViewer.h
 *  \brief      Contains the declaration of the model viewer class.
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

#ifndef VIEWERS_MODELVIEWER_H_INCLUDED
#define VIEWERS_MODELVIEWER_H_INCLUDED

#include "ModelViewer/Camera.h"
#include "Readers/ModelReader.h"
#include "Viewer.h"

namespace gw2b
{

#pragma pack(push, 1)

struct VertexDefinition
{
    XMFLOAT3 mPosition;
    XMFLOAT3 mNormal;
    uint32 mDiffuse;
    XMFLOAT2 mUV[2];
    static uint32 sFVF;
};

#pragma pack(pop)

struct MeshCache
{
    IDirect3DIndexBuffer9*  mIndexBuffer;
    IDirect3DVertexBuffer9* mVertexBuffer;
};

struct TextureCache
{
    IDirect3DTexture9*  mDiffuseMap;
    IDirect3DTexture9*  mNormalMap;
};

struct AutoReleaser
{
    template <typename T>
        void operator()(T* p_pointer) const
    {
        p_pointer->Release();
    }
};

class ModelViewer : public Viewer, public INeedDatFile
{
    std::unique_ptr<IDirect3D9, AutoReleaser>       mD3D;
    std::unique_ptr<IDirect3DDevice9, AutoReleaser> mDevice;
    std::unique_ptr<ID3DXEffect, AutoReleaser>      mEffect;

    D3DPRESENT_PARAMETERS       mPresentParams;
    Model                       mModel;
    Array<MeshCache>            mMeshCache;
    Array<TextureCache>         mTextureCache;
    Camera                      mCamera;
    wxPoint                     mLastMousePos;
public:
    ModelViewer(wxWindow* pParent, const wxPoint& pPos = wxDefaultPosition, const wxSize& pSize = wxDefaultSize);
    virtual ~ModelViewer();

    virtual void clear();
    virtual void setReader(FileReader* pReader);
    /** Gets the image reader containing the data displayed by this viewer.
     *  \return ModelReader*    Reader containing the data. */
    ModelReader* GetModelReader()               { return (ModelReader*)this->reader(); }
    /** Gets the image reader containing the data displayed by this viewer.
     *  \return ModelReader*    Reader containing the data. */
    const ModelReader* GetModelReader() const   { return (const ModelReader*)this->reader(); }

    void Focus();

    void DrawMesh(uint pMeshIndex);

private:
    void OnPaintEvt(wxPaintEvent& pEvent);
    void OnMotionEvt(wxMouseEvent& pEvent);
    void OnMouseWheelEvt(wxMouseEvent& pEvent);
    void OnKeyDownEvt(wxKeyEvent& pEvent);
    void BeginFrame(uint32 pClearColor);
    void EndFrame();
    void Render();
    bool CreateBuffers(MeshCache& pCache, uint pVertexCount, uint pVertexSize, uint pIndexCount, uint pIndexSize);
    bool PopulateBuffers(const Mesh& pMesh, MeshCache& pCache);
    void UpdateMatrices();
    IDirect3DTexture9* LoadTexture(uint pFileId);
}; // class ImageViewer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_H_INCLUDED
