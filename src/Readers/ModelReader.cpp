/** \file       Readers/ModelReader.cpp
 *  \brief      Contains the definition of the model reader class.
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
#include <wx/sstream.h>
#include <wx/txtstrm.h>
#include <sstream>
#include <new>

#include "ModelReader.h"

#include "PackFile.h"

namespace gw2b
{

enum FourCC
{
    FCC_GEOM    = 0x4d4f4547,
};

//----------------------------------------------------------------------------
//      ModelData
//----------------------------------------------------------------------------

ModelData::ModelData()
{
}

ModelData::ModelData(const ModelData& pOther)
{
    mMeshes.assign(pOther.mMeshes.begin(), pOther.mMeshes.end());
}

ModelData::~ModelData()
{
}

//----------------------------------------------------------------------------
//      Model
//----------------------------------------------------------------------------

Model::Model()
    : mData(new ModelData())
{
}

Model::Model(const Model& pOther)
    : mData(pOther.mData)
{
}

Model::~Model()
{
}

Model& Model::operator=(const Model& pOther)
{
    mData = pOther.mData;
    return *this;
}

uint Model::GetNumMeshes() const
{
    return mData->mMeshes.size();
}

Mesh& Model::GetMesh(uint pIndex)
{
    wxASSERT(pIndex < this->GetNumMeshes());
    return mData->mMeshes[pIndex];
}

const Mesh& Model::GetMesh(uint pIndex) const
{
    wxASSERT(pIndex < this->GetNumMeshes());
    return mData->mMeshes[pIndex];
}

Mesh& Model::AddMesh()
{
    this->UnShare();
    mData->mMeshes.push_back(Mesh());
    return mData->mMeshes[mData->mMeshes.size() - 1];
}

void Model::UnShare()   
{
    if (mData->GetRefCount() == 1) {
        return;
    }
    mData = new ModelData(*mData);
}

//----------------------------------------------------------------------------
//      ModelReader
//----------------------------------------------------------------------------

ModelReader::ModelReader(const Array<byte>& pData, ANetFileType pFileType)
    : FileReader(pData, pFileType)
{
}

ModelReader::~ModelReader()
{
}

Array<byte> ModelReader::ConvertData() const
{
    Model model = this->GetModel();
    std::ostringstream stream;

    // Note: wxWidgets only does locale-specific number formatting. This does
    // not work well with obj-files.
    stream.imbue(std::locale("C"));
    stream << "# " << model.GetNumMeshes() << " meshes" << std::endl;

    for (uint i = 0; i < model.GetNumMeshes(); i++){
        Mesh& mesh     = model.GetMesh(i);
        bool hasUV     = true;
        bool hasNormal = true;

        // Write header
        stream << std::endl << "# Mesh " << i << ": " << mesh.mVertices.GetSize() << " vertices, " << mesh.mTriangles.GetSize() << " triangles" << std::endl;
        stream << "g mesh" << i << std::endl;
        stream << "usemtl " << mesh.mMaterialName.c_str() << std::endl;

        // Write positions
        for (uint j = 0; j < mesh.mVertices.GetSize(); j++) {
            stream << "v " << mesh.mVertices[j].mPosition.x << ' ' << mesh.mVertices[j].mPosition.y << ' ' << mesh.mVertices[j].mPosition.z << std::endl;
        }

        // Write UVs
        for (uint j = 0; j < mesh.mVertices.GetSize(); j++) {
            // If the first vertex does not have UV, none of them do
            if (mesh.mVertices[j].mHasUV == 0) { hasUV = false; break; }
            stream << "vt " << mesh.mVertices[j].mUV[0].x << ' ' << mesh.mVertices[j].mUV[0].y << std::endl;
        }
        
        // Write normals
        for (uint j = 0; j < mesh.mVertices.GetSize(); j++) {
            // If the first vertex does not have a normal, none of them do
            if (mesh.mVertices[j].mHasNormal == 0) { hasNormal = false; break; }
            stream << "vn " << mesh.mVertices[j].mNormal.x << ' ' << mesh.mVertices[j].mNormal.y << ' ' << mesh.mVertices[j].mNormal.z << std::endl;
        }

        // Write faces
        for (uint j = 0; j < mesh.mTriangles.GetSize(); j++) {
            Triangle& triangle = mesh.mTriangles[j];
            
            stream << 'f';
            for (uint k = 0; k < 3; k++) { 
                uint index = triangle.mIndices[k] + 1;
                stream << ' ' << index;
                
                // UV reference
                if (hasUV) {
                    stream << '/' << index;
                } else if (hasNormal) {
                    stream << '/';
                }

                // Normal reference
                if (hasNormal) {
                    stream << '/' << index;
                }
            }
            stream << std::endl;
        }

        // newline before next mesh!
        stream << std::endl;
    }

    // Close stream
    stream.flush();
    std::string output = stream.str();
    stream.clear();

    // Convert string to byte array
    Array<byte> outputData(output.length());
    ::memcpy(outputData.GetPointer(), output.c_str(), output.length());

    return outputData;
}

Model ModelReader::GetModel() const
{
    Model newModel;

    // Bail if there is no data to read
    if (mData.GetSize() == 0) {
        return newModel;
    }

    // Populate the model
    PackFile packFile(mData);
    this->ReadGeometry(newModel, packFile);

    return newModel;
}

void ModelReader::ReadGeometry(Model& pModel, PackFile& pPackFile) const
{
    uint size;
    const byte* data = pPackFile.GetChunk(FCC_GEOM, size);

    // Bail if no data
    if (!data) {
        return;
    }

    // Read some interesting data
    const ANetPfChunkHeader* header   = reinterpret_cast<const ANetPfChunkHeader*>(data);
    uint32 meshCount                  = *reinterpret_cast<const uint32*>(&data[sizeof(*header)]);
    uint32 meshInfoOffsetTableOffset  = *reinterpret_cast<const uint32*>(&data[sizeof(*header) + 4]);
    const uint32* meshInfoOffsetTable = reinterpret_cast<const uint32*>(&data[sizeof(*header) + 4 + meshInfoOffsetTableOffset]);

    // Read all submeshes
    for (uint i = 0; i < meshCount; i++) {
        const byte* pos = reinterpret_cast<const byte*>(&meshInfoOffsetTable[i]);
        
        // Fetch mesh info
        pos += meshInfoOffsetTable[i];
        const ANetModelMeshInfo* meshInfo = reinterpret_cast<const ANetModelMeshInfo*>(pos);

        // Fetch buffer info
        pos  = reinterpret_cast<const byte*>(&meshInfo->mBufferInfoOffset);
        pos += meshInfo->mBufferInfoOffset;
        const ANetModelBufferInfo* bufferInfo = reinterpret_cast<const ANetModelBufferInfo*>(pos);

        // Add new mesh
        Mesh& mesh = pModel.AddMesh();
        // Material data
        mesh.mMaterialId = meshInfo->mMaterialId;
        pos  = reinterpret_cast<const byte*>(&meshInfo->mMaterialNameOffset);
        pos += meshInfo->mMaterialNameOffset;
        mesh.mMaterialName = wxString::FromUTF8(reinterpret_cast<const char*>(pos));
        // Vertex data
        if (bufferInfo->mVertexCount) {
            pos  = reinterpret_cast<const byte*>(&bufferInfo->mVertexBufferOffset);
            pos += bufferInfo->mVertexBufferOffset;
            this->ReadVertexBuffer(mesh.mVertices, pos, bufferInfo->mVertexCount, static_cast<ANetFlexibleVertexFormat>(bufferInfo->mVertexFormat));
        }
        // Index data
        if (bufferInfo->mIndexCount) {
            pos  = reinterpret_cast<const byte*>(&bufferInfo->mIndexBufferOffset);
            pos += bufferInfo->mIndexBufferOffset;
            this->ReadIndexBuffer(mesh.mTriangles, pos, bufferInfo->mIndexCount);
        }
    }
}

void ModelReader::ReadIndexBuffer(Array<Triangle>& pTriangles, const byte* pData, uint pIndexCount) const
{
    pTriangles.SetSize(pIndexCount / 3);
    ::memcpy(pTriangles.GetPointer(), pData, pTriangles.GetSize() * sizeof(Triangle));
}

void ModelReader::ReadVertexBuffer(Array<Vertex>& pVertices, const byte* pData, uint pVertexCount, ANetFlexibleVertexFormat pVertexFormat) const
{
    pVertices.SetSize(pVertexCount);
    uint vertexSize = this->GetVertexSize(pVertexFormat);

#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(pVertexCount); i++) {
        const byte* pos = &pData[i * vertexSize];

        // Init vertex
        Vertex& vertex    = pVertices[i];
        vertex.mHasColor  = 0;
        vertex.mHasNormal = 0;
        vertex.mHasUV     = 0;

        // Bit 0: Position
        if (pVertexFormat & ANFVF_Position) {
            ::memcpy(&vertex.mPosition, pos, sizeof(vertex.mPosition));
            pos += sizeof(vertex.mPosition);
        }
        // Bit 1: Weights
        if (pVertexFormat & ANFVF_Weights) {
            pos += 4;
        }
        // Bit 2: Group
        if (pVertexFormat & ANFVF_Group) {
            pos += 4;
        }
        // Bit 3: Normal
        if (pVertexFormat & ANFVF_Normal) {
            ::memcpy(&vertex.mNormal, pos, sizeof(vertex.mNormal));
            vertex.mHasNormal = 1;
            pos += sizeof(vertex.mNormal);
        }
        // Bit 4: Color
        if (pVertexFormat & ANFVF_Color) {
            vertex.mColor = *reinterpret_cast<const uint32*>(pos);
            vertex.mHasColor = 1;
            pos += sizeof(uint32);
        }
        // Bit 5: Tangent
        if (pVertexFormat & ANFVF_Tangent) {
            pos += sizeof(XMFLOAT3);
        }
        // Bit 6: Bitangent
        if (pVertexFormat & ANFVF_Bitangent) {
            pos += sizeof(XMFLOAT3);
        }
        // Bit 7: Tangent frame
        if (pVertexFormat & ANFVF_TangentFrame) {
            pos += sizeof(XMFLOAT3);
        }
        // Bit 8-15: 32-bit UV
        uint uvFlag = (pVertexFormat & ANFVF_UV32Mask) >> 8;
        if (uvFlag) {
            for (uint i = 0; i < 7; i++) {
                if (((uvFlag >> i) & 1) == 0) { continue; }
                if (vertex.mHasUV < 2) {
                    ::memcpy(&vertex.mUV[vertex.mHasUV++], pos, sizeof(vertex.mUV[0]));
                }
                pos += sizeof(vertex.mUV[0]);
            }
        }
        // Bit 16-23: 16-bit UV
        uvFlag = (pVertexFormat & ANFVF_UV16Mask) >> 16;
        if (uvFlag) {
            for (uint i = 0; i < 7; i++) {
                if (((uvFlag >> i) & 1) == 0) { continue; }
                if (vertex.mHasUV < 2) {
                    const half* uv = reinterpret_cast<const half*>(pos);
                    vertex.mUV[vertex.mHasUV].x = uv[0];
                    vertex.mUV[vertex.mHasUV].y = uv[1];
                    vertex.mHasUV++;
                }
                pos += sizeof(half) * 2;
            }
        }
        // Bit 24: Unknown 48-byte value
        if (pVertexFormat & ANFVF_Unknown1) {
            pos += 48;
        }
        // Bit 25: Unknown 4-byte value
        if (pVertexFormat & ANFVF_Unknown2) {
            pos += 4;
        }
        // Bit 26: Unknown 4-byte value
        if (pVertexFormat & ANFVF_Unknown3) {
            pos += 4;
        }
        // Bit 27: Unknown 16-byte value
        if (pVertexFormat & ANFVF_Unknown4) {
            pos += 16;
        }
        // Bit 28: Compressed position
        if (pVertexFormat & ANFVF_PositionCompressed) {
            vertex.mPosition.x = *reinterpret_cast<const half*>(pos + 0 * sizeof(half));
            vertex.mPosition.y = *reinterpret_cast<const half*>(pos + 1 * sizeof(half));
            vertex.mPosition.z = *reinterpret_cast<const half*>(pos + 2 * sizeof(half));
            pos += 3 * sizeof(half);
        }
        // Bit 29: Unknown 12-byte value
        if (pVertexFormat & ANFVF_Unknown5) {
            pos += 12;
        }
    }
}

uint ModelReader::GetVertexSize(ANetFlexibleVertexFormat pVertexFormat) const
{
    uint uvCount = 0;
    uint uvField = (pVertexFormat & ANFVF_UV32Mask) >> 0x08;
    for (uint i = 0; i < 7; i++) {
        if (((uvField >> i) & 1) != 0)
            uvCount++;
    }

    uvField = (pVertexFormat & ANFVF_UV16Mask) >> 0x10;
    uint uv16Count = 0;
    for (uint i = 0; i < 7; i++) {
        if (((uvField >> i) & 1) != 0)
            uv16Count++;
    }

    return ((pVertexFormat & ANFVF_Position) * 12)
        + ((pVertexFormat & ANFVF_Weights) * 2)
        + ((pVertexFormat & ANFVF_Group))
        + (((pVertexFormat & ANFVF_Normal) >> 3) * 12)
        + ((pVertexFormat & ANFVF_Color) >> 2)
        + (((pVertexFormat & ANFVF_Tangent) >> 5) * 12)
        + (((pVertexFormat & ANFVF_Bitangent) >> 6) * 12)
        + (((pVertexFormat & ANFVF_TangentFrame) >> 7) * 12)
        + (uvCount * 8)
        + (uv16Count * 4)
        + (((pVertexFormat & ANFVF_Unknown1) >> 24) * 48)
        + (((pVertexFormat & ANFVF_Unknown2) >> 25) * 4)
        + (((pVertexFormat & ANFVF_Unknown3) >> 26) * 4)
        + (((pVertexFormat & ANFVF_Unknown4) >> 27) * 16)
        + (((pVertexFormat & ANFVF_PositionCompressed) >> 28) * 6)
        + (((pVertexFormat & ANFVF_Unknown5) >> 29) * 12);
}

}; // namespace gw2b
