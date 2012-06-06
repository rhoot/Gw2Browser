/** \file       ReadIndexTask.cpp
 *  \brief      Contains declaration of the ScanDatTask class.
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
#include "ScanDatTask.h"

#include "DatFile.h"
#include "DatIndex.h"
#include "FileReader.h"

namespace gw2b
{

ScanDatTask::ScanDatTask(const std::shared_ptr<DatIndex>& pIndex, DatFile& pDatFile)
    : mIndex(pIndex)
    , mDatFile(pDatFile)
{
    Ensure::notNull(pIndex.get());
    Ensure::notNull(&pDatFile);
}

ScanDatTask::~ScanDatTask()
{
}

bool ScanDatTask::init()
{
    this->setMaxProgress(mDatFile.numFiles());
    this->setCurrentProgress(mIndex->highestMftEntry() + 1);

    uint filesLeft = mDatFile.numFiles() - (mIndex->highestMftEntry() + 1);
    mIndex->reserveEntries(filesLeft);

    return true;
}

void ScanDatTask::perform()
{
    // Make sure the output buffer is big enough
    this->EnsureBufferSize(0x20);

    // Read file
    uint32 entryNumber = this->currentProgress();
    uint size = mDatFile.peekFile(entryNumber, 0x20, mOutputBuffer.GetPointer());

    // Skip if empty
    if (!size) {
        this->setCurrentProgress(entryNumber + 1);
        return;
    }

    // Get the file type
    ANetFileType fileType;
    DatFile::IdentificationResult results = mDatFile.identifyFileType(mOutputBuffer.GetPointer(), size, fileType);

    // Enough data to identify the file type?
    uint lastRequestedSize = 0x20;
    while (results == DatFile::IR_NotEnoughData) {
        uint sizeRequired = this->GetRequiredIdentificationSize(mOutputBuffer.GetPointer(), size, fileType);

        // Prevent infinite loops
        if (sizeRequired == lastRequestedSize) { break; }
        lastRequestedSize = sizeRequired;

        // Re-read with the newly asked-for size
        this->EnsureBufferSize(sizeRequired);
        size    = mDatFile.peekFile(entryNumber, sizeRequired, mOutputBuffer.GetPointer());
        results = mDatFile.identifyFileType(mOutputBuffer.GetPointer(), size, fileType);
    }

    // Need another check, since the file might have been reloaded a couple of times
    if (!size) {
        this->setCurrentProgress(entryNumber + 1);
        return;
    }

    // Categorize the entry
    DatIndexCategory* category = this->Categorize(fileType, mOutputBuffer.GetPointer(), size);

    // Add to index
    uint baseId = mDatFile.baseIdFromFileNum(entryNumber);
    DatIndexEntry& newEntry = mIndex->addIndexEntry()
        ->setBaseId(baseId)
        .setFileId(mDatFile.fileIdFromFileNum(entryNumber))
        .setFileType(fileType)
        .setMftEntry(entryNumber)
        .setName(wxString::Format(wxT("%d"), baseId));
    // Found a file with no baseId...
    if (baseId == 0) {
        newEntry.setName(wxString::Format(wxT("ID-less_%d"), entryNumber));
    }
    // Finalize the add
    category->addEntry(&newEntry);
    newEntry.finalizeAdd();

    // Delete the reader and proceed to the next file
    this->setText(wxString::Format(wxT("Scanning .dat: %d/%d"), entryNumber, this->maxProgress()));
    this->setCurrentProgress(entryNumber + 1);
}

uint ScanDatTask::GetRequiredIdentificationSize(byte* pData, uint pSize, ANetFileType pFileType)
{
    switch (pFileType) {
    case ANFT_Binary:
        if (pSize >= 0x40) {
            return *(uint32*)(pData + 0x3c) + 0x18;
        } else {
            return 0x140;   // Seems true for most of them
        }
    case ANFT_ABFF:
        return 0x64;
    case ANFT_Sound:
        return 0x160;
    default:
        return 0x20;
    }
}

#define MakeSubCategory(x)  { category = category->findOrAddSubCategory(x); }
DatIndexCategory* ScanDatTask::Categorize(ANetFileType pFileType, byte* pData, uint pSize)
{
    DatIndexCategory* category = nullptr;
    
    // Textures
    if (pFileType > ANFT_TextureStart && pFileType < ANFT_TextureEnd) {
        category = mIndex->findOrAddCategory(wxT("Textures"));

        switch (pFileType) {
        case ANFT_ATEX:
            MakeSubCategory(wxT("Generic textures"));
            break;
        case ANFT_ATTX:
            MakeSubCategory(wxT("Terrain textures"));
            break;
        case ANFT_ATEC:
            MakeSubCategory(wxT("ATEC"));
            break;
        case ANFT_ATEP:
            MakeSubCategory(wxT("Map textures"));
            break;
        case ANFT_ATEU:
            MakeSubCategory(wxT("UI textures"));
            break;
        case ANFT_ATET:
            MakeSubCategory(wxT("ATET"));
            break;
        case ANFT_DDS:
            MakeSubCategory(wxT("DDS"));
            break;
        }

        if (pFileType != ANFT_DDS && pSize >= 12) {
            uint16 width  = *(const uint16*)(pData + 0x8);
            uint16 height = *(const uint16*)(pData + 0xa);
            MakeSubCategory(wxString::Format(wxT("%dx%d"), width, height));
        } else if (pSize >= 20) {
            uint32 width  = *(const uint32*)(pData + 0x10);
            uint32 height = *(const uint32*)(pData + 0x0c);
            MakeSubCategory(wxString::Format(wxT("%dx%d"), width, height));
        }
    }

    // Sounds
    else if (pFileType == ANFT_MP3 || pFileType == ANFT_OGG || pFileType == ANFT_Sound) {
        category = mIndex->findOrAddCategory(wxT("Sounds"));
    }

    // Binaries
    else if (pFileType == ANFT_Binary || pFileType == ANFT_EXE || pFileType == ANFT_DLL) {
        category = mIndex->findOrAddCategory(wxT("Binaries"));
    }

    // Strings
    else if (pFileType == ANFT_StringFile) {
        category = mIndex->findOrAddCategory(wxT("Strings"));
    }

    // Manifests
    else if (pFileType == ANFT_Manifest) {
        category = mIndex->findOrAddCategory(wxT("Manifests"));
    }

    // Bank files
    else if (pFileType == ANFT_Bank) {
        category = mIndex->findOrAddCategory(wxT("Bank files"));
    }

    // Model files
    else if (pFileType == ANFT_Model) {
        category = mIndex->findOrAddCategory(wxT("Models"));
    }

    // Dependency table
    else if (pFileType == ANFT_DependencyTable) {
        category = mIndex->findOrAddCategory(wxT("Dependency tables"));
    }

    // EULA
    else if (pFileType == ANFT_EULA) {
        category = mIndex->findOrAddCategory(wxT("EULA"));
    }
    
    // Cinematic
    else if (pFileType == ANFT_Cinematic) {
        category = mIndex->findOrAddCategory(wxT("Cinematics"));
    }

    // Havok
    else if (pFileType == ANFT_HavokCloth) {
        category = mIndex->findOrAddCategory(wxT("Havok cloth"));
    }

    // Maps
    else if (pFileType == ANFT_Map) {
        category = mIndex->findOrAddCategory(wxT("Maps"));
    }

    // Materials
    else if (pFileType == ANFT_Material) {
        category = mIndex->findOrAddCategory(wxT("Materials"));
    }

    // Random PF files
    else if (pFileType == ANFT_PF) {
        category = mIndex->findOrAddCategory(wxT("Misc"));

        if (pSize >= 12) {
            MakeSubCategory(wxString((const char*)(pData + 8), 4));
        }
    }

    // ABFF
    else if (pFileType == ANFT_ABFF) {
        category = mIndex->findOrAddCategory(wxT("Misc"));
        MakeSubCategory(wxT("ABFF"));
    }

    // unknown stuff
    else {
        category = mIndex->findOrAddCategory(wxT("Unknown"));
        MakeSubCategory(wxString::Format(wxT("%x"), *(const uint32*)pData));
    }

    return category;
}

void ScanDatTask::EnsureBufferSize(uint pSize)
{
    if (mOutputBuffer.GetSize() < pSize) {
        mOutputBuffer.SetSize(pSize);
    }
}

}; // namespace gw2b
