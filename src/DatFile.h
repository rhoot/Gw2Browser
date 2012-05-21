/** \file       DatFile.h
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

#pragma once

#ifndef DATFILE_H_INCLUDED
#define DATFILE_H_INCLUDED

#include <wx/file.h>
#include "ANetStructs.h"

namespace gw2b
{
class FileReader;

/** Represents a GW2 .dat file. */
class DatFile
{
    struct IdEntry
    {
        uint32  mBaseId;
        uint32  mFileId;
    };
private:
    typedef Array<ANetMftEntry,false>   EntryArray;
    typedef Array<IdEntry,false>        EntryToIdArray;
    typedef Array<byte,false>           InputBufferArray;
private:
    wxFile              mFile;
    ANetDatHeader       mDatHead;
    ANetMftHeader       mMftHead;
    EntryArray          mMftEntries;
    EntryToIdArray      mEntryToId;
    InputBufferArray    mInputBuffer;
    uint                mLastReadEntry;
private:
    enum { MFT_FILE_OFFSET = 16 };
public:
    enum IdentificationResult 
    {
        IR_Success,
        IR_NotEnoughData,
        IR_Failure,
    };
public:
    /** Default constructor. Initializes internals. */
    DatFile();
    /** Constructor. Initializes internals and opens the given .dat file.
     *  \param[in]  pFilename   Name of the .dat file to open. */
    DatFile(const wxString& pFilename);
    /** Destructor. Makes sure to clear out any unfreed data. */
    ~DatFile();

    /** Opens the given .dat file for reading.
     *  \param[in]  pFilename   Name of the .dat file to open.
     *  \return bool    true if opening succeeded, false if not. */
    bool Open(const wxString& pFilename);
    /** Checks whether or not this object currently has a .dat file open.
     *  \return bool    true if a .dat file is open, false if not. */
    bool IsOpen() const;
    /** Closes the open .dat file, if any. */
    void Close();

    /** Gets the MFT entry number for the file with the given file id.
     *  \param[in]  pFileId     ID of the file to get the entry number for.
     *  \return uint    The MFT entry num if it was found, UINT_MAX if not. */
    uint GetEntryNumFromFileId(uint pFileId) const;
    /** Gets the file ID of the file with the given MFT entry number.
     *  \param[in]  pEntryNum   Entry number to get the file ID for.
     *  \return uint    The file ID if it was found, UINT_MAX if not. */
    uint GetFileIdFromEntryNum(uint pEntryNum) const;
    /** Gets the file ID of the file with the given MFT file entry number.
     *  \param[in]  pEntryNum   File entry number to get the file ID for.
     *  \return uint    The file ID if it was found, UINT_MAX if not. */
    uint GetFileIdFromFileNum(uint pEntryNum) const;

    /** Gets the MFT entry number for the file with the given base id.
     *  \param[in]  pBaseId     Base ID of the file to get the entry number for.
     *  \return uint    The MFT entry num if it was found, UINT_MAX if not. */
    uint GetEntryNumFromBaseId(uint pBaseId) const;
    /** Gets the base id of the file with the given MFT entry number.
     *  \param[in]  pEntryNum   Entry number to get the base id for.
     *  \return uint    The base id if it was found, UINT_MAX if not. */
    uint GetBaseIdFromEntryNum(uint pEntryNum) const;
    /** Gets the base id of the file with the given MFT file entry number.
     *  \param[in]  pEntryNum   File entry number to get the base id for.
     *  \return uint    The base id if it was found, UINT_MAX if not. */
    uint GetBaseIdFromFileNum(uint pEntryNum) const;

    /** Gets the total uncompressed size of the given entry.
     *  \param[in]  pEntryNum   Entry number to check the size for.
     *  \return uint    Uncompressed size of the entry. */
    uint GetEntrySize(uint pEntryNum);
    /** Gets the total uncompressed size of the given file.
     *  \param[in]  pFileNum   File entry number to check the size for.
     *  \return uint    Uncompressed size of the file. */
    uint GetFileSize(uint pFileNum);
    /** Gets the amount of total MFT entries in the .dat file. 
     *  \return uint    Amount of entries in the .dat file, UINT_MAX if file not open. */
    uint GetNumEntries() const                  { if (!this->IsOpen()) { return UINT_MAX; } return mMftHead.mNumEntries; }
    /** Gets the amount of file MFT entries in the .dat file. 
     *  \return uint    Amount of file entries in the .dat file, UINT_MAX if file not open. */
    uint GetNumFiles() const                    { if (!this->IsOpen()) { return UINT_MAX; } return mMftHead.mNumEntries - MFT_FILE_OFFSET; }
    /** Gets the amount of entries that appear before the file entries in the 
    *   .dat file.
     *  \return uint    Index of the first file entry in the MFT. */
    uint GetMftFileOffset() const               { return MFT_FILE_OFFSET; }

    /** Peeks at the contents of the given MFT entry and returns the results.
     *  \param[in]  pEntryNum   MFT entry number to get contents for.
     *  \param[in]  pPeekSize   Amount of bytes to peek at. Specifying 0 will read the whole entry.
     *  \param[in,out]  poBuffer    Buffer to store results in. Must be *at least*
     *                  pPeekSize in length.
     *  \return uint    Size of poBuffer. */
    uint PeekEntry(uint pEntryNum, uint pPeekSize, byte* poBuffer);
    /** Peeks at the contents of the given MFT file entry and returns the results.
     *  \param[in]  pFileNum    MFT entry number to get contents for.
     *  \param[in]  pPeekSize   Amount of bytes to peek at. Specifying 0 will read the whole file.
     *  \param[in,out]  poBuffer    Buffer to store results in. Must be *at least*
     *                  pPeekSize in length.
     *  \return uint    Size of poBuffer. */
    uint PeekFile(uint pFileNum, uint pPeekSize, byte* poBuffer);
    /** Peeks at the contents of the given MFT entry and returns the results.
     *  \param[in]  pEntryNum   MFT entry number to get contents for.
     *  \param[in]  pPeekSize   Amount of bytes to peek at.
     *  \return Array<byte>* Object used to handle the peeked data. Must be deleted. */
    Array<byte>* PeekEntry(uint pEntryNum, uint pPeekSize);
    /** Peeks at the contents of the given MFT file entry and returns the results.
     *  \param[in]  pFileNum    MFT file entry number to get contents for.
     *  \param[in]  pPeekSize   Amount of bytes to peek at.
     *  \return Array<byte>* Object used to handle the peeked data. Must be deleted. */
    Array<byte>* PeekFile(uint pFileNum, uint pPeekSize);

    /** Reads the contents of the given MFT entry and returns the results.
     *  \param[in]  pEntryNum   MFT entry number to get contents for.
     *  \param[in,out]  poBuffer    Buffer to store results in. It is up to the
                        caller to make sure the buffer is big enough.
     *  \return uint    Size of poBuffer. */
    uint ReadEntry(uint pEntryNum, byte* poBuffer);
    /** Reads the contents of the given MFT file entry and returns the results.
     *  \param[in]  pFileNum   MFT entry number to get contents for.
     *  \param[in,out]  poBuffer    Buffer to store results in. It is up to the
                        caller to make sure the buffer is big enough.
     *  \return uint    Size of poBuffer. */
    uint ReadFile(uint pFileNum, byte* poBuffer);
    /** Reads the data contained at the given MFT entry.
     *  \param[in]  pEntryNum   MFT entry number to read.
     *  \return Array<byte>* Object used to handle the read data. Must be deleted. */
    Array<byte>* ReadEntry(uint pEntryNum);
    /** Reads the file contained at the given MFT entry.
     *  \param[in]  pFileNum    MFT file entry number to read.
     *  \return Array<byte>* Object used to handle the read file. Must be deleted. */
    Array<byte>* ReadFile(uint pFileNum);

    IdentificationResult IdentifyFileType(byte* pData, uint pSize, ANetFileType& pFileType);

}; // class DatFile

}; // namespace gw2b

#endif // DATFILE_H_INCLUDED
