/** \file       FileReader.cpp
 *  \brief      Contains the definition for the base class of readers for the 
 *              various file types.
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
#include "FileReader.h"

#include "Readers/ImageReader.h"

namespace gw2b
{
    
FileReader::FileReader(const FileReaderData& pData)
    : mData(pData)
{
}

FileReader::~FileReader()
{
    FreePointer(mData.mData);
}

void FileReader::Clean()
{
    FreePointer(mData.mData);
    ::memset(&mData, 0, sizeof(mData));
}

byte* FileReader::ConvertData(uint& pOutSize) const
{
    if (!mData.mData) {
        pOutSize = 0;
        return NULL;
    }

    byte* outData = Alloc<byte>(mData.mSize);
    ::memcpy(outData, mData.mData, mData.mSize);
    pOutSize = mData.mSize;

    return outData;
}

FileReader* FileReader::GetReaderForData(const FileReaderData& pData)
{
    switch (pData.mFileType) {
    case ANFT_ATEX:
    case ANFT_ATTX:
    case ANFT_ATEC:
    case ANFT_ATEP:
    case ANFT_ATEU:
    case ANFT_ATET:
    case ANFT_DDS:
        if (ImageReader::IsValidHeader(pData.mData, pData.mSize)) {
            return new ImageReader(pData);
        }
        break;
    default:
        break;
    }
    return new FileReader(pData);
}

}; // namespace gw2b
