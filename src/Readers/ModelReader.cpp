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
#include <vector>

#include "ModelReader.h"

#include "DatFile.h"
#include "PackFile.h"

namespace gw2b
{

enum FourCC
{
    FCC_GEOM    = 0x4d4f4547,
    FCC_MODL    = 0x4c444f4d,
};

//----------------------------------------------------------------------------
//      ModelData
//----------------------------------------------------------------------------

ModelData::ModelData()
{
}

ModelData::ModelData(const ModelData& p_other)
{
    meshes.assign(p_other.meshes.begin(), p_other.meshes.end());
    materialData.assign(p_other.materialData.begin(), p_other.materialData.end());
}

ModelData::~ModelData()
{
}

//----------------------------------------------------------------------------
//      Model
//----------------------------------------------------------------------------

Model::Model()
    : m_data(new ModelData())
{
}

Model::Model(const Model& p_other)
    : m_data(p_other.m_data)
{
}

Model::~Model()
{
}

Model& Model::operator=(const Model& p_other)
{
    m_data = p_other.m_data;
    return *this;
}

uint Model::numMeshes() const
{
    return m_data->meshes.size();
}

const Mesh& Model::mesh(uint p_index) const
{
    Assert(p_index < this->numMeshes());
    return m_data->meshes[p_index];
}

Mesh* Model::addMeshes(uint p_amount)
{
    this->unShare();

    uint oldSize = m_data->meshes.size();
    m_data->meshes.resize(oldSize + p_amount);

    return &(m_data->meshes[oldSize]);
}

uint Model::numMaterialData() const
{
    return m_data->materialData.size();
}

MaterialData& Model::materialData(uint p_index)
{
    Assert(p_index < this->numMaterialData());
    return m_data->materialData[p_index];
}

const MaterialData& Model::materialData(uint p_index) const
{
    Assert(p_index < this->numMaterialData());
    return m_data->materialData[p_index];
}

MaterialData& Model::addMaterialData()
{
    this->unShare();

    MaterialData newData;
    newData.diffuseMap = 0;
    newData.normalMap      = 0;
    m_data->materialData.push_back(newData);

    return m_data->materialData[m_data->materialData.size() - 1];
}

void Model::unShare()   
{
    if (m_data->GetRefCount() == 1) {
        return;
    }
    m_data = new ModelData(*m_data);
}

//----------------------------------------------------------------------------
//      ModelReader
//----------------------------------------------------------------------------

ModelReader::ModelReader(const Array<byte>& p_data, ANetFileType p_fileType)
    : FileReader(p_data, p_fileType)
{
}

ModelReader::~ModelReader()
{
}

Array<byte> ModelReader::convertData() const
{
    Model model = this->getModel();
    std::ostringstream stream;

    // Note: wxWidgets only does locale-specific number formatting. This does
    // not work well with obj-files.
    stream.imbue(std::locale("C"));
    stream << "# " << model.numMeshes() << " meshes" << std::endl;

    uint indexBase = 1;
    for (uint i = 0; i < model.numMeshes(); i++){
        const Mesh& mesh = model.mesh(i);
        bool hasUV       = true;
        bool hasNormal   = true;

        // Write header
        stream << std::endl << "# Mesh " << i << ": " << mesh.vertices.GetSize() << " vertices, " << mesh.triangles.GetSize() << " triangles" << std::endl;
        stream << "g mesh" << i << std::endl;
        stream << "usemtl " << mesh.materialName.c_str() << std::endl;

        // Write positions
        for (uint j = 0; j < mesh.vertices.GetSize(); j++) {
            stream << "v " << mesh.vertices[j].position.x << ' ' << mesh.vertices[j].position.y << ' ' << mesh.vertices[j].position.z << std::endl;
        }

        // Write UVs
        for (uint j = 0; j < mesh.vertices.GetSize(); j++) {
            // If the first vertex does not have UV, none of them do
            if (mesh.vertices[j].hasUV == 0) { hasUV = false; break; }
            stream << "vt " << mesh.vertices[j].uv[0].x << ' ' << mesh.vertices[j].uv[0].y << std::endl;
        }
        
        // Write normals
        for (uint j = 0; j < mesh.vertices.GetSize(); j++) {
            // If the first vertex does not have a normal, none of them do
            if (mesh.vertices[j].hasNormal == 0) { hasNormal = false; break; }
            stream << "vn " << mesh.vertices[j].normal.x << ' ' << mesh.vertices[j].normal.y << ' ' << mesh.vertices[j].normal.z << std::endl;
        }

        // Write faces
        for (uint j = 0; j < mesh.triangles.GetSize(); j++) {
            const Triangle& triangle = mesh.triangles[j];
            
            stream << 'f';
            for (uint k = 0; k < 3; k++) { 
                uint index = triangle.indices[k] + indexBase;
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
        indexBase += mesh.vertices.GetSize();
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

Model ModelReader::getModel() const
{
    Model newModel;

    // Bail if there is no data to read
    if (m_data.GetSize() == 0) {
        return newModel;
    }

    // Populate the model
    PackFile packFile(m_data);
    this->readGeometry(newModel, packFile);
    this->readMaterialData(newModel, packFile);

    return newModel;
}

void ModelReader::readGeometry(Model& p_model, PackFile& p_packFile) const
{
    uint size;
    const byte* data = p_packFile.findChunk(FCC_GEOM, size);

    // Bail if no data
    if (!data) {
        return;
    }

    // Read some interesting data
    auto header                      = reinterpret_cast<const ANetPfChunkHeader*>(data);
    uint32 meshCount                 = *reinterpret_cast<const uint32*>(&data[sizeof(*header)]);
    uint32 meshInfoOffsetTableOffset = *reinterpret_cast<const uint32*>(&data[sizeof(*header) + 4]);
    auto meshInfoOffsetTable         = reinterpret_cast<const uint32*>(&data[sizeof(*header) + 4 + meshInfoOffsetTableOffset]);

    // Create storage for submeshes now, so we can parallelize the below loop
    Mesh* meshes = p_model.addMeshes(meshCount);

    // Read all submeshes
#pragma omp parallel for shared(meshes)
    for (int i = 0; i < static_cast<int>(meshCount); i++) {
        auto pos = reinterpret_cast<const byte*>(&meshInfoOffsetTable[i]);
        
        // Fetch mesh info
        pos += meshInfoOffsetTable[i];
        auto meshInfo = reinterpret_cast<const ANetModelMeshInfo*>(pos);

        // Fetch buffer info
        pos  = reinterpret_cast<const byte*>(&meshInfo->bufferInfoOffset);
        pos += meshInfo->bufferInfoOffset;
        auto bufferInfo = reinterpret_cast<const ANetModelBufferInfo*>(pos);

        // Add new mesh
        Mesh& mesh = meshes[i];
        // Material data
        mesh.materialIndex = meshInfo->materialIndex;
        pos  = reinterpret_cast<const byte*>(&meshInfo->materialNameOffset);
        pos += meshInfo->materialNameOffset;
        mesh.materialName = wxString::FromUTF8(reinterpret_cast<const char*>(pos));
        // Vertex data
        if (bufferInfo->vertexCount) {
            pos  = reinterpret_cast<const byte*>(&bufferInfo->vertexBufferOffset);
            pos += bufferInfo->vertexBufferOffset;
            this->readVertexBuffer(mesh, pos, bufferInfo->vertexCount, static_cast<ANetFlexibleVertexFormat>(bufferInfo->vertexFormat));
        }
        // Index data
        if (bufferInfo->indexCount) {
            pos  = reinterpret_cast<const byte*>(&bufferInfo->indexBufferOffset);
            pos += bufferInfo->indexBufferOffset;
            this->readIndexBuffer(mesh, pos, bufferInfo->indexCount);
        }
    }
}

void ModelReader::readIndexBuffer(Mesh& p_mesh, const byte* p_data, uint p_indexCount) const
{
    p_mesh.triangles.SetSize(p_indexCount / 3);
    ::memcpy(p_mesh.triangles.GetPointer(), p_data, p_mesh.triangles.GetSize() * sizeof(Triangle));

    // Calculate bounds
    p_mesh.bounds.minX = std::numeric_limits<float>::max();
    p_mesh.bounds.minY = std::numeric_limits<float>::max();
    p_mesh.bounds.minZ = std::numeric_limits<float>::max();
    p_mesh.bounds.maxX = std::numeric_limits<float>::min();
    p_mesh.bounds.maxY = std::numeric_limits<float>::min();
    p_mesh.bounds.maxZ = std::numeric_limits<float>::min();

    const uint16* indices = reinterpret_cast<const uint16*>(p_data);
    for (uint i = 0; i < p_indexCount; i++) {
        auto& vertex = p_mesh.vertices[indices[i]];
        // X axis
        if (vertex.position.x < p_mesh.bounds.minX) { p_mesh.bounds.minX = vertex.position.x; }
        if (vertex.position.x > p_mesh.bounds.maxX) { p_mesh.bounds.maxX = vertex.position.x; }
        // Y axis
        if (vertex.position.y < p_mesh.bounds.minY) { p_mesh.bounds.minY = vertex.position.y; }
        if (vertex.position.y > p_mesh.bounds.maxY) { p_mesh.bounds.maxY = vertex.position.y; }
        // Z axis
        if (vertex.position.z < p_mesh.bounds.minZ) { p_mesh.bounds.minZ = vertex.position.z; }
        if (vertex.position.z > p_mesh.bounds.maxZ) { p_mesh.bounds.maxZ = vertex.position.z; }
    }
}

void ModelReader::readVertexBuffer(Mesh& p_mesh, const byte* p_data, uint p_vertexCount, ANetFlexibleVertexFormat p_vertexFormat) const
{
    p_mesh.vertices.SetSize(p_vertexCount);
    uint vertexSize = this->vertexSize(p_vertexFormat);

#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(p_vertexCount); i++) {
        auto pos = &p_data[i * vertexSize];

        // Init vertex
        Vertex& vertex    = p_mesh.vertices[i];
        vertex.hasColor  = 0;
        vertex.hasNormal = 0;
        vertex.hasUV     = 0;

        // Bit 0: Position
        if (p_vertexFormat & ANFVF_Position) {
            ::memcpy(&vertex.position, pos, sizeof(vertex.position));
            pos += sizeof(vertex.position);
        }
        // Bit 1: Weights
        if (p_vertexFormat & ANFVF_Weights) {
            pos += 4;
        }
        // Bit 2: Group
        if (p_vertexFormat & ANFVF_Group) {
            pos += 4;
        }
        // Bit 3: Normal
        if (p_vertexFormat & ANFVF_Normal) {
            ::memcpy(&vertex.normal, pos, sizeof(vertex.normal));
            vertex.hasNormal = 1;
            pos += sizeof(vertex.normal);
        }
        // Bit 4: Color
        if (p_vertexFormat & ANFVF_Color) {
            vertex.color = *reinterpret_cast<const uint32*>(pos);
            vertex.hasColor = 1;
            pos += sizeof(uint32);
        }
        // Bit 5: Tangent
        if (p_vertexFormat & ANFVF_Tangent) {
            pos += sizeof(XMFLOAT3);
        }
        // Bit 6: Bitangent
        if (p_vertexFormat & ANFVF_Bitangent) {
            pos += sizeof(XMFLOAT3);
        }
        // Bit 7: Tangent frame
        if (p_vertexFormat & ANFVF_TangentFrame) {
            pos += sizeof(XMFLOAT3);
        }
        // Bit 8-15: 32-bit UV
        uint uvFlag = (p_vertexFormat & ANFVF_UV32Mask) >> 8;
        if (uvFlag) {
            for (uint i = 0; i < 7; i++) {
                if (((uvFlag >> i) & 1) == 0) { continue; }
                if (vertex.hasUV < 2) {
                    ::memcpy(&vertex.uv[vertex.hasUV++], pos, sizeof(vertex.uv[0]));
                }
                pos += sizeof(vertex.uv[0]);
            }
        }
        // Bit 16-23: 16-bit UV
        uvFlag = (p_vertexFormat & ANFVF_UV16Mask) >> 16;
        if (uvFlag) {
            for (uint i = 0; i < 7; i++) {
                if (((uvFlag >> i) & 1) == 0) { continue; }
                if (vertex.hasUV < 2) {
                    const half* uv = reinterpret_cast<const half*>(pos);
                    vertex.uv[vertex.hasUV].x = uv[0];
                    vertex.uv[vertex.hasUV].y = uv[1];
                    vertex.hasUV++;
                }
                pos += sizeof(half) * 2;
            }
        }
        // Bit 24: Unknown 48-byte value
        if (p_vertexFormat & ANFVF_Unknown1) {
            pos += 48;
        }
        // Bit 25: Unknown 4-byte value
        if (p_vertexFormat & ANFVF_Unknown2) {
            pos += 4;
        }
        // Bit 26: Unknown 4-byte value
        if (p_vertexFormat & ANFVF_Unknown3) {
            pos += 4;
        }
        // Bit 27: Unknown 16-byte value
        if (p_vertexFormat & ANFVF_Unknown4) {
            pos += 16;
        }
        // Bit 28: Compressed position
        if (p_vertexFormat & ANFVF_PositionCompressed) {
            vertex.position.x = *reinterpret_cast<const half*>(pos + 0 * sizeof(half));
            vertex.position.y = *reinterpret_cast<const half*>(pos + 1 * sizeof(half));
            vertex.position.z = *reinterpret_cast<const half*>(pos + 2 * sizeof(half));
            pos += 3 * sizeof(half);
        }
        // Bit 29: Unknown 12-byte value
        if (p_vertexFormat & ANFVF_Unknown5) {
            pos += 12;
        }
    }
}

uint ModelReader::vertexSize(ANetFlexibleVertexFormat p_vertexFormat) const
{
    uint uvCount = 0;
    uint uvField = (p_vertexFormat & ANFVF_UV32Mask) >> 0x08;
    for (uint i = 0; i < 7; i++) {
        if (((uvField >> i) & 1) != 0)
            uvCount++;
    }

    uvField = (p_vertexFormat & ANFVF_UV16Mask) >> 0x10;
    uint uv16Count = 0;
    for (uint i = 0; i < 7; i++) {
        if (((uvField >> i) & 1) != 0)
            uv16Count++;
    }

    return ((p_vertexFormat & ANFVF_Position) * 12)
        + ((p_vertexFormat & ANFVF_Weights) * 2)
        + ((p_vertexFormat & ANFVF_Group))
        + (((p_vertexFormat & ANFVF_Normal) >> 3) * 12)
        + ((p_vertexFormat & ANFVF_Color) >> 2)
        + (((p_vertexFormat & ANFVF_Tangent) >> 5) * 12)
        + (((p_vertexFormat & ANFVF_Bitangent) >> 6) * 12)
        + (((p_vertexFormat & ANFVF_TangentFrame) >> 7) * 12)
        + (uvCount * 8)
        + (uv16Count * 4)
        + (((p_vertexFormat & ANFVF_Unknown1) >> 24) * 48)
        + (((p_vertexFormat & ANFVF_Unknown2) >> 25) * 4)
        + (((p_vertexFormat & ANFVF_Unknown3) >> 26) * 4)
        + (((p_vertexFormat & ANFVF_Unknown4) >> 27) * 16)
        + (((p_vertexFormat & ANFVF_PositionCompressed) >> 28) * 6)
        + (((p_vertexFormat & ANFVF_Unknown5) >> 29) * 12);
}

void ModelReader::readMaterialData(Model& p_model, PackFile& p_packFile) const
{
    uint size;
    const byte* data = p_packFile.findChunk(FCC_MODL, size);

    // Bail if no data
    if (!data) {
        return;
    }

    // Read some necessary data
    const ANetPfChunkHeader* header                 = reinterpret_cast<const ANetPfChunkHeader*>(data);
    uint32 numMaterialInfo                          = *reinterpret_cast<const uint32*>(&data[sizeof(*header)]);
    uint32 materialInfoOffset                       = *reinterpret_cast<const uint32*>(&data[sizeof(*header) + 4]);
    const ANetModelMaterialArray* materialInfoArray = reinterpret_cast<const ANetModelMaterialArray*>(&data[sizeof(*header) + 4 + materialInfoOffset]);

    // Loop through each material info
    for (uint i = 0; i < numMaterialInfo; i++) {
        // Bail if no offset or count
        if (!materialInfoArray[i].materialCount || !materialInfoArray[i].materialsOffset) { continue; }

        // Read the offset table for this set of materials
        const byte* pos = materialInfoArray[i].materialsOffset + reinterpret_cast<const byte*>(&materialInfoArray[i].materialsOffset);
        const int32* offsetTable = reinterpret_cast<const int32*>(pos);

        // Loop through each material in these material infos
        for (uint j = 0; j < materialInfoArray->materialCount; j++) {
            MaterialData& data = (p_model.numMaterialData() <= j ? p_model.addMaterialData() : p_model.materialData(j));

            // Bail if offset is nullptr
            if (offsetTable[j] == 0) { continue; }

            // Bail if this material index already has data
            if (data.diffuseMap && data.normalMap) { continue; }

            // Read material info
            const byte* pos = offsetTable[j] + reinterpret_cast<const byte*>(&offsetTable[j]);
            const ANetModelMaterialInfo* materialInfo = reinterpret_cast<const ANetModelMaterialInfo*>(pos);

            // We are *only* interested in textures
            if (materialInfo->textureCount == 0) { continue; }
            pos = materialInfo->texturesOffset + reinterpret_cast<const byte*>(&materialInfo->texturesOffset);
            const ANetModelTextureReference* textures = reinterpret_cast<const ANetModelTextureReference*>(pos);

            // Out of these, we only care about the diffuse and normal maps
            for (uint t = 0; t < materialInfo->textureCount; t++) {
                // Get file reference
                pos = textures[t].offsetToFileReference + reinterpret_cast<const byte*>(&textures[t].offsetToFileReference);
                const ANetFileReference* fileReference = reinterpret_cast<const ANetFileReference*>(pos);

                // Diffuse?
                if (textures[t].hash == 0x67531924) {
                    data.diffuseMap = DatFile::fileIdFromFileReference(*fileReference);
                }
                // Normal?
                else if (textures[t].hash == 0x1816C9EE) {
                    data.normalMap = DatFile::fileIdFromFileReference(*fileReference);
                }
            }
        }
    }
}

}; // namespace gw2b
