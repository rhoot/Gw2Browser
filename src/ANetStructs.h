/* \file       ANetStructs.h
*  \brief      Mainly contains structs used by ArenaNet in their files.
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

#pragma once

#ifndef ANETSTRUCTS_H_INCLUDED
#define ANETSTRUCTS_H_INCLUDED

#include "stdafx.h"

namespace gw2b {

	/** Contains the various (known) file formats in the dat. */
	enum ANetFileType {
		ANFT_Unknown,               /**< Unknown format. */

		// Texture types
		ANFT_TextureStart,          /**< Values in between this and ANFT_TextureEnd are texture types. */
		ANFT_ATEX,                  /**< ATEX texture, generic use. */
		ANFT_ATTX,                  /**< ATTX texture, used for terrain (in GW1). */
		ANFT_ATEC,                  /**< ATEC texture, unknown use. */
		ANFT_ATEP,                  /**< ATEP texture, used for maps. */
		ANFT_ATEU,                  /**< ATEU texture, used for UI. */
		ANFT_ATET,                  /**< ATET texture, unknown use. */
		ANFT_DDS,                   /**< DDS texture, not an ANet specific format. */
		ANFT_JPEG,					/**< JPEG Image, not an ANet specific format. */
		ANFT_TextureEnd,            /**< Values in between this and ANFT_TextureStart are texture types. */

		// Sound
		ANFT_SoundStart,			/**< Values in between this and ANFT_SoundEnd are sound types. */
		ANFT_Sound,                 /**< Sound file of unknown type. */
		ANFT_MP3,                   /**< MP3 file, probably with a custom header. */
		ANFT_OGG,                   /**< OGG file, probably with a custom header. */
		ANFT_SoundEnd,				/**< Values in between this and ANFT_SoundStart are sound types. */

		// PF
		ANFT_PF,                    /**< PF file of unknown type. */
		ANFT_Manifest,              /**< Manifest file. */
		ANFT_TextPackManifest,		/**< TextPack Manifest file. */
		ANFT_TextPackVoices,		/**< TextPack Voices file. */
		ANFT_Bank,                  /**< Soundbank file, contains other files. */
		ANFT_BankIndex,				/**< Soundbank files index */
		ANFT_Model,                 /**< Model file. */
		ANFT_DependencyTable,       /**< Dependency table. */
		ANFT_EULA,                  /**< EULA file. */
		ANFT_HavokCloth,            /**< Cloth physics properties. */
		ANFT_Map,                   /**< Map file. */
		ANFT_MapShadow,				/**< Map shadow file. */
		ANFT_PagedImageTable,		/**< Paged Image Table file. */
		ANFT_Material,              /**< Compiled DirectX 9 shader. */
		ANFT_Composite,				/**< Composite data. */
		ANFT_Cinematic,             /**< Cinematic data. */
		ANFT_Animation,				/**< Animation data. */
		ANFT_EmoteAnimation,		/**< Emote animation data. */
		ANFT_PortalManifest,		/**< Portal Manifest file. */
		ANFT_AudioScript,			/**< Audio script file. */

		// Binary
		ANFT_Binary,                /**< Binary file of unknown type. */
		ANFT_DLL,                   /**< DLL file. */
		ANFT_EXE,                   /**< Executable file. */

		// Misc
		ANFT_ABFF,                  /**< abff file, seem to be a legacy type from GW1. */
		ANFT_StringFile,            /**< Strings file. */
	};

	/** Compression flags that appear in the MFT entries. */
	enum ANetCompressionFlags {
		ANCF_Uncompressed = 0,    /**< File is uncompressed. */
		ANCF_Compressed = 8,    /**< File is compressed. */
	};

	/** Flags appearing in MFT entries based on their use. */
	enum ANetMftEntryFlags {
		ANMEF_None = 0,    /**< No flags set. */
		ANMEF_InUse = 1,    /**< Entry is in use. */
	};

	/** GW2 FVF fields. */
	enum ANetFlexibleVertexFormat {
		ANFVF_Position = 0x00000001,   /**< 12 bytes. Position as three 32-bit floats in the order x, y, z. */
		ANFVF_Weights = 0x00000002,   /**< 4 bytes. Contains bone weights. */
		ANFVF_Group = 0x00000004,   /**< 4 bytes. Related to bone weights. */
		ANFVF_Normal = 0x00000008,   /**< 12 bytes. Normal as three 32-bit floats in the order x, y, z. */
		ANFVF_Color = 0x00000010,   /**< 4 bytes. Vertex color. */
		ANFVF_Tangent = 0x00000020,   /**< 12 bytes. Tangent as three 32-bit floats in the order x, y, z. */
		ANFVF_Bitangent = 0x00000040,   /**< 12 bytes. Bitangent as three 32-bit floats in the order x, y, z. */
		ANFVF_TangentFrame = 0x00000080,   /**< 12 bytes. */
		ANFVF_UV32Mask = 0x0000ff00,   /**< 8 bytes for each set bit. Contains UV-coords as two 32-bit floats in the order u, v. */
		ANFVF_UV16Mask = 0x00ff0000,   /**< 4 bytes for each set bit. Contains UV-coords as two 16-bit floats in the order u, v. */
		ANFVF_Unknown1 = 0x01000000,   /**< 48 bytes. Unknown data. */
		ANFVF_Unknown2 = 0x02000000,   /**< 4 bytes. Unknown data. */
		ANFVF_Unknown3 = 0x04000000,   /**< 4 bytes. Unknown data. */
		ANFVF_Unknown4 = 0x08000000,   /**< 16 bytes. Unknown data. */
		ANFVF_PositionCompressed = 0x10000000,   /**< 6 bytes. Position as three 16-bit floats in the order x, y, z. */
		ANFVF_Unknown5 = 0x20000000,   /**< 12 bytes. Unknown data. */
	};

#pragma pack(push, 1)

	/** Gw2.dat file header. */
	struct ANetDatHeader {
		byte version;                   /**< Version of the .dat file format. */
		byte identifier[3];             /**< 0x41 0x4e 0x1a */
		uint32 headerSize;              /**< Size of this header. */
		uint32 unknownField1;
		uint32 chunkSize;               /**< Size of each chunk in the file. */
		uint32 cRC;                     /**< CRC of the 16 first bytes of the header. */
		uint32 unknownField2;
		uint64 mftOffset;               /**< Offset to the MFT, from the start of the file. */
		uint32 mftSize;                 /**< Size of the MFT, in bytes. */
		uint32 flags;
	};

	/** Gw2.dat file MFT header (entry 0). */
	struct ANetMftHeader {
		byte identifier[4];             /**< 'Mft' 0x1a */
		uint64 unknownField1;
		uint32 numEntries;              /**< Amount of entries in the MFT, including this. */
		uint64 unknownField2;
	};

	/** Gw2.dat file MFT entry. */
	struct ANetMftEntry {
		uint64 offset;                  /**< Location in the dat that the file is stored at. */
		uint32 size;                    /**< Uncompressed size of the file. */
		uint16 compressionFlag;         /**< Entry compression flags. See ANetCompressionFlags. */
		uint16 entryFlags;              /**< Entry flags. See ANetMftEntryFlags. */
		uint32 counter;                 /**< Was 'counter' in GW1, seems unused in GW1. */
		uint32 crc;                     /**< Was 'crc' in GW1, seems to have different usage in GW2. */
	};

	/** Gw2.dat fileId->mftEntry table entry. */
	struct ANetFileIdEntry {
		uint32 fileId;                  /**< File ID. */
		uint32 mftEntryIndex;           /**< Index of the file in the mft. */
	};

	/** ANet file reference data. */
	struct ANetFileReference {
		uint16 parts[3];                /**< Part1 is always above 0x100, Part2 is always between 0x100 and 0x101, Part3 is always 0x00 */
	};

	/** ATEX file header. */
	struct ANetAtexHeader {
		union {
			byte identifier[4];         /**< File identifier (FourCC). */
			uint32 identifierInteger;   /**< File identifier (FourCC), as integer. */
		};
		union {
			byte format[4];             /**< Format of the contained data. */
			uint32 formatInteger;       /**< Format of the contained data, as integer. */
		};
		uint16 width;                   /**< Width of the texture, in pixels. */
		uint16 height;                  /**< Height of the texture, in pixels. */
	};

	/** PF file header. */
	struct ANetPfHeader {
		byte identifier[2];             /**< Always 'PF'. */
		uint16 unknownField1;
		uint16 unknownField2;           /**< Must always be 0 according to the exe. */
		uint16 pkFileVersion;           /**< PF-version of this file (0xc). */

		union {
			byte type[4];               /**< Type of data contained in this PF file. */
			uint32 typeInteger;         /**< Type of data contained in this PF file, as integer for easy comparison. */
		};
	};

	/** PF file chunk header. */
	struct ANetPfChunkHeader {
		union {
			byte chunkType[4];          /**< Identifies the chunk type. */
			uint32 chunkTypeInteger;    /**< Identifies the chunk type, as integer for easy comparison. */
		};
		uint32 chunkDataSize;           /**< Total size of this chunk, excluding this field and mChunkType, but \e including the remaining fields. */
		uint16 chunkVersion;            /**< Version of this chunk. */
		uint16 chunkHeaderSize;         /**< Size of the chunk header. */
		uint32 offsetTableOffset;       /**< Offset to the offset table. */
	};

	/** MODL file, GEOM chunk mesh info data. */
	struct ANetModelMeshInfo {
		uint32 unknown1[5];             /**< Unknown fields. Seems to always be zero. */
		uint32 unknownData1Count;       /**< Amount of fields pointed to by mUnknownData1Offset. */
		uint32 unknownData1Offset;      /**< Offset to unknown data. */
		float unknownFloats[8];         /**< Eight unknown 32-bit float values. */
		uint32 unknownData2Count;       /**< Amount of fields pointed to by mUnknownData2Offset. Each field is 28 bytes. */
		uint32 unknownData2Offset;      /**< Offset to unknown data. Each field in the pointed-to data is 28 bytes. */
		int32 materialIndex;            /**< Index into the material data array. */
		uint32 materialNameOffset;      /**< Offset to null-terminated material name. */
		uint32 unknownData3Count;       /**< Amount of fields pointed to by mUnknownData3Offset. Each field is 8 bytes. */
		uint32 unknownData3Offset;      /**< Offset to unknown data. Each field in the pointed-to data is 8 bytes. */
		uint32 bufferInfoOffset;        /**< Offset to this mesh's buffer info. */
	};

	/** MODL file, GEOM chunk buffer info data. */
	struct ANetModelBufferInfo {
		uint32 vertexCount;             /**< Amount of vertices in this mesh. */
		uint32 vertexFormat;            /**< Vertex format used by this mesh. */
		uint32 vertexBufferSize;        /**< Total size of the vertex buffer. */
		uint32 vertexBufferOffset;      /**< Offset to the vertex buffer. */
		uint32 indexCount;              /**< Amount of indices in this mesh's LOD0. */
		uint32 indexBufferOffset;       /**< Offset to the index data used by LOD0. */
		uint32 lodLevelCount;           /**< Amount of additional LOD levels for this mesh. */
		uint32 lodLevelOffset;          /**< Offset to the first LOD level's data. */
		uint32 unknownDataCount;        /**< Amount of fields pointed to by mUnknownDataOffset. Each field is a uint32. */
		uint32 unknownDataOffset;       /**< Offset to unknown data. Each field in the pointed-to data is a uint32. */
	};

	/** MODL file, MODL chunk material array. */
	struct ANetModelMaterialArray {
		uint32 unknown1;
		uint32 unknown2;
		uint32 materialCount;
		int32 materialsOffset;
	};

	/** MODL file, MODL chunk material info data. */
	struct ANetModelMaterialInfo {
		uint32 unknown1[3];             /**< Unknown data. */
		int32 materialFileOffset;       /**< Offset to material file reference. */
		uint32 flags;                   /**< Mesh flags. */
		uint32 unknown2;                /**< Unknown data. */
		uint32 textureCount;            /**< Amount of texture references. */
		int32 texturesOffset;           /**< Offset to texture references. */
		uint32 vectorCount;             /**< Amount of vectors for use by the material. */
		int32 vectorsOffset;            /**< Offset to vector data. */
		uint32 hashCount;               /**< Amount of hashes pointed to by mHashesOffset. Only contains hashes used by vectors. */
		int32 hashesOffset;             /**< Offset to hashes. */
		uint32 unknown3Count;
		int32 unknown3Offset;
		uint32 unknown4Count;
		int32 unknown4Offset;
		uint32 unknown5;
	};

	/** MODL file, MODL chunk texture reference data. */
	struct ANetModelTextureReference {
		int32 offsetToFileReference;    /**< Offset to the texture file reference. */
		uint32 unknown1;                /**< Unknown data. */
		uint32 hash;                    /**< Hash used to associate the texture with a variable in the material. */
		uint32 unknown2;                /**< Could potentially be part of the hash, but doesn't seem like it. */
		byte unknown3[13];              /**< Unknown data. */
	};

#pragma pack(pop)

}; // namespace gw2mw

#endif // ANETSTRUCTS_H_INCLUDED
