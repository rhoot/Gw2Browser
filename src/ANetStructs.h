/** \file       ANetStructs.h
 *  \brief      Mainly contains structs used by ArenaNet in their files.
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

#ifndef ANETSTRUCTS_H_INCLUDED
#define ANETSTRUCTS_H_INCLUDED

#include "stdafx.h"

namespace gw2b
{

/** Contains the various (known) file formats in the dat. */
enum ANetFileType
{
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
    ANFT_TextureEnd,            /**< Values in between this and ANFT_TextureStart are texture types. */

    // Sound
    ANFT_Sound,                 /**< Sound file of unknown type. */
    ANFT_MP3,                   /**< MP3 file, probably with a custom header. */
    ANFT_OGG,                   /**< OGG file, probably with a custom header. */

    // PF
    ANFT_PF,                    /**< PF file of unknown type. */
    ANFT_Manifest,              /**< Manifest file. */
    ANFT_Bank,                  /**< Bank file, contains other files. */
    ANFT_Model,                 /**< Model file. */
    ANFT_DependencyTable,       /**< Dependency table. */
    ANFT_EULA,                  /**< EULA file. */
    ANFT_HavokCloth,            /**< Cloth physics properties. */
    ANFT_Map,                   /**< Map file. */
    ANFT_Material,              /**< Compiled DirectX 9 shader. */
    ANFT_Cinematic,             /**< Cinematic data. */

    // Binary
    ANFT_Binary,                /**< Binary file of unknown type. */
    ANFT_DLL,                   /**< DLL file. */
    ANFT_EXE,                   /**< Executable file. */

    // Misc
    ANFT_ABFF,                  /**< abff file, seem to be a legacy type from GW1. */
    ANFT_StringFile,            /**< Strings file. */
};

/** Compression flags that appear in the MFT entries. */
enum ANetCompressionFlags
{
    ANCF_Uncompressed   = 0,    /**< File is uncompressed. */
    ANCF_Compressed     = 8,    /**< File is compressed. */
};

/** Flags appearing in MFT entries based on their use. */
enum ANetMftEntryFlags
{
    ANMEF_None          = 0,    /**< No flags set. */
    ANMEF_InUse         = 1,    /**< Entry is in use. */
};

#pragma pack(push, 1)

/** Gw2.dat file header. */
struct ANetDatHeader
{
    byte mVersion;                  /**< Version of the .dat file format. */
    byte mIdentifier[3];            /**< 0x41 0x4e 0x1a */
    uint32 mHeaderSize;             /**< Size of this header. */
    uint32 mUnknownField1;          
    uint32 mChunkSize;              /**< Size of each chunk in the file. */
    uint32 mCRC;                    /**< CRC of the 16 first bytes of the header. */
    uint32 mUnknownField2;          
    uint64 mMftOffset;              /**< Offset to the MFT, from the start of the file. */
    uint32 mMftSize;                /**< Size of the MFT, in bytes. */
    uint32 mFlags;                  
};

/** Gw2.dat file MFT header (entry 0). */
struct ANetMftHeader
{
    byte mIdentifier[4];            /**< 'Mft' 0x1a */
    uint64 mUnknownField1;           
    uint32 mNumEntries;             /**< Amount of entries in the MFT, including this. */
    uint64 mUnknownField2;
};

/** Gw2.dat file MFT entry. */
struct ANetMftEntry
{
    uint64 mOffset;                 /**< Location in the dat that the file is stored at. */
    uint32 mSize;                   /**< Uncompressed size of the file. */
    uint16 mCompressionFlag;        /**< Entry compression flags. See ANetCompressionFlags. */
    uint16 mEntryFlags;             /**< Entry flags. See ANetMftEntryFlags. */
    uint32 mCounter;                /**< Was 'counter' in GW1, seems unused in GW1. */
    uint32 mCRC;                    /**< Was 'crc' in GW1, seems to have different usage in GW2. */
};

/** Gw2.dat fileId->mftEntry table entry. */
struct ANetFileIdEntry
{
    uint32 mFileId;                 /**< File ID. */
    uint32 mMftEntryIndex;          /**< Index of the file in the mft. */
};

/** ATEX file header. */
struct ANetAtexHeader
{
    union {
        byte mIdentifier[4];        /**< File identifier (FourCC). */
        uint32 mIdentifierInteger;  /**< File identifier (FourCC), as integer. */
    };
    union {
        byte mFormat[4];            /**< Format of the contained data. */
        uint32 mFormatInteger;      /**< Format of the contained data, as integer. */
    };
    uint16  mWidth;                 /**< Width of the texture, in pixels. */
    uint16  mHeight;                /**< Height of the texture, in pixels. */
};

/** PF file header. */
struct ANetPfHeader
{
    byte mIdentifier[2];            /**< Always 'PF'. */
    uint16 mUnknownField1;
    uint16 mUnknownField2;          /**< Must always be 0 according to the exe. */
    uint16 mPkFileVersion;          /**< PF-version of this file (0xc). */

    union {
        byte mType[4];              /**< Type of data contained in this PF file. */
        uint32 mTypeInteger;        /**< Type of data contained in this PF file, as integer for easy comparison. */
    };
};

/** PF file chunk header. **/
struct ANetPfChunkHeader
{
    union {
        byte mChunkType[4];         /**< Identifies the chunk type. */
        uint32 mChunkTypeInteger;   /**< Identifies the chunk type, as integer for easy comparison. */
    };
    uint32 mChunkDataSize;          /**< Total size of this chunk, excluding this field and mChunkType, but \e including the remaining fields. */
    uint16 mChunkVersion;           /**< Version of this chunk. */
    uint16 mChunkHeaderSize;        /**< Size of the chunk header. */
    uint32 mOffsetTableOffset;      /**< Offset to the offset table. */
};

#pragma pack(pop)

}; // namespace gw2mw

#endif // ANETSTRUCTS_H_INCLUDED
