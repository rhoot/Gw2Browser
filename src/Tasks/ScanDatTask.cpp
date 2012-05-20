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

ScanDatTask::ScanDatTask(DatIndex* pIndex, DatFile& pDatFile)
    : mIndex(pIndex, true)
    , mDatFile(pDatFile)
{
    Ensure::NotNull(pIndex);
    Ensure::NotNull(&pDatFile);
}

ScanDatTask::~ScanDatTask()
{
}

bool ScanDatTask::Init()
{
    this->SetMaxProgress(mDatFile.GetNumFiles());
    this->SetCurrentProgress(mIndex->GetHighestMftEntry() + 1);

    uint filesLeft = mDatFile.GetNumFiles() - (mIndex->GetHighestMftEntry() + 1);
    mIndex->ReserveEntries(filesLeft);

    return true;
}

void ScanDatTask::Perform()
{
    if (!this->IsDone()) {
        uint32 entryNumber = this->GetCurrentProgress();
        Array<byte>* file = mDatFile.PeekFile(entryNumber, 0x20);

        // Skip if empty
        if (!file || !file->GetSize()) {
            this->SetCurrentProgress(entryNumber + 1);
            DeletePointer(file);
            return;
        }

        // Get the file type
        ANetFileType fileType;
        DatFile::IdentificationResult results = mDatFile.IdentifyFileType(file->GetPointer(), file->GetSize(), fileType);
        uint lastRequestedSize = 0x20;
        while (results == DatFile::IR_NotEnoughData) {
            uint sizeRequired = this->GetRequiredIdentificationSize(file->GetPointer(), file->GetSize(), fileType);
            if (sizeRequired <= lastRequestedSize) { break; }
            lastRequestedSize = sizeRequired;
            DeletePointer(file);
            file = mDatFile.PeekFile(entryNumber, sizeRequired);
            mDatFile.IdentifyFileType(file->GetPointer(), file->GetSize(), fileType);
        }

        // Categorize the entry
        DatIndexCategory* category = this->Categorize(fileType, file->GetPointer(), file->GetSize());

        // Add to index
        uint baseId = mDatFile.GetBaseIdFromFileNum(entryNumber);
        DatIndexEntry& newEntry = mIndex->AddIndexEntry()
            ->SetBaseId(baseId)
            .SetFileId(mDatFile.GetFileIdFromFileNum(entryNumber))
            .SetFileType(fileType)
            .SetMftEntry(entryNumber)
            .SetName(wxString::Format(wxT("%d"), baseId));
        // Found a file with no baseId...
        if (baseId == 0) {
            newEntry.SetName(wxString::Format(wxT("ID-less_%d"), entryNumber));
        }
        // Finalize the add
        category->AddEntry(&newEntry);
        newEntry.FinalizeAdd();

        // Delete the reader and proceed to the next file
        DeletePointer(file);
        this->SetText(wxString::Format(wxT("Scanning .dat: %d/%d"), entryNumber, this->GetMaxProgress()));
        this->SetCurrentProgress(entryNumber + 1);
    }
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

#define MakeSubCategory(x)  { category = category->FindOrAddSubCategory(x); }
DatIndexCategory* ScanDatTask::Categorize(ANetFileType pFileType, byte* pData, uint pSize)
{
    DatIndexCategory* category = NULL;
    
    // Textures
    if (pFileType > ANFT_TextureStart && pFileType < ANFT_TextureEnd) {
        category = mIndex->FindOrAddCategory(wxT("Textures"));

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
        category = mIndex->FindOrAddCategory(wxT("Sounds"));
    }

    // Binaries
    else if (pFileType == ANFT_Binary || pFileType == ANFT_EXE || pFileType == ANFT_DLL) {
        category = mIndex->FindOrAddCategory(wxT("Binaries"));
    }

    // Strings
    else if (pFileType == ANFT_StringFile) {
        category = mIndex->FindOrAddCategory(wxT("Strings"));
    }

    // Manifests
    else if (pFileType == ANFT_Manifest) {
        category = mIndex->FindOrAddCategory(wxT("Manifests"));
    }

    // Bank files
    else if (pFileType == ANFT_Bank) {
        category = mIndex->FindOrAddCategory(wxT("Bank files"));
    }

    // Model files
    else if (pFileType == ANFT_Model) {
        category = mIndex->FindOrAddCategory(wxT("Models"));
    }

    // Dependency table
    else if (pFileType == ANFT_DependencyTable) {
        category = mIndex->FindOrAddCategory(wxT("Dependency tables"));
    }

    // EULA
    else if (pFileType == ANFT_EULA) {
        category = mIndex->FindOrAddCategory(wxT("EULA"));
    }
    
    // Cinematic
    else if (pFileType == ANFT_Cinematic) {
        category = mIndex->FindOrAddCategory(wxT("Cinematics"));
    }

    // Havok
    else if (pFileType == ANFT_HavokCloth) {
        category = mIndex->FindOrAddCategory(wxT("Havok cloth"));
    }

    // Maps
    else if (pFileType == ANFT_Map) {
        category = mIndex->FindOrAddCategory(wxT("Maps"));
    }

    // Materials
    else if (pFileType == ANFT_Material) {
        category = mIndex->FindOrAddCategory(wxT("Materials"));
    }

    // Random PF files
    else if (pFileType == ANFT_PF) {
        category = mIndex->FindOrAddCategory(wxT("Misc"));

        if (pSize >= 12) {
            MakeSubCategory(wxString((const char*)(pData + 8), 4));
        }
    }

    // ABFF
    else if (pFileType == ANFT_ABFF) {
        category = mIndex->FindOrAddCategory(wxT("Misc"));
        MakeSubCategory(wxT("ABFF"));
    }

    // unknown stuff
    else {
        category = mIndex->FindOrAddCategory(wxT("Unknown"));
        MakeSubCategory(wxString::Format(wxT("%x"), *(const uint32*)pData));
    }

    return category;
}

}; // namespace gw2b
