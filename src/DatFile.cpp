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
    : m_lastReadEntry(-1)
{
    ::memset(&m_datHead, 0, sizeof(m_datHead));
    ::memset(&m_mftHead, 0, sizeof(m_mftHead));
}

DatFile::DatFile(const wxString& p_filename)
    : m_lastReadEntry(-1)
{
    ::memset(&m_datHead, 0, sizeof(m_datHead));
    ::memset(&m_mftHead, 0, sizeof(m_mftHead));
    this->open(p_filename);
}

DatFile::~DatFile()
{
    this->close();
}

bool DatFile::open(const wxString& p_filename)
{
    this->close();

    while (true) {
        // Open file
        m_file.Open(p_filename);
        if (!m_file.IsOpened()) { break; }

        // Read header
        if (m_file.Length() < sizeof(m_datHead)) { break; }
        m_file.Read(&m_datHead, sizeof(m_datHead));

        // Read MFT Header
        if (static_cast<uint64>(m_file.Length()) < m_datHead.mftOffset + m_datHead.mftSize) { break; }
        m_file.Seek(m_datHead.mftOffset, wxFromStart);
        m_file.Read(&m_mftHead, sizeof(m_mftHead));

        // Read all of the MFT
        if (m_datHead.mftSize != m_mftHead.numEntries * sizeof(ANetMftEntry)) { break; }
        if (m_datHead.mftSize % sizeof(ANetMftEntry)) { break; }

        m_mftEntries.SetSize(m_datHead.mftSize / sizeof(ANetMftEntry));
        m_file.Seek(m_datHead.mftOffset, wxFromStart);
        m_file.Read(m_mftEntries.GetPointer(), m_datHead.mftSize);

        // Read the file id entry table
        if (static_cast<uint64>(m_file.Length()) < m_mftEntries[2].offset + m_mftEntries[2].size) { break; }
        if (m_mftEntries[2].size % sizeof(ANetFileIdEntry)) { break; }

        uint numFileIdEntries = m_mftEntries[2].size / sizeof(ANetFileIdEntry);
        Array<ANetFileIdEntry> fileIdTable(numFileIdEntries);
        m_file.Seek(m_mftEntries[2].offset, wxFromStart);
        m_file.Read(fileIdTable.GetPointer(), m_mftEntries[2].size);

        // Extract the entry -> base/file ID tables
        m_entryToId.SetSize(m_mftEntries.GetSize());
        ::memset(m_entryToId.GetPointer(), 0, m_entryToId.GetByteSize());

        for (uint i = 0; i < numFileIdEntries; i++) {
            if (fileIdTable[i].fileId == 0 && fileIdTable[i].mftEntryIndex == 0) {
                continue;
            }

            uint entryIndex = fileIdTable[i].mftEntryIndex;
            auto& entry     = m_entryToId[entryIndex];

            if (entry.baseId == 0) {
                entry.baseId = fileIdTable[i].fileId;
            } else if (entry.fileId == 0) {
                entry.fileId = fileIdTable[i].fileId;
            }

            if (entry.baseId > 0 && entry.fileId > 0) {
                if (entry.baseId > entry.fileId) {
                    std::swap(entry.baseId, entry.fileId);
                }
            }
        }

        // Success!
        return true;
    }

    this->close();
    return false;
}

bool DatFile::isOpen() const
{
    return m_file.IsOpened();
}

void DatFile::close()
{
    // Clear input buffer and lookup tables
    m_inputBuffer.Clear();
    m_entryToId.Clear();

    // Clear PODs
    ::memset(&m_datHead, 0, sizeof(m_datHead));
    ::memset(&m_mftHead, 0, sizeof(m_mftHead));

    // Remove MFT entries and close the file
    m_mftEntries.Clear();
    m_file.Close();
}

uint DatFile::entrySize(uint p_entryNum)
{
    if (!isOpen()) { return std::numeric_limits<uint>::max(); }
    if (p_entryNum >= m_mftEntries.GetSize()) { return std::numeric_limits<uint>::max(); }
    
    auto& entry = m_mftEntries[p_entryNum];

    // If the entry is compressed we need to read the uncompressed size from the .dat
    if (entry.compressionFlag & ANCF_Compressed) {
        uint32 uncompressedSize = 0;
        m_file.Seek(entry.offset + 4, wxFromStart);
        m_file.Read(&uncompressedSize, sizeof(uncompressedSize));
        return uncompressedSize;
    } 
        
    return entry.size;
}

uint DatFile::fileSize(uint p_fileNum) {
    return this->entrySize(p_fileNum + MFT_FILE_OFFSET);
}

uint DatFile::entryNumFromFileId(uint p_fileId) const
{
    if (!isOpen()) { return std::numeric_limits<uint>::max(); }
    for (uint i = 0; i < m_entryToId.GetSize(); i++) {
        uint fileId = (m_entryToId[i].fileId == 0 ? m_entryToId[i].baseId : m_entryToId[i].fileId);
        if (fileId == p_fileId) {
            return i;
        }
    }
    return std::numeric_limits<uint>::max();
}

uint DatFile::fileIdFromEntryNum(uint p_entryNum) const
{
    if (!isOpen()) { return std::numeric_limits<uint>::max(); }
    if (p_entryNum >= m_entryToId.GetSize()) { return std::numeric_limits<uint>::max(); }
    return (m_entryToId[p_entryNum].fileId == 0 ? m_entryToId[p_entryNum].baseId : m_entryToId[p_entryNum].fileId);
}

uint DatFile::fileIdFromFileNum(uint p_fileNum) const
{
    return this->fileIdFromEntryNum(p_fileNum + MFT_FILE_OFFSET);
}

uint DatFile::entryNumFromBaseId(uint p_baseId) const
{
    if (!isOpen()) { return std::numeric_limits<uint>::max(); }
    for (uint i = 0; i < m_entryToId.GetSize(); i++) {
        if (m_entryToId[i].baseId == p_baseId) {
            return i;
        }
    }
    return std::numeric_limits<uint>::max();
}

uint DatFile::baseIdFromEntryNum(uint p_entryNum) const
{
    if (!isOpen()) { return std::numeric_limits<uint>::max(); }
    if (p_entryNum >= m_entryToId.GetSize()) { return std::numeric_limits<uint>::max(); }
    return m_entryToId[p_entryNum].baseId;
}

uint DatFile::baseIdFromFileNum(uint p_fileNum) const
{
    return this->baseIdFromEntryNum(p_fileNum + MFT_FILE_OFFSET);
}

uint DatFile::peekFile(uint p_fileNum, uint p_peekSize, byte* po_Buffer)
{
    return this->peekEntry(p_fileNum + MFT_FILE_OFFSET, p_peekSize, po_Buffer);
}

uint DatFile::peekEntry(uint p_entryNum, uint p_peekSize, byte* po_Buffer)
{
    Ensure::notNull(po_Buffer);
    uint inputSize;

    // Return instantly if size is 0, or if the file isn't open
    if (p_peekSize == 0 || !this->isOpen()) {
        return 0;
    }

    // If this was the last entry we read, there's no need to re-read it. The
    // input buffer should already contain the full file.
    if (m_lastReadEntry != p_entryNum) {
        // Perform some checks
        auto entryIsInRange = m_mftHead.numEntries > (uint)p_entryNum;
        if (!entryIsInRange) { return 0; }

        auto entryIsInUse      = (m_mftEntries[p_entryNum].entryFlags & ANMEF_InUse);
        auto fileIsLargeEnough = (uint64)m_file.Length() >= m_mftEntries[p_entryNum].offset + m_mftEntries[p_entryNum].size;
        if (!entryIsInUse || !fileIsLargeEnough) { return 0; }

        // Make sure we can re-use the input buffer
        inputSize = m_mftEntries[p_entryNum].size;
        if (m_inputBuffer.GetSize() < inputSize) {
            m_inputBuffer.SetSize(inputSize);
        }

        // Read the file data
        m_file.Seek(m_mftEntries[p_entryNum].offset, wxFromStart);
        m_file.Read(m_inputBuffer.GetPointer(), inputSize);
        m_lastReadEntry = p_entryNum;
    } else {
        inputSize = m_mftEntries[p_entryNum].size;
    }

    // If the file is compressed we need to uncompress it
    if (m_mftEntries[p_entryNum].compressionFlag) {
        uint32 outputSize = p_peekSize;
        try {
            gw2dt::compression::inflateDatFileBuffer(inputSize, m_inputBuffer.GetPointer(), outputSize, po_Buffer);
            return outputSize;
        } catch (std::exception&) {
            return 0;
        }
    } else {
        uint size = wxMin(p_peekSize, inputSize);
        ::memcpy(po_Buffer, m_inputBuffer.GetPointer(), size);
        return size;
    }
}

Array<byte> DatFile::peekFile(uint p_fileNum, uint p_peekSize)
{
    return this->peekEntry(p_fileNum + MFT_FILE_OFFSET, p_peekSize);
}

Array<byte> DatFile::peekEntry(uint p_entryNum, uint p_peekSize)
{
    Array<byte> output = Array<byte>(p_peekSize);
    uint readBytes = this->peekEntry(p_entryNum, p_peekSize, output.GetPointer());

    if (readBytes == 0) {
        return Array<byte>();
    }

    return output;
}

uint DatFile::readFile(uint p_fileNum, byte* po_Buffer)
{
    return this->readEntry(p_fileNum + MFT_FILE_OFFSET, po_Buffer);
}

uint DatFile::readEntry(uint p_entryNum, byte* po_Buffer)
{
    uint size = this->entrySize(p_entryNum);

    if (size != std::numeric_limits<uint>::max()) {
        return this->peekEntry(p_entryNum, size, po_Buffer);
    } 

    return 0;
}

Array<byte> DatFile::readFile(uint p_fileNum)
{
    return this->readEntry(p_fileNum + MFT_FILE_OFFSET);
}

Array<byte> DatFile::readEntry(uint p_entryNum)
{
    uint size = this->entrySize(p_entryNum);
    Array<byte> output;

    if (size != std::numeric_limits<uint>::max()) {
        output.SetSize(size);
        uint readBytes = this->peekEntry(p_entryNum, size, output.GetPointer());

        if (readBytes > 0) {
            return output;
        }
    }
    
    return Array<byte>();
}

DatFile::IdentificationResult DatFile::identifyFileType(const byte* p_data, uint p_size, ANetFileType& po_fileType)
{
    if (p_size < 4) { po_fileType = ANFT_Unknown; return IR_Failure; }

    // start with fourcc
    auto fourcc = *reinterpret_cast<const uint32*>(p_data);
    po_fileType = ANFT_Unknown;

    // abff files need offsetting
    if (fourcc == FCC_abff) {
        po_fileType = ANFT_ABFF;

        if (p_size >= 0x44) {
            p_data   += 0x40;
            p_size   -= 0x40;
            fourcc = *reinterpret_cast<const uint32*>(p_data);
        } else {
            return IR_NotEnoughData;
        }
    }

    switch (fourcc) {
    case FCC_ATEX:
        po_fileType = ANFT_ATEX;
        break;
    case FCC_ATTX:
        po_fileType = ANFT_ATTX;
        break;
    case FCC_ATEC:
        po_fileType = ANFT_ATEC;
        break;
    case FCC_ATEP:
        po_fileType = ANFT_ATEP;
        break;
    case FCC_ATEU:
        po_fileType = ANFT_ATEU;
        break;
    case FCC_ATET:
        po_fileType = ANFT_ATET;
        break;
    case FCC_DDS:
        po_fileType = ANFT_DDS;
        break;
    case FCC_strs:
        po_fileType = ANFT_StringFile;
        break;
    case FCC_asnd:
        po_fileType = ANFT_Sound;
        break;
    case FCC_CINP:
        po_fileType = ANFT_Cinematic;
        break;
    }

    // Identify binary files
    if ((fourcc & 0xffff) == FCC_MZ) {
        po_fileType = ANFT_Binary;

        if (p_size >= 0x40) {
            auto peOffset = *reinterpret_cast<const uint32*>(p_data + 0x3c);

            if (p_size >= (peOffset + 0x18)) {
                auto flags = *reinterpret_cast<const uint16*>(p_data + peOffset + 0x16);
                po_fileType = (flags & 0x2000) ? ANFT_DLL : ANFT_EXE;
            } else {
                return IR_NotEnoughData;
            }
        } else {
            return IR_NotEnoughData;
        }
    }

    // Identify PF files
    if ((fourcc & 0xffff) == FCC_PF) {
        po_fileType = ANFT_PF;

        if (p_size >= 12) {
            fourcc   = *reinterpret_cast<const uint32*>(p_data + 8);
        } else {
            return IR_NotEnoughData;
        }

        switch (fourcc) {
        case FCC_ARMF:
            po_fileType = ANFT_Manifest;
            break;
        case FCC_ASND:
            po_fileType = ANFT_Sound;
            break;
        case FCC_ABNK:
            po_fileType = ANFT_Bank;
            break;
        case FCC_MODL:
            po_fileType = ANFT_Model;
            break;
        case FCC_DEPS:
            po_fileType = ANFT_DependencyTable;
            break;
        case FCC_EULA:
            po_fileType = ANFT_EULA;
            break;
        case FCC_hvkC:
            po_fileType = ANFT_HavokCloth;
            break;
        case FCC_mapc:
            po_fileType = ANFT_Map;
            break;
        case FCC_AMAT:
            po_fileType = ANFT_Material;
            break;
        }
    }

    // Identify sounds
    if (po_fileType == ANFT_Sound) {
        if (p_size >= 12) {
            if (*reinterpret_cast<const uint32*>(p_data) == FCC_asnd) {
                if (p_size >= 40) {
                    po_fileType = (*reinterpret_cast<const uint32*>(p_data + 36) == FCC_OggS) ? ANFT_OGG : ANFT_MP3;
                } else {
                    return IR_NotEnoughData;
                }
            } else if (*reinterpret_cast<const uint32*>(p_data) == FCC_PF && *reinterpret_cast<const uint32*>(p_data + 8) == FCC_ASND) {
                if (p_size >= 96) {
                    po_fileType = (*reinterpret_cast<const uint32*>(p_data + 92) == FCC_OggS) ? ANFT_OGG : ANFT_MP3;
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

uint DatFile::fileIdFromFileReference(const ANetFileReference& p_fileRef)
{
    Assert(p_fileRef.parts[2] == 0);
    return 0xFF00 * (p_fileRef.parts[1] - 0x100) + (p_fileRef.parts[0] - 0x100) + 1;
}

}; // namespace gw2b
