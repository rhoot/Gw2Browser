/** \file       DatIndexIO.h
 *  \brief      Contains the declaration for the index reader and writer classes.
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

#ifndef DATINDEXREADER_H_INCLUDED
#define DATINDEXREADER_H_INCLUDED

#include <wx/file.h>
#include "DatIndex.h"

namespace gw2b
{

enum {
    DatIndex_Magic          = 0x4944,
    DatIndex_Version        =    0x2,
    DatIndex_RootCategory   =   -0x1,
};

#pragma pack(push, 1)

/** Structure of the .dat index header in the file. */
struct DatIndexHead
{
    union {
        char mMagic[2];         /**< Contains 'DI'. */
        uint16 mMagicInteger;   /**< Contains 0x4944, in little endian. */
    };
    uint16 mVersion;            /**< Index format version. */
    uint64 mDatTimeStamp;       /**< Indexed .dat file's timestamp. */
    uint32 mNumEntries;         /**< Amount of entries in the index. */
    uint32 mNumCategories;      /**< Amount of categories in the index. */
};

/** Structure of the fixed-width category fields in the .dat index file. */
struct DatIndexCategoryFields
{
    int32 mParent;              /**< Index of the category's parent. -1 for none. */
    uint16 mNameLength;         /**< Length of the category's name, in bytes. */
}; 

/** Structure of the fixed-width entry fields in the .dat index file. */
struct DatIndexEntryFields
{
    int32 mCategory;            /**< Index of the category it belongs to. */
    uint32 mBaseId;             /**< Base ID of the indexed file. */
    uint32 mFileId;             /**< File ID of the indexed file. */
    uint32 mMftEntry;           /**< MFT entry number of the indexed file. */
    uint32 mFileType;           /**< Type of the indexed file. */
    uint16 mNameLength;         /**< Length of the entry's name, in bytes. */
};

#pragma pack(pop)

/** Responsible for reading a .dat index from file. */
class DatIndexReader
{
    DatIndex&       mIndex;
    DatIndexHead    mHeader;
    wxFile          mFile;
public:
    /** Result of the Read() operation. */
    enum ReadResult
    {
        RR_Failure              = 0,            /**< Read failed. Bit 2+ should contain reason. */
        RR_Success              = 1,            /**< Read succeeded. */
        // Bit 1 should only be set for success flags, to allow for (result & RR_Success)
        RR_UnsupportedFile      = ((1<<1)|0),   /**< The file is of an unsupported type. */
        RR_Outdated             = ((1<<2)|0),   /**< The index file version is outdated. */
        RR_CorruptFile          = ((1<<3)|0),   /**< The index file appears to be corrupt. */
    };
public:
    /** Constructor.
     *  \param[in]  pIndex  Index to read into. */
    DatIndexReader(DatIndex& pIndex);
    /** Destructor. */
    ~DatIndexReader();

    /** Opens the given file for reading.
     *  \param[in]  pFilename   File to open.
     *  \return bool    true if open was successful, false if not. */
    bool Open(const wxString& pFilename);
    /** Closes the opened file. */
    void Close();
    /** Determines whether this task is done. 
     *  \return bool    true if the task is done, false if not. */
    bool IsDone() const;
    /** Determines whether there is an open index file.
     *  \return bool    true if there is an open index file, false if not. */
    bool IsOpen() const                 { return mFile.IsOpened(); }

    /** Gets the current amount of read categories.
     *  \return uint    amount of categories. */
    uint GetCurrentCategory() const     { return mIndex.GetNumCategories(); }
    /** Gets the total amount of categories in the file.
     *  \return uint    amount of categories. */
    uint GetNumCategories() const       { return mHeader.mNumCategories; }

    /** Gets the current amount of read entries.
     *  \return uint    amount of entries. */
    uint GetCurrentEntry() const        { return mIndex.GetNumEntries(); }
    /** Gets the total amount of entries in the file.
     *  \return uint    amount of entries. */
    uint GetNumEntries() const          { return mHeader.mNumEntries; }

    /** Performs a read cycle, reading some categories/entries from the file
     *  and adding them to the index. 
     *  \param[in]  pAmount     Amount of read cycles to perform.
     *  \return ReadResult  The result of the read operation(s). */
    ReadResult Read(uint pAmount = 1);
}; // class DatIndexReader

/** Responsible for writing a .dat index to file. */
class DatIndexWriter
{
    DatIndex&       mIndex;
    wxFile          mFile;
    uint            mCategoriesWritten;
    uint            mEntriesWritten;
public:
    /** Constructor.
     *  \param[in]  pIndex  Index to write onto disk. */
    DatIndexWriter(DatIndex& pIndex);
    /** Destructor. */
    ~DatIndexWriter();

    /** Opens the given file for writing.
     *  \param[in]  pFilename   File to open.
     *  \return bool    true if open was successful, false if not. */
    bool Open(const wxString& pFilename);
    /** Closes the opened file. */
    void Close();
    /** Determines whether this task is done. 
     *  \return bool    true if the task is done, false if not. */
    bool IsDone() const;
    /** Determines whether there is an open index file.
     *  \return bool    true if there is an open index file, false if not. */
    bool IsOpen() const                 { return mFile.IsOpened(); }

    /** Gets the current amount of written categories.
     *  \return uint    amount of categories. */
    uint GetCurrentCategory() const     { return mCategoriesWritten; }
    /** Gets the total amount of categories to write.
     *  \return uint    amount of categories. */
    uint GetNumCategories() const       { return mIndex.GetNumCategories(); }

    /** Gets the current amount of written entries.
     *  \return uint    amount of entries. */
    uint GetCurrentEntry() const        { return mEntriesWritten; }
    /** Gets the total amount of entries to write.
     *  \return uint    amount of entries. */
    uint GetNumEntries() const          { return mIndex.GetNumEntries(); }

    /** Performs a write cycle, writing some categories/entries to the file. 
     *  \param[in]  pAmount     Amount of write cycles to perform.
     *  \return bool    true if successful, false if not. */
    bool Write(uint pAmount = 1);

}; // class DatIndexWriter

}; // namespace gw2b

#endif // DATINDEXREADER_H_INCLUDED
