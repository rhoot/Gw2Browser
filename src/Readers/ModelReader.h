/** \file       Readers/ModelReader.h
 *  \brief      Contains the declaration of the model reader class.
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

#ifndef READERS_MODELREADER_H_INCLUDED
#define READERS_MODELREADER_H_INCLUDED

#include <vector>

#include "FileReader.h"
#include "ANetStructs.h"

namespace gw2b
{
class PackFile;

#pragma pack(push, 1)

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    uint32 color;
    XMFLOAT2 uv[2];
};

union Triangle
{
    struct {
        uint16 index1;
        uint16 index2;
        uint16 index3;
    };
    uint16 indices[3];
};

struct Bounds
{
    XMFLOAT3 min;
    XMFLOAT3 max;

    Bounds& operator+=(const Bounds& p_other) 
    {
        min.x = wxMin(min.x, p_other.min.x);
        min.y = wxMin(min.y, p_other.min.y);
        min.z = wxMin(min.z, p_other.min.z);
        max.x = wxMax(max.x, p_other.max.x);
        max.y = wxMax(max.y, p_other.max.y);
        max.z = wxMax(max.z, p_other.max.z);
        return *this;
    }

    XMFLOAT3 center() const
    {
        XMVECTOR min = ::XMLoadFloat3(&this->min);
        XMVECTOR max = ::XMLoadFloat3(&this->max);
        XMVECTOR center = ::XMVectorLerp(min, max, 0.5f);
        XMFLOAT3 retval;
        ::XMStoreFloat3(&retval, center);
        return retval;
    }

    XMFLOAT3 size() const
    {
        XMVECTOR min  = ::XMLoadFloat3(&this->min);
        XMVECTOR max  = ::XMLoadFloat3(&this->max);
        XMVECTOR size = ::XMVectorSubtract(max, min);
        XMFLOAT3 retval;
        ::XMStoreFloat3(&retval, size);
        return retval;
    }
};

#pragma pack(pop)

struct Mesh
{
    Array<Vertex>   vertices;
    Array<Triangle> triangles;
    wxString        materialName;
    int             materialIndex;
    Bounds          bounds;
    byte            hasNormal  : 1;
    byte            hasColor   : 1;
    byte            hasUV      : 2;
};

struct MaterialData
{
    uint32 diffuseMap;
    uint32 normalMap;
};

class ModelData : public wxRefCounter
{
public:
    std::vector<Mesh>         meshes;
    std::vector<MaterialData> materialData;
public:
    ModelData();
    ModelData(const ModelData& p_other);
    virtual ~ModelData();
};

class Model
{
    wxObjectDataPtr<ModelData>  m_data;
public:
    Model();
    Model(const Model& p_other);
    ~Model();

    Model& operator=(const Model& p_other);

    // Submeshes
    uint numMeshes() const;
    const Mesh& mesh(uint p_index) const;
    Mesh* addMeshes(uint p_amount);

    // Material data
    uint numMaterialData() const;
    MaterialData& materialData(uint p_index);
    const MaterialData& materialData(uint p_index) const;
    MaterialData* addMaterialData(uint p_amount);

    // helpers
    Bounds bounds() const;
private:
    void unShare();
};

class ModelReader : public FileReader
{
public:
    /** Constructor.
     *  \param[in]  pData       Data to be handled by this reader.
     *  \param[in]  pFileType   File type of the given data. */
    ModelReader(const Array<byte>& p_data, ANetFileType p_fileType);
    /** Destructor. Clears all data. */
    virtual ~ModelReader();

    /** Gets the type of data contained in this file. Not to be confused with
     *  file type.
     *  \return DataType    type of data. */
    virtual DataType dataType() const override       { return DT_Model; }
    /** Gets an appropriate file extension for the contents of this reader.
     *  \return wxString    File extension. */
    virtual const wxChar* extension() const override { return wxT(".obj"); }
    /** Converts the data associated with this file into OBJ.
     *  \return Array<byte> converted data. */
    virtual Array<byte> convertData() const override;
    /** Gets the model represented by this data.
     *  \return Model   model. */
    Model getModel() const;

private:
    void readGeometry(Model& p_model, PackFile& p_packFile) const;
    void readVertexBuffer(Mesh& p_mesh, const byte* p_data, uint p_vertexCount, ANetFlexibleVertexFormat p_vertexFormat) const;
    void readIndexBuffer(Mesh& p_mesh, const byte* p_data, uint p_indexCount) const;
    uint vertexSize(ANetFlexibleVertexFormat p_vertexFormat) const;
    void readMaterialData(Model& p_model, PackFile& p_packFile) const;
}; // class ModelReader

}; // namespace gw2b

#endif // READERS_MODELREADER_H_INCLUDED
