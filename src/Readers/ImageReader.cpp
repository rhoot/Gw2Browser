/** \file       Readers/ImageReader.cpp
 *  \brief      Contains the definition of the image reader class.
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
#include <wx/mstream.h>

#include "Imported/AtexAsm.h"
#include "ImageReader.h"

namespace gw2b
{

enum FourCC
{
    FCC_ATEX  = 0x58455441,
    FCC_ATTX  = 0x58545441,
    FCC_ATEP  = 0x50455441,
    FCC_ATEU  = 0x55455441,
    FCC_ATEC  = 0x43455441,
    FCC_ATET  = 0x54455441,
    FCC_3DCX  = 0x58434433,
    FCC_DXT   = 0x00545844,

    FCC_DXT1  = 0x31545844,
    FCC_DXT2  = 0x32545844,
    FCC_DXT3  = 0x33545844,
    FCC_DXT4  = 0x34545844,
    FCC_DXT5  = 0x35545844,
    FCC_DXTN  = 0x4e545844,
    FCC_DXTL  = 0x4c545844,
};

#pragma pack(push, 1)

struct BGR
{
    uint8   b;
    uint8   g;
    uint8   r;
};

#pragma pack(pop)


ImageReader::ImageReader(const FileReaderData& pData)
    : FileReader(pData)
{
}

ImageReader::~ImageReader()
{
}

wxImage* ImageReader::GetImage() const
{
    wxSize size;
    BGRA* imageData = this->ReadAtexData(size);

    // Bail if the data was invalid
    if (!imageData) {
        return NULL;
    }

    // Separate color from alpha, since that's what wxWidgets wants for some weird reason
    uint numPixels = size.x * size.y;
    BGR* colors    = Alloc<BGR>(numPixels);
    uint8* alpha   = Alloc<uint8>(numPixels);
    for (uint i = 0; i < numPixels; i++) {
        ::memcpy(&colors[i], &imageData[i], sizeof(BGR));
        alpha[i] = imageData[i].a;
    }
    FreePointer(imageData);

    // Create and return image
    wxImage* image = new wxImage(size.x, size.y);
    image->SetData((unsigned char*)colors);
    image->SetAlpha(alpha);
    return image;
}

byte* ImageReader::ConvertData(uint& pOutSize) const
{
    wxImage* image = this->GetImage();

    // Bail if invalid
    if (!image) {
        pOutSize = 0;
        return NULL;
    }

    // Write the png to memory
    wxMemoryOutputStream stream;
    if (!image->SaveFile(stream, wxBITMAP_TYPE_PNG)) {
        DeletePointer(image);
        pOutSize = 0;
        return NULL;
    }

    // Reset the position of the stream
    wxStreamBuffer* buffer = stream.GetOutputStreamBuffer();
    buffer->Seek(0, wxFromStart);

    // Read the data from the stream and into a buffer
    pOutSize   = buffer->GetBytesLeft();
    byte* data = Alloc<byte>(pOutSize);
    buffer->Read(data, pOutSize);

    // Return the PNG data
    DeletePointer(image);
    return data;
}

BGRA* ImageReader::ReadAtexData(wxSize& pOutSize) const
{
    assert(IsValidHeader(mData.mData, mData.mSize));
    ANetAtexHeader* atex = (ANetAtexHeader*)mData.mData;

    // Determine mipmap0 size and bail if the file is too small
    if (mData.mSize >= sizeof(ANetAtexHeader) + sizeof(uint32)) {
        uint32 mipMap0Size = *(uint32*)(mData.mData + sizeof(ANetAtexHeader));
        if (mipMap0Size + sizeof(ANetAtexHeader) > mData.mSize) {
            pOutSize.Set(0, 0);
            return NULL;
        }
    } else {
        pOutSize.Set(0, 0);
        return NULL;
    }

    // Create and init
    ::SImageDescriptor descriptor;
    descriptor.xres = atex->mWidth;
    descriptor.yres = atex->mHeight;
    descriptor.Data = mData.mData;
    descriptor.imageformat = 0xf;
    descriptor.a = mData.mSize;
    descriptor.b = 6;
    descriptor.c = 0;

    // Allocate output
    BGRA* output = Alloc<BGRA>(atex->mWidth * atex->mHeight);
    BGRA* image  = NULL;
    descriptor.image = (byte*)output;

    // Uncompress
    switch (atex->mFormatInteger) {
    case FCC_DXT1:
        if (::AtexDecompress((uint*)mData.mData, mData.mSize, 0xf, descriptor, (uint*)output)) {
            image = this->ProcessDXT1(output, atex->mWidth, atex->mHeight);
        }
        break;
    case FCC_DXT2:
    case FCC_DXT3:
    case FCC_DXTN:
        if (::AtexDecompress((uint*)mData.mData, mData.mSize, 0x11, descriptor, (uint*)output)) {
            image = this->ProcessDXT3(output, atex->mWidth, atex->mHeight);
        }
        break;
    case FCC_DXT4:
    case FCC_DXT5:
        if (::AtexDecompress((uint*)mData.mData, mData.mSize, 0x13, descriptor, (uint*)output)) {
            image = this->ProcessDXT5(output, atex->mWidth, atex->mHeight);
        }
        break;
    case FCC_DXTL:
        if (::AtexDecompress((uint*)mData.mData, mData.mSize, 0x12, descriptor, (uint*)output)) {
            image = this->ProcessDXT5(output, atex->mWidth, atex->mHeight);
            for (uint i = 0; i < ((uint)atex->mWidth * (uint)atex->mHeight); i++) {
                image[i].r = (image[i].r * image[i].a) / 0xff;
                image[i].g = (image[i].g * image[i].a) / 0xff;
                image[i].b = (image[i].b * image[i].a) / 0xff;
            }
        }
        break;
    case FCC_3DCX:
        if (::AtexDecompress((uint*)mData.mData, mData.mSize, 0x13, descriptor, (uint*)output)) {
            image = this->Process3DCX(output, atex->mWidth, atex->mHeight);
        }
        break;
    default:
        FreePointer(output);
        return NULL;
    }

    FreePointer(output);
    if (image) { pOutSize.Set(atex->mWidth, atex->mHeight); }
    return image;
}

bool ImageReader::IsValidHeader(byte* pData, uint pSize)
{
    if (pSize >= sizeof(ANetAtexHeader)) {
        ANetAtexHeader* atex = (ANetAtexHeader*)pData;
        uint32 fourcc        = atex->mIdentifierInteger;
        uint32 compression   = atex->mFormatInteger;

        // The compression algorithm for non-power-of-two textures is unknown
        if (!IsPowerOfTwo(atex->mWidth) || !IsPowerOfTwo(atex->mHeight)) {
            return false;
        }

        if ((fourcc == FCC_ATEX) || (fourcc == FCC_ATTX) || (fourcc == FCC_ATEP) || (fourcc == FCC_ATEU) || (fourcc == FCC_ATEC) || (fourcc == FCC_ATET)) {
            return (compression == FCC_DXT1)
                || (compression == FCC_DXT2)
                || (compression == FCC_DXT3)
                || (compression == FCC_DXT4)
                || (compression == FCC_DXT5)
                || (compression == FCC_DXTN)
                || (compression == FCC_DXTL)
                || (compression == FCC_3DCX);
        }
    }

    return false;
}

BGRA* ImageReader::ProcessDXT1(BGRA* pData, uint pWidth, uint pHeight) const {
    uint numBlocks    = (pWidth * pHeight / 16);
    DXT1Block* blocks = (DXT1Block*)pData;
    BGRA* image       = Alloc<BGRA>(pWidth * pHeight);

    uint numHorizBlocks = pWidth >> 2;
    uint numVertBlocks  = pHeight >> 2;

    for (uint y = 0; y < numVertBlocks; y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            DXT1Block& block = blocks[(y * numHorizBlocks) + x];
            this->ProcessDXT1Block(image, block, x * 4, y * 4, pWidth);
        }    
    }

    return image;
}

void ImageReader::ProcessDXT1Block(BGRA* pOutput, const DXT1Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    uint32 indices = pBlock.mIndices;
    BGRA colors[4];

    // Color 1
    colors[0].r = pBlock.mColors.mRed1   << 3;
    colors[0].g = pBlock.mColors.mGreen1 << 2;
    colors[0].b = pBlock.mColors.mBlue1  << 3;
    colors[0].a = 0xff;
    // Color 2
    colors[1].r = pBlock.mColors.mRed2   << 3;
    colors[1].g = pBlock.mColors.mGreen2 << 2;
    colors[1].b = pBlock.mColors.mBlue2  << 3;
    colors[1].a = 0xff;
    // Colors 3 and 4
    if (pBlock.mColors.mColor1 > pBlock.mColors.mColor2) {
        colors[2].r = (colors[0].r * 2 + colors[1].r) / 3;
        colors[2].g = (colors[0].g * 2 + colors[1].g) / 3;
        colors[2].b = (colors[0].b * 2 + colors[1].b) / 3;
        colors[2].a = 0xff;
        colors[3].r = (colors[0].r + colors[1].r * 2) / 3;
        colors[3].g = (colors[0].g + colors[1].g * 2) / 3;
        colors[3].b = (colors[0].b + colors[1].b * 2) / 3;
        colors[3].a = 0xff;
    } else {
        colors[2].r = (colors[0].r + colors[1].r) >> 1;
        colors[2].g = (colors[0].g + colors[1].g) >> 1;
        colors[2].b = (colors[0].b + colors[1].b) >> 1;
        colors[2].a = 0xff;
        ::memset(&colors[3], 0, sizeof(colors[3]));
    }

    for (uint y = 0; y < 4; y++) {
        for (uint x = 0; x < 4; x++) {
            BGRA& output = pOutput[(pBlockY + y) * pWidth + pBlockX + x];
            ::memcpy(&output, &colors[indices & 3], sizeof(output));
            indices >>= 2;
        }
    }
}

BGRA* ImageReader::ProcessDXT3(BGRA* pData, uint pWidth, uint pHeight) const
{
    uint numBlocks    = (pWidth * pHeight / 16);
    DXT3Block* blocks = (DXT3Block*)pData;
    BGRA* image       = Alloc<BGRA>(pWidth * pHeight);

    uint numHorizBlocks = pWidth >> 2;
    uint numVertBlocks  = pHeight >> 2;

    for (uint y = 0; y < numVertBlocks; y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            DXT3Block& block = blocks[(y * numHorizBlocks) + x];
            this->ProcessDXT3Block(image, block, x * 4, y * 4, pWidth);
        }
    }

    return image;
}

void ImageReader::ProcessDXT3Block(BGRA* pOutput, const DXT3Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    uint32 indices = pBlock.mIndices;
    uint64 alpha   = pBlock.mAlpha;
    BGR colors[4];

    // Color 1
    colors[0].r = pBlock.mColors.mRed1   << 3;
    colors[0].g = pBlock.mColors.mGreen1 << 2;
    colors[0].b = pBlock.mColors.mBlue1  << 3;
    // Color 2
    colors[1].r = pBlock.mColors.mRed2   << 3;
    colors[1].g = pBlock.mColors.mGreen2 << 2;
    colors[1].b = pBlock.mColors.mBlue2  << 3;
    // Colors 3 and 4
    colors[2].r = (colors[0].r * 2 + colors[1].r) / 3;
    colors[2].g = (colors[0].g * 2 + colors[1].g) / 3;
    colors[2].b = (colors[0].b * 2 + colors[1].b) / 3;
    colors[3].r = (colors[0].r + colors[1].r * 2) / 3;
    colors[3].g = (colors[0].g + colors[1].g * 2) / 3;
    colors[3].b = (colors[0].b + colors[1].b * 2) / 3;

    for (uint y = 0; y < 4; y++) {
        for (uint x = 0; x < 4; x++) {
            BGRA& output = pOutput[(pBlockY + y) * pWidth + pBlockX + x];
            BGR& color   = colors[indices & 3];

            ::memcpy(&output, &color, sizeof(color));
            output.a = (alpha & 0xf) * 0x11;    // (*0x11) instead of (<<4) since 0xf should map to 0xff and not 0xf0.

            indices >>= 2;
            alpha   >>= 4;
        }
    }
}

BGRA* ImageReader::ProcessDXT5(BGRA* pData, uint pWidth, uint pHeight) const
{
    uint numBlocks    = (pWidth * pHeight / 16);
    DXT3Block* blocks = (DXT3Block*)pData;
    BGRA* image       = Alloc<BGRA>(pWidth * pHeight);

    uint numHorizBlocks = pWidth >> 2;
    uint numVertBlocks  = pHeight >> 2;

    for (uint y = 0; y < numVertBlocks; y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            DXT3Block& block = blocks[(y * numHorizBlocks) + x];
            this->ProcessDXT5Block(image, block, x * 4, y * 4, pWidth);
        }
    }

    return image;
}

void ImageReader::ProcessDXT5Block(BGRA* pOutput, const DXT3Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    uint32 indices = pBlock.mIndices;
    uint64 alpha   = pBlock.mAlpha;
    BGR    colors[4];
    uint8  alphas[8];

    // Color 1
    colors[0].r = pBlock.mColors.mRed1   << 3;
    colors[0].g = pBlock.mColors.mGreen1 << 2;
    colors[0].b = pBlock.mColors.mBlue1  << 3;
    // Color 2
    colors[1].r = pBlock.mColors.mRed2   << 3;
    colors[1].g = pBlock.mColors.mGreen2 << 2;
    colors[1].b = pBlock.mColors.mBlue2  << 3;
    // Colors 3 and 4
    colors[2].r = (colors[0].r * 2 + colors[1].r) / 3;
    colors[2].g = (colors[0].g * 2 + colors[1].g) / 3;
    colors[2].b = (colors[0].b * 2 + colors[1].b) / 3;
    colors[3].r = (colors[0].r + colors[1].r * 2) / 3;
    colors[3].g = (colors[0].g + colors[1].g * 2) / 3;
    colors[3].b = (colors[0].b + colors[1].b * 2) / 3;

    // Alpha 1 and 2
    alphas[0] = (alpha & 0xff);
    alphas[1] = (alpha & 0xff00) >> 8;
    alpha   >>= 16;
    // Alpha 3 to 8
    if (alphas[0] > alphas[1]) {
        for (uint i = 2; i < 8; i++) {
            alphas[i] = ((8 - i) * alphas[0] + (i - 1) * alphas[1]) / 7;
        }
    } else {
        for (uint i = 2; i < 6; i++) {
            alphas[i] = ((6 - i) * alphas[0] + (i - 1) * alphas[1]) / 5;
        }
        alphas[6] = 0x00;
        alphas[7] = 0xff;
    }

    for (uint y = 0; y < 4; y++) {
        for (uint x = 0; x < 4; x++) {
            BGRA& output = pOutput[(pBlockY + y) * pWidth + pBlockX + x];
            BGR& color   = colors[indices & 3];

            ::memcpy(&output, &color, sizeof(color));
            output.a = alphas[alpha & 7];

            indices >>= 2;
            alpha   >>= 3;
        }
    }
}

BGRA* ImageReader::Process3DCX(BGRA* pData, uint pWidth, uint pHeight) const
{
    uint numBlocks    = (pWidth * pHeight / 16);
    DCXBlock* blocks  = (DCXBlock*)pData;
    BGRA* image       = Alloc<BGRA>(pWidth * pHeight);

    uint numHorizBlocks = pWidth >> 2;
    uint numVertBlocks  = pHeight >> 2;

    for (uint y = 0; y < numVertBlocks; y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            DCXBlock& block = blocks[(y * numHorizBlocks) + x];
            // 3DCX actually uses RGBA, so *pretend* that's what the output is
            this->Process3DCXBlock((RGBA*)image, block, x * 4, y * 4, pWidth);
        }
    }

    return image;
}

void ImageReader::Process3DCXBlock(RGBA* pOutput, const DCXBlock& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    static float floatToByte = 127.5f;
    static float byteToFloat = (1.0f / floatToByte);

    uint64 red   = pBlock.mRed;
    uint64 green = pBlock.mGreen;
    uint8 reds[8];
    uint8 greens[8];

    // Reds 1 and 2
    reds[0] = (red & 0xff);
    reds[1] = (red & 0xff00) >> 8;
    red   >>= 16;
    // Reds 3 to 8
    if (reds[0] > reds[1]) {
        for (uint i = 2; i < 8; i++) {
            reds[i] = ((8 - i) * reds[0] + (i - 1) * reds[1]) / 7;
        }
    } else {
        for (uint i = 2; i < 6; i++) {
            reds[i] = ((6 - i) * reds[0] + (i - 1) * reds[1]) / 5;
        }
        reds[6] = 0x00;
        reds[7] = 0xff;
    }
    // Greens 1 and 2
    greens[0] = (green & 0xff);
    greens[1] = (green & 0xff00) >> 8;
    green   >>= 16;
    //Greens 3 to 8
    if (greens[0] > greens[1]) {
        for (uint i = 2; i < 8; i++) {
            greens[i] = ((8 - i) * greens[0] + (i - 1) * greens[1]) / 7;
        }
    } else {
        for (uint i = 2; i < 6; i++) {
            greens[i] = ((6 - i) * greens[0] + (i - 1) * greens[1]) / 5;
        }
        greens[6] = 0x00;
        greens[7] = 0xff;
    }

    struct { float r; float g; float b; } normal;
    for (uint y = 0; y < 4; y++) {
        for (uint x = 0; x < 4; x++) {
            RGBA& output = pOutput[(pBlockY + y) * pWidth + pBlockX + x];

            // Get normal
            normal.r = ((float)reds[red & 7] * byteToFloat) - 1.0f;
            normal.g = ((float)(0xff - greens[green & 7]) * byteToFloat) - 1.0f;

            // Compute blue, based on red/green
            normal.b = ::sqrt(1.0f - normal.r * normal.r - normal.g * normal.g);

            // Store normal
            output.r = ((normal.r + 1.0f) * floatToByte);
            output.g = ((normal.g + 1.0f) * floatToByte);
            output.b = ((normal.b + 1.0f) * floatToByte);
            output.a = 0xff;

            red   >>= 3;
            green >>= 3;
        }
    }
}

}; // namespace gw2b
