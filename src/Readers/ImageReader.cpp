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

#ifdef RGB
#  undef RGB
#endif

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
    FCC_DDS   = 0x20534444,

    FCC_DXT1  = 0x31545844,
    FCC_DXT2  = 0x32545844,
    FCC_DXT3  = 0x33545844,
    FCC_DXT4  = 0x34545844,
    FCC_DXT5  = 0x35545844,
    FCC_DXTN  = 0x4e545844,
    FCC_DXTL  = 0x4c545844,
    FCC_DXTA  = 0x41545844,
};

ImageReader::ImageReader(const Array<byte>& pData, ANetFileType pFileType)
    : FileReader(pData, pFileType)
{
}

ImageReader::~ImageReader()
{
}

wxImage ImageReader::GetImage() const
{
    Assert(m_data.GetSize() >= 4);

    wxSize size;
    BGR* colors   = nullptr;
    uint8* alphas = nullptr;

    // Read the correct type of data
    uint32 fourcc = *reinterpret_cast<const uint32*>(m_data.GetPointer());
    if (fourcc != FCC_DDS) {
        if (!this->ReadAtexData(size, colors, alphas)) {
            return wxImage();
        }
    } else {
        if (!this->ReadDdsData(size, colors, alphas)) {
            return wxImage();
        }
    }

    // Create image and fill it with color data
    wxImage image(size.x, size.y);
    image.SetData(reinterpret_cast<unsigned char*>(colors));

    // Set alpha if the format has any
    if (alphas) {
        image.SetAlpha(alphas);
    }

    return image;
}

Array<byte> ImageReader::convertData() const
{
    wxImage image = this->GetImage();

    // Bail if invalid
    if (!image.IsOk()) {
        return Array<byte>();
    }

    // Write the png to memory
    wxMemoryOutputStream stream;
    if (!image.SaveFile(stream, wxBITMAP_TYPE_PNG)) {
        return Array<byte>();
    }

    // Reset the position of the stream
    wxStreamBuffer* buffer = stream.GetOutputStreamBuffer();
    buffer->Seek(0, wxFromStart);

    // Read the data from the stream and into a buffer
    Array<byte> data = Array<byte>(buffer->GetBytesLeft());
    buffer->Read(data.GetPointer(), data.GetSize());

    // Return the PNG data
    return data;
}

bool ImageReader::ReadDdsData(wxSize& poSize, BGR*& poColors, uint8*& poAlphas) const
{
    // Get header
    if (m_data.GetSize() < sizeof(DdsHeader)) { return false; }
    const DdsHeader* header = reinterpret_cast<const DdsHeader*>(m_data.GetPointer());
    
    // Ensure some of the values are correct
    if (header->mMagic != FCC_DDS || 
        header->mSize != sizeof(DdsHeader) - 4 || 
        header->mPixelFormat.mSize != sizeof(DdsPixelFormat))
    {
        return false;
    }

    // Determine the pixel format
    if (header->mPixelFormat.mFlags & 0x40) {               // 0x40 = DDPF_RGB, uncompressed data
        if (!this->ProcessUncompressedDDS(header, reinterpret_cast<RGB*&>(poColors), poAlphas)) {
            return false;
        }
    } else if (header->mPixelFormat.mFlags & 0x4) {         // 0x4 = DDPF_FOURCC, compressed
        const BGRA* data = reinterpret_cast<const BGRA*>(&m_data[sizeof(*header)]);
        switch (header->mPixelFormat.mFourCC) {
        case FCC_DXT1:
            this->ProcessDXT1(data, header->mWidth, header->mHeight, poColors, poAlphas);
            break;
        case FCC_DXT2:
        case FCC_DXT3:
            this->ProcessDXT3(data, header->mWidth, header->mHeight, poColors, poAlphas);
            break;
        case FCC_DXT4:
        case FCC_DXT5:
            this->ProcessDXT5(data, header->mWidth, header->mHeight, poColors, poAlphas);
            break;
        }
    } else if (header->mPixelFormat.mFlags & 0x20000) {     // 0x20000 = DDPF_LUMINANCE, single-byte color
        if (!this->ProcessLuminanceDDS(header, reinterpret_cast<RGB*&>(poColors))) {
            return false;
        }
    }

    if (!!poColors) {
        poSize.Set(header->mWidth, header->mHeight);
    }

    return !!poColors;
}

bool ImageReader::ProcessLuminanceDDS(const DdsHeader* pHeader, RGB*& poColors) const
{
    // Ensure the image is 8-bit
    if (pHeader->mPixelFormat.mRGBBitCount != 8) { return false; }

    // Determine the size of the pixel data
    uint numPixels = pHeader->mWidth * pHeader->mHeight;
    uint dataSize  = (numPixels * pHeader->mPixelFormat.mRGBBitCount) >> 3;

    // Image data buffer too small?
    if (m_data.GetSize() < (sizeof(*pHeader) + dataSize)) { return false; }

    // Read the data (we've already determined that the data is 8bpp above)
    const uint8* pixelData = static_cast<const uint8*>(&m_data[sizeof(*pHeader)]);

#pragma omp parallel for
    for (int y = 0; y < static_cast<int>(pHeader->mHeight); y++) {
        uint32 curPixel  = (y * pHeader->mWidth);

        for (uint x = 0; x < pHeader->mWidth; x++) {
            ::memset(&poColors[curPixel], pixelData[curPixel], sizeof(poColors[curPixel]));
            curPixel++;
        }
    }

    return true;
}

bool ImageReader::ProcessUncompressedDDS(const DdsHeader* pHeader, RGB*& poColors, uint8*& poAlphas) const
{
    // Ensure the image is 32-bit. Until a non-32 bit texture is found,
    // there's no point adding support for it
    if (pHeader->mPixelFormat.mRGBBitCount != 32) { return false; }

    // Determine the size of the pixel data
    uint numPixels = pHeader->mWidth * pHeader->mHeight;
    uint dataSize  = (numPixels * pHeader->mPixelFormat.mRGBBitCount) >> 3;

    // Image data buffer too small?
    if (m_data.GetSize() < (sizeof(*pHeader) + dataSize)) { return false; }

    // Color data
    RGBA shift;
    poColors = allocate<RGB>(numPixels);
    shift.r = lowestSetBit(pHeader->mPixelFormat.mRBitMask);
    shift.g = lowestSetBit(pHeader->mPixelFormat.mGBitMask);
    shift.b = lowestSetBit(pHeader->mPixelFormat.mBBitMask);

    // Alpha data
    bool hasAlpha = (pHeader->mPixelFormat.mFlags & 0x1);    // 0x1 = DDPF_ALPHAPIXELS, alpha is present
    if (hasAlpha) { 
        poAlphas = allocate<uint8>(numPixels); 
        shift.a  = lowestSetBit(pHeader->mPixelFormat.mABitMask);
    }

    // Read the data (we've already determined that the data is 32bpp)
    const uint32* pixelData = reinterpret_cast<const uint32*>(&m_data[sizeof(*pHeader)]);

#pragma omp parallel for
    for (int y = 0; y < static_cast<int>(pHeader->mHeight); y++) {
        uint32 curPixel = (y * pHeader->mWidth);

        for (uint x = 0; x < pHeader->mWidth; x++) {
            poColors[curPixel].r = (pixelData[curPixel] & pHeader->mPixelFormat.mRBitMask) >> shift.r;
            poColors[curPixel].g = (pixelData[curPixel] & pHeader->mPixelFormat.mGBitMask) >> shift.g;
            poColors[curPixel].b = (pixelData[curPixel] & pHeader->mPixelFormat.mBBitMask) >> shift.b;

            if (hasAlpha) {
                poAlphas[curPixel] = (pixelData[curPixel] & pHeader->mPixelFormat.mABitMask) >> shift.a;
            }

            curPixel++;
        }
    }

    return true;
}

bool ImageReader::ReadAtexData(wxSize& poSize, BGR*& poColors, uint8*& poAlphas) const
{
    Assert(IsValidHeader(m_data.GetPointer(), m_data.GetSize()));
    const ANetAtexHeader* atex = reinterpret_cast<const ANetAtexHeader*>(m_data.GetPointer());

    // Determine mipmap0 size and bail if the file is too small
    if (m_data.GetSize() >= sizeof(ANetAtexHeader) + sizeof(uint32)) {
        uint32 mipMap0Size = *reinterpret_cast<const uint32*>(&m_data[sizeof(ANetAtexHeader)]);

        if (mipMap0Size + sizeof(ANetAtexHeader) > m_data.GetSize()) {
            poSize.Set(0, 0);
            return false;
        }
    } else {
        poSize.Set(0, 0);
        return false;
    }

    // Create and init
    ::SImageDescriptor descriptor;
    descriptor.xres = atex->width;
    descriptor.yres = atex->height;
    descriptor.Data = m_data.GetPointer();
    descriptor.imageformat = 0xf;
    descriptor.a = m_data.GetSize();
    descriptor.b = 6;
    descriptor.c = 0;

    // Init some fields
    const uint* data = reinterpret_cast<const uint*>(m_data.GetPointer());
    poColors = nullptr;
    poAlphas = nullptr;

    // Allocate output
    BGRA* output     = allocate<BGRA>(atex->width * atex->height);
    descriptor.image = reinterpret_cast<unsigned char*>(output);

    // Uncompress
    switch (atex->formatInteger) {
    case FCC_DXT1:
        if (::AtexDecompress(data, m_data.GetSize(), 0xf, descriptor, reinterpret_cast<uint*>(output))) {
            this->ProcessDXT1(output, atex->width, atex->height, poColors, poAlphas);
        }
        break;
    case FCC_DXT2:
    case FCC_DXT3:
    case FCC_DXTN:
        if (::AtexDecompress(data, m_data.GetSize(), 0x11, descriptor, reinterpret_cast<uint*>(output))) {
            this->ProcessDXT3(output, atex->width, atex->height, poColors, poAlphas);
        }
        break;
    case FCC_DXT4:
    case FCC_DXT5:
        if (::AtexDecompress(data, m_data.GetSize(), 0x13, descriptor, reinterpret_cast<uint*>(output))) {
            this->ProcessDXT5(output, atex->width, atex->height, poColors, poAlphas);
        }
        break;
    case FCC_DXTA:
        if (::AtexDecompress(data, m_data.GetSize(), 0x14, descriptor, reinterpret_cast<uint*>(output))) {
            this->ProcessDXTA(reinterpret_cast<uint64*>(output), atex->width, atex->height, poColors);
        }
        break;
    case FCC_DXTL:
        if (::AtexDecompress(data, m_data.GetSize(), 0x12, descriptor, reinterpret_cast<uint*>(output))) {
            this->ProcessDXT5(output, atex->width, atex->height, poColors, poAlphas);

            for (uint i = 0; i < (static_cast<uint>(atex->width) * static_cast<uint>(atex->height)); i++) {
                poColors[i].r = (poColors[i].r * poAlphas[i]) / 0xff;
                poColors[i].g = (poColors[i].g * poAlphas[i]) / 0xff;
                poColors[i].b = (poColors[i].b * poAlphas[i]) / 0xff;
            }
        }
        break;
    case FCC_3DCX:
        if (::AtexDecompress(data, m_data.GetSize(), 0x13, descriptor, reinterpret_cast<uint*>(output))) {
            this->Process3DCX(reinterpret_cast<RGBA*>(output), atex->width, atex->height, poColors, poAlphas);
        }
        break;
    default:
        freePointer(output);
        return false;
    }

    freePointer(output);

    if (poColors) { 
        poSize.Set(atex->width, atex->height); 
        return true;
    }

    return false;
}

bool ImageReader::IsValidHeader(const byte* pData, uint pSize)
{
    if (pSize < 0x10) { return false; }
    uint32 fourcc = *reinterpret_cast<const uint32*>(pData);

    // Is this a DDS file?
    if (fourcc == FCC_DDS) {
        if (pSize < sizeof(DdsHeader)) { return false; }
        const DdsHeader* dds = reinterpret_cast<const DdsHeader*>(pData);

        bool isUncompressedRgb  = !!(dds->mPixelFormat.mFlags &    0x40);
        bool isCompressedRgb    = !!(dds->mPixelFormat.mFlags &     0x4);
        bool isLuminanceTexture = !!(dds->mPixelFormat.mFlags & 0x20000);
        bool is8Bit             = (dds->mPixelFormat.mRGBBitCount ==  8);
        bool is32Bit            = (dds->mPixelFormat.mRGBBitCount == 32);

        return (isCompressedRgb || (isLuminanceTexture && is8Bit) || (isUncompressedRgb && is32Bit));
    }

    const ANetAtexHeader* atex = reinterpret_cast<const ANetAtexHeader*>(pData);
    uint32 compression   = atex->formatInteger;

    // The compression algorithm for non-power-of-two textures is unknown
    if (!isPowerOfTwo(atex->width) || !isPowerOfTwo(atex->height)) {
        return false;
    }

    if ((fourcc == FCC_ATEX) || (fourcc == FCC_ATTX) || (fourcc == FCC_ATEP) || 
        (fourcc == FCC_ATEU) || (fourcc == FCC_ATEC) || (fourcc == FCC_ATET)) 
    {
        return (compression == FCC_DXT1) || 
                (compression == FCC_DXT2) || 
                (compression == FCC_DXT3) || 
                (compression == FCC_DXT4) || 
                (compression == FCC_DXT5) || 
                (compression == FCC_DXTN) || 
                (compression == FCC_DXTL) || 
                (compression == FCC_DXTA) ||
                (compression == FCC_3DCX);
    }

    return false;
}

void ImageReader::ProcessDXTColor(BGR* pColors, uint8* pAlphas, const DXTColor& pBlockColor, bool pIsDXT1) const
{
    // Color 0
    pColors[0].r = (pBlockColor.mRed1   << 3) | (pBlockColor.mRed1   >> 2 );
    pColors[0].g = (pBlockColor.mGreen1 << 2) | (pBlockColor.mGreen1 >> 4 );
    pColors[0].b = (pBlockColor.mBlue1  << 3) | (pBlockColor.mBlue1  >> 2 );
    // Color 1
    pColors[1].r = (pBlockColor.mRed2   << 3) | (pBlockColor.mRed2   >> 2 );
    pColors[1].g = (pBlockColor.mGreen2 << 2) | (pBlockColor.mGreen2 >> 4 );
    pColors[1].b = (pBlockColor.mBlue2  << 3) | (pBlockColor.mBlue2  >> 2 );
    // Colors 2 and 3
    if (!pIsDXT1 || pBlockColor.mColor1 > pBlockColor.mColor2) {
        pColors[2].r = (pColors[0].r * 2 + pColors[1].r) / 3;
        pColors[2].g = (pColors[0].g * 2 + pColors[1].g) / 3;
        pColors[2].b = (pColors[0].b * 2 + pColors[1].b) / 3;
        pColors[3].r = (pColors[0].r + pColors[1].r * 2) / 3;
        pColors[3].g = (pColors[0].g + pColors[1].g * 2) / 3;
        pColors[3].b = (pColors[0].b + pColors[1].b * 2) / 3;
        // Alpha is only being set for DXT1
        if (pAlphas) { ::memset(pAlphas, 0xff, 4); }
    } else {
        pColors[2].r = (pColors[0].r + pColors[1].r) >> 1;
        pColors[2].g = (pColors[0].g + pColors[1].g) >> 1;
        pColors[2].b = (pColors[0].b + pColors[1].b) >> 1;
        ::memset(&pColors[3], 0, sizeof(pColors[3]));
        // Alpha is only being set for DXT1
        if (pAlphas) {
            ::memset(pAlphas, 0xff, 3);
            pAlphas[3] = 0x0;
        }
    }
}

void ImageReader::ProcessDXT1(const BGRA* pData, uint pWidth, uint pHeight, BGR*& poColors, uint8*& poAlphas) const {
    uint numPixels  = (pWidth * pHeight);
    uint numBlocks  = numPixels >> 4;
    const DXT1Block* blocks = reinterpret_cast<const DXT1Block*>(pData);

    poColors = allocate<BGR>(numPixels);
    poAlphas = allocate<uint8>(numPixels);

    const uint numHorizBlocks = pWidth >> 2;
    const uint numVertBlocks  = pHeight >> 2;

#pragma omp parallel for
    for (int y = 0; y < static_cast<int>(numVertBlocks); y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            const DXT1Block& block = blocks[(y * numHorizBlocks) + x];
            this->ProcessDXT1Block(poColors, poAlphas, block, x * 4, y * 4, pWidth);
        }    
    }
}

void ImageReader::ProcessDXT1Block(BGR* pColors, uint8* pAlphas, const DXT1Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    uint32 indices = pBlock.mIndices;
    BGR colors[4];
    uint8 alphas[4];

    this->ProcessDXTColor(colors, alphas, pBlock.mColors, true);

    for (uint y = 0; y < 4; y++) {
        uint curPixel = (pBlockY + y) * pWidth + pBlockX;

        for (uint x = 0; x < 4; x++) {
            BGR& color   = pColors[curPixel];
            uint8& alpha = pAlphas[curPixel];

            uint index = (indices & 3);
            ::memcpy(&color, &colors[index], sizeof(color));
            alpha = alphas[index];

            curPixel++;
            indices >>= 2;
        }
    }
}

void ImageReader::ProcessDXTA(const uint64* pData, uint pWidth, uint pHeight, BGR*& poColors) const
{
    uint numPixels    = (pWidth * pHeight);
    uint numBlocks    = numPixels >> 4;

    poColors = allocate<BGR>(numPixels);

    const uint numHorizBlocks = pWidth >> 2;
    const uint numVertBlocks  = pHeight >> 2;

#pragma omp parallel for
    for (int y = 0; y < static_cast<int>(numVertBlocks); y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            uint64 block = pData[(y * numHorizBlocks) + x];
            this->ProcessDXTABlock(poColors, block, x * 4, y * 4, pWidth);
        }    
    }
}

void ImageReader::ProcessDXTABlock(BGR* pColors, uint64 pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    uint8  alphas[8];

    // Alpha 1 and 2
    alphas[0] = (pBlock & 0xff);
    alphas[1] = (pBlock & 0xff00) >> 8;
    pBlock  >>= 16;
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
        uint curPixel = (pBlockY + y) * pWidth + pBlockX;

        for (uint x = 0; x < 4; x++) {
            ::memset(&pColors[curPixel], alphas[pBlock & 0x7], sizeof(pColors[curPixel]));
            curPixel++;
            pBlock >>= 3;
        }
    }
}

void ImageReader::ProcessDXT3(const BGRA* pData, uint pWidth, uint pHeight, BGR*& poColors, uint8*& poAlphas) const
{
    uint numPixels  = (pWidth * pHeight);
    uint numBlocks  = numPixels >> 4;
    const DXT3Block* blocks = reinterpret_cast<const DXT3Block*>(pData);

    poColors = allocate<BGR>(numPixels);
    poAlphas = allocate<uint8>(numPixels);
    
    const uint numHorizBlocks = pWidth >> 2;
    const uint numVertBlocks  = pHeight >> 2;

#pragma omp parallel for
    for (int y = 0; y < static_cast<int>(numVertBlocks); y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            const DXT3Block& block = blocks[(y * numHorizBlocks) + x];
            this->ProcessDXT3Block(poColors, poAlphas, block, x * 4, y * 4, pWidth);
        }
    }
}

void ImageReader::ProcessDXT3Block(BGR* pColors, uint8* pAlphas, const DXT3Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    uint32 indices    = pBlock.mIndices;
    uint64 blockAlpha = pBlock.mAlpha;
    BGR colors[4];

    this->ProcessDXTColor(colors, nullptr, pBlock.mColors, false);

    for (uint y = 0; y < 4; y++) {
        uint curPixel = (pBlockY + y) * pWidth + pBlockX;

        for (uint x = 0; x < 4; x++) {
            BGR& color   = pColors[curPixel];
            uint8& alpha = pAlphas[curPixel];

            uint index = (indices & 3);
            ::memcpy(&color, &colors[index], sizeof(color));
            alpha = ((blockAlpha & 0xf) << 4) | (blockAlpha & 0xf);

            curPixel++;
            indices    >>= 2;
            blockAlpha >>= 4;
        }
    }
}

void ImageReader::ProcessDXT5(const BGRA* pData, uint pWidth, uint pHeight, BGR*& poColors, uint8*& poAlphas) const
{
    uint numPixels    = (pWidth * pHeight);
    uint numBlocks    = numPixels >> 4;
    const DXT3Block* blocks = reinterpret_cast<const DXT3Block*>(pData);

    poColors = allocate<BGR>(numPixels);
    poAlphas = allocate<uint8>(numPixels);

    const uint numHorizBlocks = pWidth >> 2;
    const uint numVertBlocks  = pHeight >> 2;

#pragma omp parallel for
    for (int y = 0; y < static_cast<int>(numVertBlocks); y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            const DXT3Block& block = blocks[(y * numHorizBlocks) + x];
            this->ProcessDXT5Block(poColors, poAlphas, block, x * 4, y * 4, pWidth);
        }
    }
}

void ImageReader::ProcessDXT5Block(BGR* pColors, uint8* pAlphas, const DXT3Block& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    uint32 indices    = pBlock.mIndices;
    uint64 blockAlpha = pBlock.mAlpha;
    BGR    colors[4];
    uint8  alphas[8];

    this->ProcessDXTColor(colors, nullptr, pBlock.mColors, false);

    // Alpha 1 and 2
    alphas[0]    = (blockAlpha & 0xff);
    alphas[1]    = (blockAlpha & 0xff00) >> 8;
    blockAlpha >>= 16;
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
        uint curPixel = (pBlockY + y) * pWidth + pBlockX;

        for (uint x = 0; x < 4; x++) {
            BGR& color = pColors[curPixel];
            uint8& alpha = pAlphas[curPixel];

            uint index = (indices & 3);
            ::memcpy(&color, &colors[index], sizeof(color));
            alpha = alphas[blockAlpha & 7];

            curPixel++;
            indices    >>= 2;
            blockAlpha >>= 3;
        }
    }
}

void ImageReader::Process3DCX(const RGBA* pData, uint pWidth, uint pHeight, BGR*& poColors, uint8*& poAlphas) const
{
    uint numPixels  = (pWidth * pHeight);
    uint numBlocks  = numPixels >> 4;
    const DCXBlock* blocks = reinterpret_cast<const DCXBlock*>(pData);

    poColors = allocate<BGR>(numPixels);
    poAlphas = nullptr; // 3DCX does not use alpha

    const uint numHorizBlocks = pWidth >> 2;
    const uint numVertBlocks  = pHeight >> 2;

#pragma omp parallel for
    for (int y = 0; y < static_cast<int>(numVertBlocks); y++) {
        for (uint x = 0; x < numHorizBlocks; x++)
        {
            const DCXBlock& block = blocks[(y * numHorizBlocks) + x];
            // 3DCX actually uses RGB and not BGR, so *pretend* that's what the output is
            this->Process3DCXBlock(reinterpret_cast<RGB*>(poColors), block, x * 4, y * 4, pWidth);
        }
    }
}

void ImageReader::Process3DCXBlock(RGB* pColors, const DCXBlock& pBlock, uint pBlockX, uint pBlockY, uint pWidth) const
{
    const float floatToByte = 127.5f;
    const float byteToFloat = (1.0f / floatToByte);

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
        uint curPixel = (pBlockY + y) * pWidth + pBlockX;

        for (uint x = 0; x < 4; x++) {
            RGB& color   = pColors[curPixel];

            // Get normal
            normal.r = ((float)reds[red & 7]     * byteToFloat) - 1.0f;
            normal.g = ((float)greens[green & 7] * byteToFloat) - 1.0f;

            // Compute blue, based on red/green
            normal.b = ::sqrt(1.0f - normal.r * normal.r - normal.g * normal.g);

            // Store normal
            color.r = ((normal.r + 1.0f) * floatToByte);
            color.g = ((normal.g + 1.0f) * floatToByte);
            color.b = ((normal.b + 1.0f) * floatToByte);

            // Invert green as that seems to be the more common format
            color.g = 0xff - color.g;

            curPixel++;
            red   >>= 3;
            green >>= 3;
        }
    }
}

}; // namespace gw2b
