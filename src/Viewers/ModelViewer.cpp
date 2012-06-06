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

uint32 VertexDefinition::s_fvf(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2);

ModelViewer::ModelViewer(wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size)
    : Viewer(p_parent, p_pos, p_size)
    , m_lastMousePos(std::numeric_limits<int>::min(), std::numeric_limits<int>::min())
{
    ::memset(&m_presentParams, 0, sizeof(m_presentParams));
    m_presentParams.PresentationInterval    = D3DPRESENT_INTERVAL_IMMEDIATE;
    m_presentParams.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    m_presentParams.Windowed                = true;
    m_presentParams.BackBufferCount         = 1;
    m_presentParams.BackBufferWidth         = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    m_presentParams.BackBufferHeight        = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    m_presentParams.BackBufferFormat        = D3DFMT_A8R8G8B8;
    m_presentParams.EnableAutoDepthStencil  = true;
    m_presentParams.AutoDepthStencilFormat  = D3DFMT_D16;

    // Init Direct3D
    IDirect3DDevice9* device = nullptr;
    m_d3d.reset(::Direct3DCreate9(D3D_SDK_VERSION));
    m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->GetHandle(), D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE, &m_presentParams, &device);
    m_device.reset(device);

    // Load shader
    ID3DXEffect* effect = nullptr;
    ID3DXBuffer* errors = nullptr;
    uint32 shaderFlags  = ifDebug(D3DXSHADER_DEBUG, 0);
    HRESULT result = ::D3DXCreateEffectFromFileW(m_device.get(), L"data/model_view.hlsl", nullptr, nullptr, shaderFlags, nullptr, &effect, &errors);
    m_effect.reset(effect);

    if (FAILED(result)) {
        wxMessageBox(wxString::Format(wxT("Error 0x%08x while loading shader: %s"), result, static_cast<const char*>(errors->GetBufferPointer())));
    }

    // Load font
    ID3DXFont* font = nullptr;
    ::D3DXCreateFontA(m_device.get(), 18, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI", &font);
    m_font.reset(font);

    // Hook up events
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(ModelViewer::onPaintEvt));
    this->Connect(wxEVT_MOTION, wxMouseEventHandler(ModelViewer::onMotionEvt));
    this->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(ModelViewer::onMouseWheelEvt));
    this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(ModelViewer::onKeyDownEvt));
}

ModelViewer::~ModelViewer()
{
    for (uint i = 0; i < m_meshCache.GetSize(); i++) {
        if (m_meshCache[i].indexBuffer)  { m_meshCache[i].indexBuffer->Release();  }
        if (m_meshCache[i].vertexBuffer) { m_meshCache[i].vertexBuffer->Release(); }
    }
    for (uint i = 0; i < m_textureCache.GetSize(); i++) {
        if (m_textureCache[i].diffuseMap) { m_textureCache[i].diffuseMap->Release(); }
        if (m_textureCache[i].normalMap)  { m_textureCache[i].normalMap->Release();  }
    }
}

void ModelViewer::clear()
{
    for (uint i = 0; i < m_meshCache.GetSize(); i++) {
        if (m_meshCache[i].indexBuffer)  { m_meshCache[i].indexBuffer->Release();  }
        if (m_meshCache[i].vertexBuffer) { m_meshCache[i].vertexBuffer->Release(); }
    }
    for (uint i = 0; i < m_textureCache.GetSize(); i++) {
        if (m_textureCache[i].diffuseMap) { m_textureCache[i].diffuseMap->Release(); }
        if (m_textureCache[i].normalMap)  { m_textureCache[i].normalMap->Release();  }
    }
    m_textureCache.Clear();
    m_meshCache.Clear();
    m_model = Model();
    Viewer::clear();
}

void ModelViewer::setReader(FileReader* p_reader)
{
    Ensure::isOfType<ModelReader>(p_reader);
    Viewer::setReader(p_reader);

    // Load model
    auto reader = this->modelReader();
    m_model = reader->GetModel();

    // Create DX mesh cache
    m_meshCache.SetSize(m_model.GetNumMeshes());

    // Load meshes
    for (uint i = 0; i < m_model.GetNumMeshes(); i++) {
        auto& mesh  = m_model.GetMesh(i);
        auto& cache = m_meshCache[i];

        // Create and populate the buffers
        uint vertexCount = mesh.mVertices.GetSize();
        uint vertexSize  = sizeof(VertexDefinition);
        uint indexCount  = mesh.mTriangles.GetSize() * 3;
        uint indexSize   = sizeof(uint16);

        if (!this->createBuffers(cache, vertexCount, vertexSize, indexCount, indexSize)) { continue; }
        if (!this->populateBuffers(mesh, cache)) { 
            releasePointer(cache.indexBuffer);
            releasePointer(cache.vertexBuffer);
            continue;
        }
    }

    // Create DX texture cache
    m_textureCache.SetSize(m_model.GetNumMaterialData());

    // Load textures
    for (uint i = 0; i < m_model.GetNumMaterialData(); i++) {
        auto& material = m_model.GetMaterialData(i);
        auto& cache    = m_textureCache[i];

        // Load diffuse texture
        if (material.mDiffuseTexture) {
            cache.diffuseMap = this->loadTexture(material.mDiffuseTexture);
        } else {
            cache.diffuseMap = nullptr;
        }

        // Load normal map
        if (material.mNormalMap) {
            cache.normalMap = this->loadTexture(material.mNormalMap);
        } else {
            cache.normalMap = nullptr;
        }
    }

    // Re-focus and re-render
    this->focus();
    this->Refresh(false);
}

bool ModelViewer::createBuffers(MeshCache& p_cache, uint p_vertexCount, uint p_vertexSize, uint p_indexCount, uint p_indexSize)
{
    p_cache.indexBuffer  = nullptr;
    p_cache.vertexBuffer = nullptr;

    // 0 indices or 0 vertices, either is an empty mesh
    if (!p_vertexCount || !p_indexCount) {
        return false;
    }

    // Allocate vertex buffer and bail if it fails
    if (FAILED(m_device->CreateVertexBuffer(p_vertexCount * p_vertexSize, D3DUSAGE_WRITEONLY, VertexDefinition::s_fvf,
        D3DPOOL_DEFAULT, &p_cache.vertexBuffer, nullptr))) 
    {
        return false;
    }

    // Allocate index buffer and bail if it fails
    if (FAILED(m_device->CreateIndexBuffer(p_indexCount * p_indexSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
        D3DPOOL_DEFAULT, &p_cache.indexBuffer, nullptr))) 
    {
        p_cache.vertexBuffer->Release();
        p_cache.vertexBuffer = nullptr;
        p_cache.indexBuffer  = nullptr;
        return false;
    }

    return true;
}

bool ModelViewer::populateBuffers(const Mesh& pMesh, MeshCache& p_cache)
{
    uint vertexCount = pMesh.mVertices.GetSize();
    uint vertexSize  = sizeof(VertexDefinition);
    uint indexCount  = pMesh.mTriangles.GetSize() * 3;
    uint indexSize   = sizeof(uint16);

    // Lock vertex buffer
    VertexDefinition* vertices;
    if (FAILED(p_cache.vertexBuffer->Lock(0, vertexCount * vertexSize, reinterpret_cast<void**>(&vertices), 0))){
        return false;
    }

    // Populate vertex buffer
    for (uint j = 0; j < vertexCount; j++) {
        vertices[j].position = pMesh.mVertices[j].mPosition;

        // Normal
        if (pMesh.mVertices[j].mHasNormal) {
            vertices[j].normal = pMesh.mVertices[j].mNormal;
        } else {
            vertices[j].normal.x = 0;
            vertices[j].normal.y = 0;
            vertices[j].normal.z = 1;
        }

        // Color
        if (pMesh.mVertices[j].mHasColor) {
            vertices[j].diffuse = pMesh.mVertices[j].mColor;
        } else {
            ::memset(&vertices[j].diffuse, 0xff, sizeof(uint32));
        }

        // UV1
        if (pMesh.mVertices[j].mHasUV > 0) {
            vertices[j].uv[0] = pMesh.mVertices[j].mUV[0];
        } else {
            ::memset(&vertices[j].uv[0], 0, sizeof(XMFLOAT2));
        }

        // UV2
        if (pMesh.mVertices[j].mHasUV > 1) {
            vertices[j].uv[1] = pMesh.mVertices[j].mUV[1];
        } else {
            ::memset(&vertices[j].uv[1], 0, sizeof(XMFLOAT2));
        }
    }
    Assert(SUCCEEDED(p_cache.vertexBuffer->Unlock()));

    // Lock index buffer
    uint16* indices;
    if (FAILED(p_cache.indexBuffer->Lock(0, indexCount * indexSize, reinterpret_cast<void**>(&indices), 0))) {
        return false;
    }
    // Copy index buffer
    ::memcpy(indices, pMesh.mTriangles.GetPointer(), indexCount * indexSize);
    Assert(SUCCEEDED(p_cache.indexBuffer->Unlock()));

    return true;
}

void ModelViewer::beginFrame(uint32 p_clearColor)
{
    wxSize clientSize = this->GetClientSize();

    D3DVIEWPORT9 viewport;
    ::memset(&viewport, 0, sizeof(viewport));
    viewport.Width  = clientSize.x;
    viewport.Height = clientSize.y;
    viewport.MaxZ   = 1;
    viewport.MinZ   = 0;

    m_device->SetViewport(&viewport);
    m_device->BeginScene();
    m_device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, p_clearColor, 1, 0);
}

void ModelViewer::endFrame()
{
    D3DVIEWPORT9 viewport;
    m_device->GetViewport(&viewport);

    RECT sourceRect;
    ::memset(&sourceRect, 0, sizeof(sourceRect));
    sourceRect.right = viewport.Width;
    sourceRect.bottom = viewport.Height;

    m_device->EndScene();
    m_device->Present(&sourceRect, nullptr, nullptr, nullptr);
}

void ModelViewer::render()
{
    m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    this->updateMatrices();

    uint vertexCount   = 0;
    uint triangleCount = 0;

    this->beginFrame(0x353535);
    for (uint i = 0; i < m_model.GetNumMeshes(); i++) {
        this->drawMesh(i);
        vertexCount   += m_model.GetMesh(i).mVertices.GetSize();
        triangleCount += m_model.GetMesh(i).mTriangles.GetSize();
    }

    this->drawText(0, 0,    wxString::Format(wxT("Meshes: %d"), m_model.GetNumMeshes()));
    this->drawText(0, 0x18, wxString::Format(wxT("Vertices: %d"), vertexCount));
    this->drawText(0, 0x30, wxString::Format(wxT("Triangles: %d"), triangleCount));
    
    this->endFrame();
}

void ModelViewer::onPaintEvt(wxPaintEvent& p_event)
{
    this->render();
}

void ModelViewer::drawMesh(uint p_meshIndex)
{
    auto& mesh = m_meshCache[p_meshIndex];

    // No mesh to draw?
    if (mesh.indexBuffer == nullptr || mesh.vertexBuffer == nullptr) {
        return;
    }

    // Count vertices / primitives
    uint vertexCount    = m_model.GetMesh(p_meshIndex).mVertices.GetSize();
    uint primitiveCount = m_model.GetMesh(p_meshIndex).mTriangles.GetSize();
    int  materialIndex  = m_model.GetMesh(p_meshIndex).mMaterialIndex;
    
    // Set buffers
    if (FAILED(m_device->SetFVF(VertexDefinition::s_fvf))) { return; }
    if (FAILED(m_device->SetStreamSource(0, mesh.vertexBuffer, 0, sizeof(VertexDefinition)))) { return; }
    if (FAILED(m_device->SetIndices(mesh.indexBuffer))) { return; }

    // Begin drawing
    uint numPasses;
    m_effect->SetTechnique("RenderScene");
    m_effect->Begin(&numPasses, 0);

    // Update texture
    if (materialIndex >= 0 && m_textureCache[materialIndex].diffuseMap) {
        m_effect->SetTexture("g_DiffuseTex", m_textureCache[materialIndex].diffuseMap);
    }

    // Draw each shader pass
    for (uint i = 0; i < numPasses; i++) {
        m_effect->BeginPass(i);
        m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, primitiveCount);
        m_effect->EndPass();
    }

    // End
    m_effect->End();
}

void ModelViewer::drawText(uint p_x, uint p_y, const wxString& p_text)
{
    if (m_font.get()) {
        RECT outRect;
        ::SetRect(&outRect, p_x, p_y, p_x + 0x200, p_y + 0x14);
        m_font->DrawTextW(NULL, p_text.wchar_str(), -1, &outRect, DT_LEFT | DT_NOCLIP, 0xFFFFFFFF);
    }
}


void ModelViewer::updateMatrices()
{
    // All models are located at 0,0,0 with no rotation, so no world matrix is needed

    // View matrix
    auto viewMatrix = m_camera.calculateViewMatrix();

    // Projection matrix
    wxSize clientSize = this->GetClientSize();
    float aspectRatio = (static_cast<float>(clientSize.x) / static_cast<float>(clientSize.y));
    auto projMatrix   = ::XMMatrixPerspectiveFovLH((5.0f / 12.0f) * XM_PI, aspectRatio, 0.1f, 2000);

    // WorldViewProjection matrix
    XMFLOAT4X4 worldViewProjMatrix;
    ::XMStoreFloat4x4(&worldViewProjMatrix, ::XMMatrixMultiply(viewMatrix, projMatrix));

    m_effect->SetMatrix("g_WorldViewProjMatrix", reinterpret_cast<D3DXMATRIX*>(&worldViewProjMatrix));
}

IDirect3DTexture9* ModelViewer::loadTexture(uint p_fileId)
{
    auto entryNumber = this->datFile()->entryNumFromFileId(p_fileId);
    auto fileData    = this->datFile()->readEntry(entryNumber);

    // Bail if read failed
    if (fileData.GetSize() == 0) { return nullptr; }

    // Convert to image
    ANetFileType fileType;
    this->datFile()->identifyFileType(fileData.GetPointer(), fileData.GetSize(), fileType);
    auto reader = FileReader::readerForData(fileData, fileType);
    
    // Bail if not an image
    auto imgReader = dynamic_cast<ImageReader*>(reader);
    if (!imgReader) {
        deletePointer(reader);
        return nullptr;
    }

    // Convert to PNG and bail if invalid
    auto pngData = imgReader->convertData();
    if (pngData.GetSize() == 0) {
        deletePointer(reader);
        return nullptr;
    }

    // Finally, load texture from in-memory PNG.
    IDirect3DTexture9* texture = nullptr;
    ::D3DXCreateTextureFromFileInMemory(m_device.get(), pngData.GetPointer(), pngData.GetSize(), &texture);

    // Delete reader and return
    deletePointer(reader);
    return texture;
}

void ModelViewer::focus()
{
    float fov      = (5.0f / 12.0f) * XM_PI;
    uint meshCount = m_model.GetNumMeshes();
    
    if (!meshCount) { return; }

    // Calculate complete bounds
    Bounds bounds;
    for (uint i = 0; i < meshCount; i++) {
        auto& meshBounds = m_model.GetMesh(i).mBounds;

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
    auto minVector    = ::XMLoadFloat3(&min);
    auto maxVector    = ::XMLoadFloat3(&max);
    auto centerVector = ::XMVectorScale(::XMVectorAdd(minVector, maxVector), 0.5f);

    XMFLOAT3 center;
    ::XMStoreFloat3(&center, centerVector);

    // Update camera and re-render
    m_camera.setPivot(center);
    m_camera.setDistance(distance);
    this->Refresh(false);
}

void ModelViewer::onMotionEvt(wxMouseEvent& p_event)
{
    if (m_lastMousePos.x == std::numeric_limits<int>::min() && 
        m_lastMousePos.y == std::numeric_limits<int>::min()) 
    {
        m_lastMousePos = p_event.GetPosition();
    }

    // Yaw/Pitch
    if (p_event.LeftIsDown()) {
        float rotateSpeed = (XM_PI / 180.0f);   // 1 degree per pixel
        m_camera.addYaw(rotateSpeed * -(p_event.GetX() - m_lastMousePos.x));
        m_camera.addPitch(rotateSpeed * (p_event.GetY() - m_lastMousePos.y));
        this->Refresh(false);
    }

    // Pan
    if (p_event.MiddleIsDown()) {
        float xPan = -(p_event.GetX() - m_lastMousePos.x);
        float yPan = -(p_event.GetY() - m_lastMousePos.y);
        m_camera.pan(xPan, yPan);
        this->Refresh(false);
    }

    m_lastMousePos = p_event.GetPosition();
}

void ModelViewer::onMouseWheelEvt(wxMouseEvent& p_event)
{
    float zoomSteps = static_cast<float>(p_event.GetWheelRotation()) / static_cast<float>(p_event.GetWheelDelta());
    m_camera.multiplyDistance(-zoomSteps);
    this->Refresh(false);
}

void ModelViewer::onKeyDownEvt(wxKeyEvent& p_event)
{
    if (p_event.GetKeyCode() == 'F') {
        this->focus();
    }
}

}; // namespace gw2b
