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

PackFile::PackFile(const Array<byte>& p_data)
    : m_data(p_data)
{
}

PackFile::~PackFile()
{
}

const byte* PackFile::findChunk(uint p_chunkType, uint& po_size) const
{
    po_size = 0;

    // Bail if the data size is too small
    if (m_data.GetSize() < sizeof(ANetPfHeader)) {
        return nullptr;
    }

    // Bail when Gw2 would
    auto header = reinterpret_cast<const ANetPfHeader*>(m_data.GetPointer());
    if (header->identifier[0] != 'P' || 
        header->identifier[1] != 'F' || 
        header->unknownField2 != 0 ||
        header->pkFileVersion > 0xC)
    {
        return nullptr;
    }

    auto end = 1 + &m_data[m_data.GetSize() - 1];
    auto pos = &m_data[sizeof(ANetPfHeader)];

    while (pos < end) {
        uint bytesLeft = (end - pos);
        if (bytesLeft < sizeof(ANetPfChunkHeader)) {
            return nullptr;
        }

        // Get the chunk header
        auto chunkHead = reinterpret_cast<const ANetPfChunkHeader*>(pos);
        // Calculate actual data size, as mChunkDataSize does not count the size of some header variables
        auto chunkSize = chunkHead->chunkDataSize + offsetof(ANetPfChunkHeader, chunkVersion);

        // Correct chunk type?
        if (chunkHead->chunkTypeInteger == p_chunkType) {
            // Bail if too little data left
            if (chunkSize > bytesLeft) {
                return nullptr;
            }
            // Return result
            po_size = chunkSize;
            return pos;
        } else {
            pos += chunkSize;
        }
    }

    return nullptr;
}

}; // namespace gw2b
