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
    XMFLOAT3 mPosition;
    XMFLOAT3 mNormal;
    uint32 mColor;
    XMFLOAT2 mUV[2];
    byte mHasNormal   : 1;
    byte mHasColor    : 1;
    byte mHasUV       : 2;
};

union Triangle
{
    struct {
        uint16 mIndex1;
        uint16 mIndex2;
        uint16 mIndex3;
    };
    uint16 mIndices[3];
};

struct Bounds
{
    float mMinX;
    float mMinY;
    float mMinZ;
    float mMaxX;
    float mMaxY;
    float mMaxZ;
};

#pragma pack(pop)

struct Mesh
{
    Array<Vertex>   mVertices;
    Array<Triangle> mTriangles;
    wxString        mMaterialName;
    int             mMaterialIndex;
    Bounds          mBounds;
};

struct MaterialData
{
    uint32 mDiffuseTexture;
    uint32 mNormalMap;
};

class ModelData : public wxRefCounter
{
public:
    std::vector<Mesh> mMeshes;
    std::vector<MaterialData> mMaterialData;
public:
    ModelData();
    ModelData(const ModelData& pOther);
    virtual ~ModelData();
};

class Model
{
    wxObjectDataPtr<ModelData>  mData;
public:
    Model();
    Model(const Model& pOther);
    ~Model();

    Model& operator=(const Model& pOther);

    // Submeshes
    uint GetNumMeshes() const;
    const Mesh& GetMesh(uint pIndex) const;
    Mesh& AddMesh();

    // Material data
    uint GetNumMaterialData() const;
    MaterialData& GetMaterialData(uint pIndex);
    const MaterialData& GetMaterialData(uint pIndex) const;
    MaterialData& AddMaterialData();
private:
    void UnShare();
};

class ModelReader : public FileReader
{
public:
    /** Constructor.
     *  \param[in]  pData       Data to be handled by this reader.
     *  \param[in]  pFileType   File type of the given data. */
    ModelReader(const Array<byte>& pData, ANetFileType pFileType);
    /** Destructor. Clears all data. */
    virtual ~ModelReader();

    /** Gets the type of data contained in this file. Not to be confused with
     *  file type.
     *  \return DataType    type of data. */
    virtual DataType GetDataType() const        { return DT_Model; }
    /** Gets an appropriate file extension for the contents of this reader.
     *  \return wxString    File extension. */
    virtual const wxChar* GetExtension() const  { return wxT(".obj"); }
    /** Converts the data associated with this file into OBJ.
     *  \return Array<byte> converted data. */
    virtual Array<byte> ConvertData() const;
    /** Gets the model represented by this data.
     *  \return Model   model. */
    Model GetModel() const;

private:
    void ReadGeometry(Model& pModel, PackFile& pPackFile) const;
    void ReadVertexBuffer(Mesh& pMesh, const byte* pData, uint pVertexCount, ANetFlexibleVertexFormat pVertexFormat) const;
    void ReadIndexBuffer(Mesh& pMesh, const byte* pData, uint pIndexCount) const;
    uint GetVertexSize(ANetFlexibleVertexFormat pVertexFormat) const;
    void ReadMaterialData(Model& pModel, PackFile& pPackFile) const;
}; // class ModelReader

}; // namespace gw2b

#endif // READERS_MODELREADER_H_INCLUDED
