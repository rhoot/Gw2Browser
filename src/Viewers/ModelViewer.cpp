/** \file       Viewers/ModelViewer.cpp
 *  \brief      Contains the definition of the model viewer class.
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
#include "ModelViewer.h"
#include "DatFile.h"
#include "Readers/ImageReader.h"

namespace gw2b
{

uint32 VertexDefinition::sFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2);

ModelViewer::ModelViewer(wxWindow* pParent, const wxPoint& pPos, const wxSize& pSize)
    : Viewer(pParent, pPos, pSize)
    , mLastMousePos(std::numeric_limits<int>::min(), std::numeric_limits<int>::min())
{
    ::memset(&mPresentParams, 0, sizeof(mPresentParams));
    mPresentParams.PresentationInterval    = D3DPRESENT_INTERVAL_IMMEDIATE;
    mPresentParams.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    mPresentParams.Windowed                = true;
    mPresentParams.BackBufferCount         = 1;
    mPresentParams.BackBufferWidth         = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    mPresentParams.BackBufferHeight        = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    mPresentParams.BackBufferFormat        = D3DFMT_A8R8G8B8;
    mPresentParams.EnableAutoDepthStencil  = true;
    mPresentParams.AutoDepthStencilFormat  = D3DFMT_D16;

    // Init Direct3D
    IDirect3DDevice9* device = nullptr;
    mD3D.reset(::Direct3DCreate9(D3D_SDK_VERSION));
    mD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->GetHandle(), D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE, &mPresentParams, &device);
    mDevice.reset(device);

    // Load shader
    ID3DXEffect* effect = nullptr;
    ID3DXBuffer* errors = nullptr;
    uint32 shaderFlags  = ifDebug(D3DXSHADER_DEBUG, 0);
    HRESULT result = ::D3DXCreateEffectFromFileW(mDevice.get(), L"data/model_view.hlsl", nullptr, nullptr, shaderFlags, nullptr, &effect, &errors);
    mEffect.reset(effect);

    if (FAILED(result)) {
        wxMessageBox(wxString::Format(wxT("Error 0x%08x while loading shader: %s"), result, static_cast<const char*>(errors->GetBufferPointer())));
    }

    // Hook up events
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(ModelViewer::OnPaintEvt));
    this->Connect(wxEVT_MOTION, wxMouseEventHandler(ModelViewer::OnMotionEvt));
    this->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(ModelViewer::OnMouseWheelEvt));
    this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(ModelViewer::OnKeyDownEvt));
}

ModelViewer::~ModelViewer()
{
    for (uint i = 0; i < mMeshCache.GetSize(); i++) {
        if (mMeshCache[i].mIndexBuffer)  { mMeshCache[i].mIndexBuffer->Release();  }
        if (mMeshCache[i].mVertexBuffer) { mMeshCache[i].mVertexBuffer->Release(); }
    }
    for (uint i = 0; i < mTextureCache.GetSize(); i++) {
        if (mTextureCache[i].mDiffuseMap) { mTextureCache[i].mDiffuseMap->Release(); }
        if (mTextureCache[i].mNormalMap)  { mTextureCache[i].mNormalMap->Release();  }
    }
}

void ModelViewer::clear()
{
    for (uint i = 0; i < mMeshCache.GetSize(); i++) {
        if (mMeshCache[i].mIndexBuffer)  { mMeshCache[i].mIndexBuffer->Release();  }
        if (mMeshCache[i].mVertexBuffer) { mMeshCache[i].mVertexBuffer->Release(); }
    }
    for (uint i = 0; i < mTextureCache.GetSize(); i++) {
        if (mTextureCache[i].mDiffuseMap) { mTextureCache[i].mDiffuseMap->Release(); }
        if (mTextureCache[i].mNormalMap)  { mTextureCache[i].mNormalMap->Release();  }
    }
    mTextureCache.Clear();
    mMeshCache.Clear();
    mModel = Model();
    Viewer::clear();
}

void ModelViewer::setReader(FileReader* pReader)
{
    Ensure::isOfType<ModelReader>(pReader);
    Viewer::setReader(pReader);

    // Load model
    ModelReader* reader = this->GetModelReader();
    mModel = reader->GetModel();

    // Create DX mesh cache
    mMeshCache.SetSize(mModel.GetNumMeshes());

    // Load meshes
    for (uint i = 0; i < mModel.GetNumMeshes(); i++) {
        const Mesh& mesh = mModel.GetMesh(i);
        MeshCache& cache = mMeshCache[i];

        // Create and populate the buffers
        uint vertexCount = mesh.mVertices.GetSize();
        uint vertexSize  = sizeof(VertexDefinition);
        uint indexCount  = mesh.mTriangles.GetSize() * 3;
        uint indexSize   = sizeof(uint16);

        if (!this->CreateBuffers(cache, vertexCount, vertexSize, indexCount, indexSize)) { continue; }
        if (!this->PopulateBuffers(mesh, cache)) { 
            releasePointer(cache.mIndexBuffer);
            releasePointer(cache.mVertexBuffer);
            continue;
        }
    }

    // Create DX texture cache
    mTextureCache.SetSize(mModel.GetNumMaterialData());

    // Load textures
    for (uint i = 0; i < mModel.GetNumMaterialData(); i++) {
        const MaterialData& material = mModel.GetMaterialData(i);
        TextureCache& cache = mTextureCache[i];

        // Load diffuse texture
        if (material.mDiffuseTexture) {
            cache.mDiffuseMap = this->LoadTexture(material.mDiffuseTexture);
        } else {
            cache.mDiffuseMap = nullptr;
        }

        // Load normal map
        if (material.mNormalMap) {
            cache.mNormalMap = this->LoadTexture(material.mNormalMap);
        } else {
            cache.mNormalMap = nullptr;
        }
    }

    // Re-focus and re-render
    this->Focus();
    this->Refresh(false);
}

bool ModelViewer::CreateBuffers(MeshCache& pCache, uint pVertexCount, uint pVertexSize, uint pIndexCount, uint pIndexSize)
{
    pCache.mIndexBuffer  = nullptr;
    pCache.mVertexBuffer = nullptr;

    // 0 indices or 0 vertices, either is an empty mesh
    if (!pVertexCount || !pIndexCount) {
        return false;
    }

    // Allocate vertex buffer and bail if it fails
    if (FAILED(mDevice->CreateVertexBuffer(pVertexCount * pVertexSize, D3DUSAGE_WRITEONLY, VertexDefinition::sFVF,
        D3DPOOL_DEFAULT, &pCache.mVertexBuffer, nullptr))) 
    {
        return false;
    }

    // Allocate index buffer and bail if it fails
    if (FAILED(mDevice->CreateIndexBuffer(pIndexCount * pIndexSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
        D3DPOOL_DEFAULT, &pCache.mIndexBuffer, nullptr))) 
    {
        pCache.mVertexBuffer->Release();
        pCache.mVertexBuffer = nullptr;
        pCache.mIndexBuffer  = nullptr;
        return false;
    }

    return true;
}

bool ModelViewer::PopulateBuffers(const Mesh& pMesh, MeshCache& pCache)
{
    uint vertexCount = pMesh.mVertices.GetSize();
    uint vertexSize  = sizeof(VertexDefinition);
    uint indexCount  = pMesh.mTriangles.GetSize() * 3;
    uint indexSize   = sizeof(uint16);

    // Lock vertex buffer
    VertexDefinition* vertices;
    if (FAILED(pCache.mVertexBuffer->Lock(0, vertexCount * vertexSize, reinterpret_cast<void**>(&vertices), 0))){
        return false;
    }

    // Populate vertex buffer
    for (uint j = 0; j < vertexCount; j++) {
        vertices[j].mPosition = pMesh.mVertices[j].mPosition;

        // Normal
        if (pMesh.mVertices[j].mHasNormal) {
            vertices[j].mNormal = pMesh.mVertices[j].mNormal;
        } else {
            vertices[j].mNormal.x = 0;
            vertices[j].mNormal.y = 0;
            vertices[j].mNormal.z = 1;
        }

        // Color
        if (pMesh.mVertices[j].mHasColor) {
            vertices[j].mDiffuse = pMesh.mVertices[j].mColor;
        } else {
            ::memset(&vertices[j].mDiffuse, 0xff, sizeof(uint32));
        }

        // UV1
        if (pMesh.mVertices[j].mHasUV > 0) {
            vertices[j].mUV[0] = pMesh.mVertices[j].mUV[0];
        } else {
            ::memset(&vertices[j].mUV[0], 0, sizeof(XMFLOAT2));
        }

        // UV2
        if (pMesh.mVertices[j].mHasUV > 1) {
            vertices[j].mUV[1] = pMesh.mVertices[j].mUV[1];
        } else {
            ::memset(&vertices[j].mUV[1], 0, sizeof(XMFLOAT2));
        }
    }
    Assert(SUCCEEDED(pCache.mVertexBuffer->Unlock()));

    // Lock index buffer
    uint16* indices;
    if (FAILED(pCache.mIndexBuffer->Lock(0, indexCount * indexSize, reinterpret_cast<void**>(&indices), 0))) {
        return false;
    }
    // Copy index buffer
    ::memcpy(indices, pMesh.mTriangles.GetPointer(), indexCount * indexSize);
    Assert(SUCCEEDED(pCache.mIndexBuffer->Unlock()));

    return true;
}

void ModelViewer::BeginFrame(uint32 pClearColor)
{
    wxSize clientSize = this->GetClientSize();

    D3DVIEWPORT9 viewport;
    ::memset(&viewport, 0, sizeof(viewport));
    viewport.Width  = clientSize.x;
    viewport.Height = clientSize.y;
    viewport.MaxZ   = 1;
    viewport.MinZ   = 0;

    mDevice->SetViewport(&viewport);
    mDevice->BeginScene();
    mDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, pClearColor, 1, 0);
}

void ModelViewer::EndFrame()
{
    D3DVIEWPORT9 viewport;
    mDevice->GetViewport(&viewport);

    RECT sourceRect;
    ::memset(&sourceRect, 0, sizeof(sourceRect));
    sourceRect.right = viewport.Width;
    sourceRect.bottom = viewport.Height;

    mDevice->EndScene();
    mDevice->Present(&sourceRect, nullptr, nullptr, nullptr);
}

void ModelViewer::Render()
{
    mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    this->UpdateMatrices();

    this->BeginFrame(0x353535);
    for (uint i = 0; i < mModel.GetNumMeshes(); i++) {
        this->DrawMesh(i);
    }
    this->EndFrame();
}

void ModelViewer::OnPaintEvt(wxPaintEvent& pEvent)
{
    this->Render();
}

void ModelViewer::DrawMesh(uint pMeshIndex)
{
    const MeshCache& mesh = mMeshCache[pMeshIndex];

    // No mesh to draw?
    if (mesh.mIndexBuffer == nullptr || mesh.mVertexBuffer == nullptr) {
        return;
    }

    // Count vertices / primitives
    uint vertexCount    = mModel.GetMesh(pMeshIndex).mVertices.GetSize();
    uint primitiveCount = mModel.GetMesh(pMeshIndex).mTriangles.GetSize();
    int  materialIndex  = mModel.GetMesh(pMeshIndex).mMaterialIndex;
    
    // Set buffers
    if (FAILED(mDevice->SetFVF(VertexDefinition::sFVF))) { return; }
    if (FAILED(mDevice->SetStreamSource(0, mesh.mVertexBuffer, 0, sizeof(VertexDefinition)))) { return; }
    if (FAILED(mDevice->SetIndices(mesh.mIndexBuffer))) { return; }

    // Begin drawing
    uint numPasses;
    mEffect->SetTechnique("RenderScene");
    mEffect->Begin(&numPasses, 0);

    // Update texture
    if (materialIndex >= 0 && mTextureCache[materialIndex].mDiffuseMap) {
        mEffect->SetTexture("g_DiffuseTex", mTextureCache[materialIndex].mDiffuseMap);
    }

    // Draw each shader pass
    for (uint i = 0; i < numPasses; i++) {
        mEffect->BeginPass(i);
        mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, primitiveCount);
        mEffect->EndPass();
    }

    // End
    mEffect->End();
}

void ModelViewer::UpdateMatrices()
{
    // All models are located at 0,0,0 with no rotation, so no world matrix is needed

    XMMATRIX viewMatrix;
    XMMATRIX projectionMatrix;

    // View matrix
    viewMatrix = mCamera.CalculateViewMatrix();

    // Projection matrix
    wxSize clientSize = this->GetClientSize();
    float aspectRatio = (static_cast<float>(clientSize.x) / static_cast<float>(clientSize.y));
    projectionMatrix  = ::XMMatrixPerspectiveFovLH((5.0f / 12.0f) * XM_PI, aspectRatio, 0.1f, 2000);

    // WorldViewProjection matrix
    XMFLOAT4X4 worldViewProjMatrix;
    ::XMStoreFloat4x4(&worldViewProjMatrix, ::XMMatrixMultiply(viewMatrix, projectionMatrix));

    mEffect->SetMatrix("g_WorldViewProjMatrix", reinterpret_cast<D3DXMATRIX*>(&worldViewProjMatrix));
}

IDirect3DTexture9* ModelViewer::LoadTexture(uint pFileId)
{
    uint entryNumber      = this->datFile()->entryNumFromFileId(pFileId);
    Array<byte> fileData  = this->datFile()->readEntry(entryNumber);

    // Bail if read failed
    if (fileData.GetSize() == 0) { return nullptr; }

    // Convert to image
    ANetFileType fileType;
    this->datFile()->identifyFileType(fileData.GetPointer(), fileData.GetSize(), fileType);
    FileReader* reader = FileReader::readerForData(fileData, fileType);
    
    // Bail if not an image
    ImageReader* imgReader = dynamic_cast<ImageReader*>(reader);
    if (!imgReader) {
        deletePointer(reader);
        return nullptr;
    }

    // Convert to PNG and bail if invalid
    Array<byte> pngData = imgReader->convertData();
    if (pngData.GetSize() == 0) {
        deletePointer(reader);
        return nullptr;
    }

    // Finally, load texture from in-memory PNG.
    IDirect3DTexture9* texture = nullptr;
    ::D3DXCreateTextureFromFileInMemory(mDevice.get(), pngData.GetPointer(), pngData.GetSize(), &texture);

    // Delete reader and return
    deletePointer(reader);
    return texture;
}

void ModelViewer::Focus()
{
    float fov      = (5.0f / 12.0f) * XM_PI;
    uint meshCount = mModel.GetNumMeshes();
    
    if (!meshCount) { return; }

    // Calculate complete bounds
    Bounds bounds;
    for (uint i = 0; i < meshCount; i++) {
        const Bounds& meshBounds = mModel.GetMesh(i).mBounds;

        if (i > 0) {
            bounds.mMinX = wxMin(bounds.mMinX, meshBounds.mMinX);
            bounds.mMinY = wxMin(bounds.mMinY, meshBounds.mMinY);
            bounds.mMinZ = wxMin(bounds.mMinZ, meshBounds.mMinZ);
            bounds.mMaxX = wxMax(bounds.mMaxX, meshBounds.mMaxX);
            bounds.mMaxY = wxMax(bounds.mMaxY, meshBounds.mMaxY);
            bounds.mMaxZ = wxMax(bounds.mMaxZ, meshBounds.mMaxZ);
        } else {
            bounds = meshBounds;
        }
    }

    float height = bounds.mMaxZ - bounds.mMinZ;
    if (height <= 0) { return; }

    float distance = bounds.mMinY - ((height * 0.5f) / ::tanf(fov * 0.5f));
    if (distance < 0) { distance *= -1; }

    // Calculate new pivot point
    XMFLOAT3 min(bounds.mMinX, bounds.mMinY, bounds.mMinZ);
    XMFLOAT3 max(bounds.mMaxX, bounds.mMaxY, bounds.mMaxZ);
    XMVECTOR minVector    = ::XMLoadFloat3(&min);
    XMVECTOR maxVector    = ::XMLoadFloat3(&max);
    XMVECTOR centerVector = ::XMVectorScale(::XMVectorAdd(minVector, maxVector), 0.5f);

    XMFLOAT3 center;
    ::XMStoreFloat3(&center, centerVector);

    // Update camera and re-render
    mCamera.SetPivot(center);
    mCamera.SetDistance(distance);
    this->Refresh(false);
}

void ModelViewer::OnMotionEvt(wxMouseEvent& pEvent)
{
    if (mLastMousePos.x == std::numeric_limits<int>::min() && 
        mLastMousePos.y == std::numeric_limits<int>::min()) 
    {
        mLastMousePos = pEvent.GetPosition();
    }

    // Yaw/Pitch
    if (pEvent.LeftIsDown()) {
        float rotateSpeed = (XM_PI / 180.0f);   // 1 degree per pixel
        mCamera.RotateYaw(rotateSpeed * -(pEvent.GetX() - mLastMousePos.x));
        mCamera.RotatePitch(rotateSpeed * (pEvent.GetY() - mLastMousePos.y));
        this->Refresh(false);
    }

    // Pan
    if (pEvent.MiddleIsDown()) {
        float xPan = -(pEvent.GetX() - mLastMousePos.x);
        float yPan = -(pEvent.GetY() - mLastMousePos.y);
        mCamera.Pan(xPan, yPan);
        this->Refresh(false);
    }

    mLastMousePos = pEvent.GetPosition();
}

void ModelViewer::OnMouseWheelEvt(wxMouseEvent& pEvent)
{
    float zoomSteps = static_cast<float>(pEvent.GetWheelRotation()) / static_cast<float>(pEvent.GetWheelDelta());
    mCamera.MultiplyDistance(-zoomSteps);
    this->Refresh(false);
}

void ModelViewer::OnKeyDownEvt(wxKeyEvent& pEvent)
{
    if (pEvent.GetKeyCode() == 'F') {
        this->Focus();
    }
}

}; // namespace gw2b
