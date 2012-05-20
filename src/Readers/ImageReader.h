/** \file       Readers/ImageReader.h
 *  \brief      Contains the declaration of the image reader class.
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

#ifndef READERS_IMAGEREADER_H_INCLUDED
#define READERS_IMAGEREADER_H_INCLUDED

#include "FileReader.h"

#ifdef RGB
#  undef RGB    // GOD DAMN MICROSOFT WITH YOUR GOD DAMN MACROS
#endif

namespace gw2b
{

#pragma pack(push, 1)

union BGRA 
{
    struct {
        uint8 b;
        uint8 g;
        uint8 r;
        uint8 a;
    };
    uint8 mParts[4];
    uint32 mColor;
};

union RGBA
{
    struct {
        uint8 r;
        uint8 g;
        uint8 b;
        uint8 a;
    };
    uint8 mParts[4];
    uint32 mColor;
};

struct BGR
{
    uint8   b;
    uint8   g;
    uint8   r;
};

struct RGB
{
    uint8   r;
    uint8   g;
    uint8   b;
};

struct DXTColor
{
    union {
        struct {
            uint16 mRed1    : 5;
            uint16 mGreen1  : 6;
            uint16 mBlue1   : 5;
        };
        uint16 mColor1;
    };
    union {
        struct {
            uint16 mRed2    : 5;
            uint16 mGreen2  : 6;
            uint16 mBlue2   : 5;
        };
        uint16 mColor2;
    };
};

struct DXT1Block
{
    DXTColor mColors;
    uint32   mIndices;
};

struct DXT3Block
{
    uint64   mAlpha;
    DXTColor mColors;
    uint32   mIndices;
};

struct DCXBlock     // Should be 3DCXBlock, but names can't start with a number D:
{
    uint64  mGreen;
    uint64  mRed;
};

#pragma pack(pop)

class ImageReader : public FileReader
{
public:
    /** Constructor.
     *  \param[in]  pData       Data to be handled by this reader. */
    ImageReader(const FileReaderData& pData);
    /** Destructor. Clears all data. */
    virtual ~ImageReader();

    /** Gets the type of data contained in this file. Not to be confused with
     *  file type.
     *  \return DataType    type of data. */
    virtual DataType GetDataType() const        { return DT_Image; }
    /** Gets an appropriate file extension for the contents of this reader.
     *  \return wxString    File extension. */
    virtual const wxChar* GetExtension() const  { return wxT("png"); }
    /** Converts the data associated with this file into PNG.
     *  The caller is responsible for cleanup the malloc'd data.
     *  \param[out] poSize  size of converted data.
     *  \return byte*   converted data. */
    virtual byte* ConvertData(uint& poSize) const;
    /** Gets the image contained in the data owned by this reader.
     *  \return wxImage     Newly created image. */
    wxImage GetImage() const;
    /** Determines whether the header of this image is valid.
     *  \return bool    true if valid, false if not. */
    static bool IsValidHeader(byte* pData, uint pSize);
private:
    bool ReadAtexData(wxSize& poSize, BGR*& poColors, uint8*& poAlphas) const;
    void ProcessDXT1(BGRA* pData, uint pWidth, uint pHeight, BGR*& poColors, uint8*& poAlphas) const;
    void ProcessDXT1Block(BGR* pColors, uint8* pAlphas, const DXT1Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const;
    void ProcessDXT3(BGRA* pData, uint pWidth, uint pHeight, BGR*& poColors, uint8*& poAlphas) const;
    void ProcessDXT3Block(BGR* pColors, uint8* pAlphas, const DXT3Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const;
    void ProcessDXT5(BGRA* pData, uint pWidth, uint pHeight, BGR*& poColors, uint8*& poAlphas) const;
    void ProcessDXT5Block(BGR* pColors, uint8* pAlphas, const DXT3Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const;
    void Process3DCX(BGRA* pData, uint pWidth, uint pHeight, BGR*& poColors, uint8*& poAlphas) const;
    void Process3DCXBlock(RGB* pColors, const DCXBlock& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const;
}; // class ImageReader

}; // namespace gw2b

#endif // READERS_IMAGEREADER_H_INCLUDED
