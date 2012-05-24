/** \file       DatFile.cpp
 *  \brief      Contains declaration of the class representing the .dat file. 
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
#include "DatFile.h"
#include "FileReader.h"

namespace gw2b
{

enum FourCC
{
    // Offset 0
    FCC_abff = 0x66666261,
    FCC_ATEX = 0x58455441,
    FCC_ATTX = 0x58545441,
    FCC_ATEC = 0x43455441,
    FCC_ATEP = 0x50455441,
    FCC_ATEU = 0x55455441,
    FCC_ATET = 0x54455441,
    FCC_DDS  = 0x20534444,
    FCC_strs = 0x73727473,
    FCC_asnd = 0x646e7361,
    FCC_CINP = 0x504e4943,

    // PF FourCC
    FCC_ARMF = 0x464d5241,
    FCC_ASND = 0x444e5341,
    FCC_ABNK = 0x4b4e4241,
    FCC_MODL = 0x4c444f4d,
    FCC_DEPS = 0x53504544,
    FCC_EULA = 0x616c7565,
    FCC_hvkC = 0x436b7668,
    FCC_mapc = 0x6370616d,
    FCC_AMAT = 0x54414d41,

    // Sound id's
    FCC_OggS = 0x5367674f,

    // Not quite FourCC
    FCC_MZ   = 0x5a4d,
    FCC_PF   = 0x4650,
};

DatFile::DatFile()
    : mLastReadEntry(-1)
{
    ::memset(&mDatHead, 0, sizeof(mDatHead));
    ::memset(&mMftHead, 0, sizeof(mMftHead));
}

DatFile::DatFile(const wxString& pFilename)
    : mLastReadEntry(-1)
{
    ::memset(&mDatHead, 0, sizeof(mDatHead));
    ::memset(&mMftHead, 0, sizeof(mMftHead));
    this->Open(pFilename);
}

DatFile::~DatFile()
{
    this->Close();
}

bool DatFile::Open(const wxString& pFilename)
{
    this->Close();

    while (true) {
        // Open file
        mFile.Open(pFilename);
        if (!mFile.IsOpened()) { break; }

        // Read header
        if (mFile.Length() < sizeof(mDatHead)) { break; }
        mFile.Read(&mDatHead, sizeof(mDatHead));

        // Read MFT Header
        if ((uint64)mFile.Length() < mDatHead.mMftOffset + mDatHead.mMftSize) { break; }
        mFile.Seek(mDatHead.mMftOffset, wxFromStart);
        mFile.Read(&mMftHead, sizeof(mMftHead));

        // Read all of the MFT
        if (mDatHead.mMftSize != mMftHead.mNumEntries * sizeof(ANetMftEntry)) { break; }
        if (mDatHead.mMftSize % sizeof(ANetMftEntry)) { break; }

        mMftEntries.SetSize(mDatHead.mMftSize / sizeof(ANetMftEntry));
        mFile.Seek(mDatHead.mMftOffset, wxFromStart);
        mFile.Read(mMftEntries.GetPointer(), mDatHead.mMftSize);

        // Read the file id entry table
        if ((uint64)mFile.Length() < mMftEntries[2].mOffset + mMftEntries[2].mSize) { break; }
        if (mMftEntries[2].mSize % sizeof(ANetFileIdEntry)) { break; }

        uint numFileIdEntries = mMftEntries[2].mSize / sizeof(ANetFileIdEntry);
        Array<ANetFileIdEntry> fileIdTable(numFileIdEntries);
        mFile.Seek(mMftEntries[2].mOffset, wxFromStart);
        mFile.Read(fileIdTable.GetPointer(), mMftEntries[2].mSize);

        // Extract the entry -> base/file ID tables
        mEntryToId.SetSize(mMftEntries.GetSize());
        ::memset(mEntryToId.GetPointer(), 0, mEntryToId.GetByteSize());

        for (uint i = 0; i < numFileIdEntries; i++) {
            if (fileIdTable[i].mFileId == 0 && fileIdTable[i].mMftEntryIndex == 0) {
                continue;
            }

            uint entryIndex = fileIdTable[i].mMftEntryIndex;
            IdEntry& entry  = mEntryToId[entryIndex];

            if (entry.mBaseId == 0) {
                entry.mBaseId = fileIdTable[i].mFileId;
            } else if (entry.mFileId == 0) {
                entry.mFileId = fileIdTable[i].mFileId;
            }

            if (entry.mBaseId > 0 && entry.mFileId > 0) {
                if (entry.mBaseId > entry.mFileId) {
                    Swap(entry.mBaseId, entry.mFileId);
                }
            }
        }

        // Success!
        return true;
    }

    this->Close();
    return false;
}

bool DatFile::IsOpen() const
{
    return mFile.IsOpened();
}

void DatFile::Close()
{
    // Clear input buffer and lookup tables
    mInputBuffer.Clear();
    mEntryToId.Clear();

    // Clear PODs
    ::memset(&mDatHead, 0, sizeof(mDatHead));
    ::memset(&mMftHead, 0, sizeof(mMftHead));

    // Remove MFT entries and close the file
    mMftEntries.Clear();
    mFile.Close();
}

uint DatFile::GetEntrySize(uint pEntryNum)
{
    if (!IsOpen()) { return std::numeric_limits<uint>::max(); }
    if (pEntryNum >= mMftEntries.GetSize()) { return std::numeric_limits<uint>::max(); }
    
    const ANetMftEntry& entry = mMftEntries[pEntryNum];

    // If the entry is compressed we need to read the uncompressed size from the .dat
    if (entry.mCompressionFlag & ANCF_Compressed) {
        uint32 uncompressedSize = 0;
        mFile.Seek(entry.mOffset + 4, wxFromStart);
        mFile.Read(&uncompressedSize, sizeof(uncompressedSize));
        return uncompressedSize;
    } 
        
    return entry.mSize;
}

uint DatFile::GetFileSize(uint pFileNum) {
    return this->GetEntrySize(pFileNum + MFT_FILE_OFFSET);
}

uint DatFile::GetEntryNumFromFileId(uint pFileId) const
{
    if (!IsOpen()) { return std::numeric_limits<uint>::max(); }
    for (uint i = 0; i < mEntryToId.GetSize(); i++) {
        uint fileId = (mEntryToId[i].mFileId == 0 ? mEntryToId[i].mBaseId : mEntryToId[i].mFileId);
        if (fileId == pFileId) {
            return i;
        }
    }
    return std::numeric_limits<uint>::max();
}

uint DatFile::GetFileIdFromEntryNum(uint pEntryNum) const
{
    if (!IsOpen()) { return std::numeric_limits<uint>::max(); }
    if (pEntryNum >= mEntryToId.GetSize()) { return std::numeric_limits<uint>::max(); }
    return (mEntryToId[pEntryNum].mFileId == 0 ? mEntryToId[pEntryNum].mBaseId : mEntryToId[pEntryNum].mFileId);
}

uint DatFile::GetFileIdFromFileNum(uint pFileNum) const
{
    return this->GetFileIdFromEntryNum(pFileNum + MFT_FILE_OFFSET);
}

uint DatFile::GetEntryNumFromBaseId(uint pBaseId) const
{
    if (!IsOpen()) { return std::numeric_limits<uint>::max(); }
    for (uint i = 0; i < mEntryToId.GetSize(); i++) {
        if (mEntryToId[i].mBaseId == pBaseId) {
            return i;
        }
    }
    return std::numeric_limits<uint>::max();
}

uint DatFile::GetBaseIdFromEntryNum(uint pEntryNum) const
{
    if (!IsOpen()) { return std::numeric_limits<uint>::max(); }
    if (pEntryNum >= mEntryToId.GetSize()) { return std::numeric_limits<uint>::max(); }
    return mEntryToId[pEntryNum].mBaseId;
}

uint DatFile::GetBaseIdFromFileNum(uint pFileNum) const
{
    return this->GetBaseIdFromEntryNum(pFileNum + MFT_FILE_OFFSET);
}

uint DatFile::PeekFile(uint pFileNum, uint pPeekSize, byte* poBuffer)
{
    return this->PeekEntry(pFileNum + MFT_FILE_OFFSET, pPeekSize, poBuffer);
}

uint DatFile::PeekEntry(uint pEntryNum, uint pPeekSize, byte* poBuffer)
{
    Ensure::NotNull(poBuffer);
    uint inputSize;

    // Return instantly if size is 0, or if the file isn't open
    if (pPeekSize == 0 || !this->IsOpen()) {
        return 0;
    }

    // If this was the last entry we read, there's no need to re-read it. The
    // input buffer should already contain the full file.
    if (mLastReadEntry != pEntryNum) {
        // Perform some checks
        bool entryIsInRange = mMftHead.mNumEntries > (uint)pEntryNum;
        if (!entryIsInRange) { return 0; }

        bool entryIsInUse      = (mMftEntries[pEntryNum].mEntryFlags & ANMEF_InUse);
        bool fileIsLargeEnough = (uint64)mFile.Length() >= mMftEntries[pEntryNum].mOffset + mMftEntries[pEntryNum].mSize;
        if (!entryIsInUse || !fileIsLargeEnough) { return 0; }

        // Make sure we can re-use the input buffer
        inputSize = mMftEntries[pEntryNum].mSize;
        if (mInputBuffer.GetSize() < inputSize) {
            mInputBuffer.SetSize(inputSize);
        }

        // Read the file data
        mFile.Seek(mMftEntries[pEntryNum].mOffset, wxFromStart);
        mFile.Read(mInputBuffer.GetPointer(), inputSize);
        mLastReadEntry = pEntryNum;
    } else {
        inputSize = mMftEntries[pEntryNum].mSize;
    }

    // If the file is compressed we need to uncompress it
    if (mMftEntries[pEntryNum].mCompressionFlag) {
        uint32 outputSize = pPeekSize;
        try {
            gw2dt::compression::inflateDatFileBuffer(inputSize, mInputBuffer.GetPointer(), outputSize, poBuffer);
            return outputSize;
        } catch (std::exception&) {
            return 0;
        }
    } else {
        uint size = wxMin(pPeekSize, inputSize);
        ::memcpy(poBuffer, mInputBuffer.GetPointer(), size);
        return size;
    }
}

Array<byte> DatFile::PeekFile(uint pFileNum, uint pPeekSize)
{
    return this->PeekEntry(pFileNum + MFT_FILE_OFFSET, pPeekSize);
}

Array<byte> DatFile::PeekEntry(uint pEntryNum, uint pPeekSize)
{
    Array<byte> output = Array<byte>(pPeekSize);
    uint readBytes = this->PeekEntry(pEntryNum, pPeekSize, output.GetPointer());

    if (readBytes == 0) {
        return Array<byte>();
    }

    return output;
}

uint DatFile::ReadFile(uint pFileNum, byte* poBuffer)
{
    return this->ReadEntry(pFileNum + MFT_FILE_OFFSET, poBuffer);
}

uint DatFile::ReadEntry(uint pEntryNum, byte* poBuffer)
{
    uint size = this->GetEntrySize(pEntryNum);

    if (size != std::numeric_limits<uint>::max()) {
        return this->PeekEntry(pEntryNum, size, poBuffer);
    } 

    return 0;
}

Array<byte> DatFile::ReadFile(uint pFileNum)
{
    return this->ReadEntry(pFileNum + MFT_FILE_OFFSET);
}

Array<byte> DatFile::ReadEntry(uint pEntryNum)
{
    uint size = this->GetEntrySize(pEntryNum);
    Array<byte> output = NULL;

    if (size != std::numeric_limits<uint>::max()) {
        output.SetSize(size);
        uint readBytes = this->PeekEntry(pEntryNum, size, output.GetPointer());

        if (readBytes > 0) {
            return output;
        }
    }
    
    return Array<byte>();
}

DatFile::IdentificationResult DatFile::IdentifyFileType(byte* pData, uint pSize, ANetFileType& pFileType)
{
    if (pSize < 4) { pFileType = ANFT_Unknown; return IR_Failure; }

    // start with fourcc
    uint32 fourcc = *(uint32*)pData;
    pFileType = ANFT_Unknown;

    // abff files need offsetting
    if (fourcc == FCC_abff) {
        pFileType = ANFT_ABFF;

        if (pSize >= 0x44) {
            pData   += 0x40;
            pSize   -= 0x40;
            fourcc = *(uint32*)pData;
        } else {
            return IR_NotEnoughData;
        }
    }

    switch (fourcc) {
    case FCC_ATEX:
        pFileType = ANFT_ATEX;
        break;
    case FCC_ATTX:
        pFileType = ANFT_ATTX;
        break;
    case FCC_ATEC:
        pFileType = ANFT_ATEC;
        break;
    case FCC_ATEP:
        pFileType = ANFT_ATEP;
        break;
    case FCC_ATEU:
        pFileType = ANFT_ATEU;
        break;
    case FCC_ATET:
        pFileType = ANFT_ATET;
        break;
    case FCC_DDS:
        pFileType = ANFT_DDS;
        break;
    case FCC_strs:
        pFileType = ANFT_StringFile;
        break;
    case FCC_asnd:
        pFileType = ANFT_Sound;
        break;
    case FCC_CINP:
        pFileType = ANFT_Cinematic;
        break;
    }

    // Identify binary files
    if ((fourcc & 0xffff) == FCC_MZ) {
        pFileType = ANFT_Binary;

        if (pSize >= 0x40) {
            uint32 peOffset = *(uint32*)(pData + 0x3c);

            if (pSize >= (peOffset + 0x18)) {
                uint16 flags = *(uint16*)(pData + peOffset + 0x16);
                pFileType = (flags & 0x2000) ? ANFT_DLL : ANFT_EXE;
            } else {
                return IR_NotEnoughData;
            }
        } else {
            return IR_NotEnoughData;
        }
    }

    // Identify PF files
    if ((fourcc & 0xffff) == FCC_PF) {
        pFileType = ANFT_PF;

        if (pSize >= 12) {
            fourcc   = *(uint32*)(pData + 8);
        } else {
            return IR_NotEnoughData;
        }

        switch (fourcc) {
        case FCC_ARMF:
            pFileType = ANFT_Manifest;
            break;
        case FCC_ASND:
            pFileType = ANFT_Sound;
            break;
        case FCC_ABNK:
            pFileType = ANFT_Bank;
            break;
        case FCC_MODL:
            pFileType = ANFT_Model;
            break;
        case FCC_DEPS:
            pFileType = ANFT_DependencyTable;
            break;
        case FCC_EULA:
            pFileType = ANFT_EULA;
            break;
        case FCC_hvkC:
            pFileType = ANFT_HavokCloth;
            break;
        case FCC_mapc:
            pFileType = ANFT_Map;
            break;
        case FCC_AMAT:
            pFileType = ANFT_Material;
            break;
        }
    }

    // Identify sounds
    if (pFileType == ANFT_Sound) {
        if (pSize >= 12) {
            if (*(uint32*)pData == FCC_asnd) {
                if (pSize >= 40) {
                    pFileType = (*(uint32*)(pData + 36) == FCC_OggS) ? ANFT_OGG : ANFT_MP3;
                } else {
                    return IR_NotEnoughData;
                }
            } else if (*(uint16*)pData == FCC_PF && *(uint32*)(pData + 8) == FCC_ASND) {
                if (pSize >= 96) {
                    pFileType = (*(uint32*)(pData + 92) == FCC_OggS) ? ANFT_OGG : ANFT_MP3;
                } else {
                    return IR_NotEnoughData;
                }
            }
        } else {
            return IR_NotEnoughData;
        }
    }

    return IR_Success;
}

}; // namespace gw2b
