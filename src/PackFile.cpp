/** \file       PackFile.cpp
 *  \brief      Contains the definition of the PF file class.
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
#include "PackFile.h"

#include "ANetStructs.h"

namespace gw2b
{

PackFile::PackFile(const Array<byte>& pData)
    : mData(pData)
{
}

PackFile::~PackFile()
{
}

const byte* PackFile::GetChunk(uint pChunkType, uint& poSize) const
{
    poSize = 0;

    // Bail if the data size is too small
    if (mData.GetSize() < sizeof(ANetPfHeader)) {
        return NULL;
    }

    // Bail when Gw2 would
    const ANetPfHeader* header = reinterpret_cast<const ANetPfHeader*>(mData.GetPointer());
    if (header->mIdentifier[0] != 'P' || 
        header->mIdentifier[1] != 'F' || 
        header->mUnknownField2 != 0 ||
        header->mPkFileVersion > 0xC)
    {
        return NULL;
    }

    const byte* end = &mData[mData.GetSize() - 1];
    const byte* pos = &mData[sizeof(ANetPfHeader)];

    while (pos < end) {
        uint bytesLeft = (end - pos);
        if (bytesLeft < sizeof(ANetPfChunkHeader)) {
            return NULL;
        }

        // Get the chunk header
        const ANetPfChunkHeader* chunkHead = reinterpret_cast<const ANetPfChunkHeader*>(pos);
        // Calculate actual data size, as mChunkDataSize does not count the size of some header variables
        uint chunkSize = chunkHead->mChunkDataSize + offsetof(ANetPfChunkHeader, mChunkVersion);

        // Correct chunk type?
        if (chunkHead->mChunkTypeInteger == pChunkType) {
            // Bail if too little data left
            if (chunkSize > bytesLeft) {
                return NULL;
            }
            // Return result
            poSize = chunkSize;
            return pos;
        } else {
            pos += chunkSize;
        }
    }

    return NULL;
}

}; // namespace gw2b
