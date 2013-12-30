#include "gw2DatTools/compression/inflateTextureFileBuffer.h"

#include <memory.h>

#include "gw2DatTools/exception/Exception.h"

#include "huffmanTreeUtils.h"

#include <iostream>
#include <vector>

namespace gw2dt
{
namespace compression
{
namespace texture
{

struct Format
{
	uint16_t flags;
	uint16_t pixelSizeInBits;
};

struct FullFormat
{
	Format format;
	uint32_t nbObPixelBlocks;
	uint32_t bytesPerPixelBlock;
	uint32_t bytesPerComponent;
	bool hasTwoComponents;

	uint16_t width;
	uint16_t height;
};

enum FormatFlags
{
	FF_COLOR = 0x10,
	FF_ALPHA = 0x20,
	FF_DEDUCEDALPHACOMP = 0x40,
	FF_PLAINCOMP = 0x80,
	FF_BICOLORCOMP = 0x200
};

enum CompressionFlags
{
	CF_DECODE_WHITE_COLOR = 0x01,
	CF_DECODE_CONSTANT_ALPHA_FROM4BITS = 0x02,
	CF_DECODE_CONSTANT_ALPHA_FROM8BITS = 0x04,
	CF_DECODE_PLAIN_COLOR = 0x08
};

// Static Values
HuffmanTree sHuffmanTreeDict;
Format sFormats[9];
bool sStaticValuesInitialized(false);

void initializeStaticValues()
{
	// Formats
	{
		Format& aDxt1Format = sFormats[0];
		aDxt1Format.flags = FF_COLOR | FF_ALPHA | FF_DEDUCEDALPHACOMP;
		aDxt1Format.pixelSizeInBits = 4;

		Format& aDxt2Format = sFormats[1];
		aDxt2Format.flags = FF_COLOR | FF_ALPHA | FF_PLAINCOMP;
		aDxt2Format.pixelSizeInBits = 8;

		sFormats[2] = sFormats[1];
		sFormats[3] = sFormats[1];
		sFormats[4] = sFormats[1];

		Format& aDxtAFormat = sFormats[5];
		aDxtAFormat.flags = FF_ALPHA | FF_PLAINCOMP;
		aDxtAFormat.pixelSizeInBits = 4;

		Format& aDxtLFormat = sFormats[6];
		aDxtLFormat.flags = FF_COLOR;
		aDxtLFormat.pixelSizeInBits = 8;

		Format& aDxtNFormat = sFormats[7];
		aDxtNFormat.flags = FF_BICOLORCOMP;
		aDxtNFormat.pixelSizeInBits = 8;

		Format& a3dcxFormat = sFormats[8];
		a3dcxFormat.flags = FF_BICOLORCOMP;
		a3dcxFormat.pixelSizeInBits = 8;
	}

    int16_t aWorkingBitTab[MaxCodeBitsLength];
    int16_t aWorkingCodeTab[MaxSymbolValue];

    // Initialize our workingTabs
    memset(&aWorkingBitTab, 0xFF, MaxCodeBitsLength * sizeof(int16_t));
    memset(&aWorkingCodeTab, 0xFF, MaxSymbolValue * sizeof(int16_t));

    fillWorkingTabsHelper(1, 0x01, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
	
    fillWorkingTabsHelper(2, 0x12, &aWorkingBitTab[0], &aWorkingCodeTab[0]);

    fillWorkingTabsHelper(6, 0x11, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x10, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x0F, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x0E, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x0D, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x0C, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x0B, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x0A, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
	fillWorkingTabsHelper(6, 0x09, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x08, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x07, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x06, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x05, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x04, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
    fillWorkingTabsHelper(6, 0x03, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
	fillWorkingTabsHelper(6, 0x02, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
	
    return buildHuffmanTree(sHuffmanTreeDict, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
}

Format deduceFormat(uint32_t iFourCC)
{
	switch(iFourCC)
	{
		case 0x31545844: // DXT1
			return sFormats[0];

		case 0x32545844: // DXT2
			return sFormats[1];

		case 0x33545844: // DXT3
			return sFormats[2];

		case 0x34545844: // DXT4
			return sFormats[3];

		case 0x35545844: // DXT5
			return sFormats[4];

		case 0x41545844: // DXTA
			return sFormats[5];

		case 0x4C545844: // DXTL
			return sFormats[6];

		case 0x4E545844: // DXTN
			return sFormats[7];

		case 0x58434433: // 3DCX
			return sFormats[8];

		default:
			throw exception::Exception("Unknown format.");
	}
}

void decodeWhiteColor(State& ioState, std::vector<bool>& ioAlphaBitMap, std::vector<bool>& ioColorBitMap, const FullFormat& iFullFormat, uint8_t* ioOutputTab)
{
	uint32_t aPixelBlockPos = 0;

	while (aPixelBlockPos < iFullFormat.nbObPixelBlocks)
    {
		// Reading next code
		uint16_t aCode = 0;
		readCode(sHuffmanTreeDict, ioState, aCode);
		
		needBits(ioState, 1);
		uint32_t aValue = readBits(ioState, 1);
		dropBits(ioState, 1);

		while (aCode > 0)
		{
			if (!ioColorBitMap[aPixelBlockPos])
			{
				if (aValue)
				{
					*reinterpret_cast<int64_t*>(&(ioOutputTab[iFullFormat.bytesPerPixelBlock * (aPixelBlockPos)])) = 0xFFFFFFFFFFFFFFFE;

					ioAlphaBitMap[aPixelBlockPos] = true;
					ioColorBitMap[aPixelBlockPos] = true;
				}
				--aCode;
			}
			++aPixelBlockPos;
		}

		while (aPixelBlockPos < iFullFormat.nbObPixelBlocks && ioColorBitMap[aPixelBlockPos])
		{
			++aPixelBlockPos;
		}
    }
}

void decodeConstantAlphaFrom4Bits(State& ioState, std::vector<bool>& ioAlphaBitMap, const FullFormat& iFullFormat, uint8_t* ioOutputTab)
{
	needBits(ioState, 4);
	uint8_t aAlphaValueByte = readBits(ioState, 4);
	dropBits(ioState, 4);

	uint32_t aPixelBlockPos = 0;

	uint16_t aIntermediateByte = aAlphaValueByte | (aAlphaValueByte << 4);
	uint32_t aIntermediateWord = aIntermediateByte | (aIntermediateByte << 8);
	uint64_t aIntermediateDWord = aIntermediateWord | (aIntermediateWord << 16);
	uint64_t aAlphaValue = aIntermediateDWord | (aIntermediateDWord << 32);
	uint64_t zero = 0;

	while (aPixelBlockPos < iFullFormat.nbObPixelBlocks)
    {
		// Reading next code
		uint16_t aCode = 0;
		readCode(sHuffmanTreeDict, ioState, aCode);
		
		needBits(ioState, 2);
		uint32_t aValue = readBits(ioState, 1);
		dropBits(ioState, 1);

		uint8_t isNotNull = readBits(ioState, 1);
		if (aValue)
		{
			dropBits(ioState, 1);
		}
		while (aCode > 0)
		{
			if (!ioAlphaBitMap[aPixelBlockPos])
			{
				if (aValue)
				{
					memcpy(&(ioOutputTab[iFullFormat.bytesPerPixelBlock * (aPixelBlockPos)]), isNotNull ? &aAlphaValue : &zero, iFullFormat.bytesPerComponent);
					ioAlphaBitMap[aPixelBlockPos] = true;
				}
				--aCode;
			}
			++aPixelBlockPos;
		}

		while (aPixelBlockPos < iFullFormat.nbObPixelBlocks && ioAlphaBitMap[aPixelBlockPos])
		{
			++aPixelBlockPos;
		}
    }
}

void decodeConstantAlphaFrom8Bits(State& ioState, std::vector<bool>& ioAlphaBitMap, const FullFormat& iFullFormat, uint8_t* ioOutputTab)
{
	needBits(ioState, 8);
	uint8_t aAlphaValueByte = readBits(ioState, 8);
	dropBits(ioState, 8);

	uint32_t aPixelBlockPos = 0;

	uint64_t aAlphaValue = aAlphaValueByte | (aAlphaValueByte << 8);
	uint64_t zero = 0;

	while (aPixelBlockPos < iFullFormat.nbObPixelBlocks)
    {
		// Reading next code
		uint16_t aCode = 0;
		readCode(sHuffmanTreeDict, ioState, aCode);
		
		needBits(ioState, 2);
		uint32_t aValue = readBits(ioState, 1);
		dropBits(ioState, 1);

		uint8_t isNotNull = readBits(ioState, 1);
		if (aValue)
		{
			dropBits(ioState, 1);
		}
		while (aCode > 0)
		{
			if (!ioAlphaBitMap[aPixelBlockPos])
			{
				if (aValue)
				{
					memcpy(&(ioOutputTab[iFullFormat.bytesPerPixelBlock * (aPixelBlockPos)]), isNotNull ? &aAlphaValue : &zero, iFullFormat.bytesPerComponent);
					ioAlphaBitMap[aPixelBlockPos] = true;
				}
				--aCode;
			}
			++aPixelBlockPos;
		}

		while (aPixelBlockPos < iFullFormat.nbObPixelBlocks && ioAlphaBitMap[aPixelBlockPos])
		{
			++aPixelBlockPos;
		}
    }
}

void decodePlainColor(State& ioState, std::vector<bool>& ioColorBitMap, const FullFormat& iFullFormat, uint8_t* ioOutputTab)
{
	needBits(ioState, 24);
	uint16_t aBlue = readBits(ioState, 8);
	dropBits(ioState, 8);
	uint16_t aGreen = readBits(ioState, 8);
	dropBits(ioState, 8);
	uint16_t aRed = readBits(ioState, 8);
	dropBits(ioState, 8);

	// TEMP

	uint8_t aRedTemp1 = (aRed - (aRed >> 5)) >> 3;
	uint8_t aBlueTemp1 = (aBlue - (aBlue >> 5)) >> 3;

	uint16_t aGreenTemp1 = (aGreen - (aGreen >> 6)) >> 2;

	uint8_t aRedTemp2 = (aRedTemp1 << 3) + (aRedTemp1 >> 2);
	uint8_t aBlueTemp2 = (aBlueTemp1 << 3) + (aBlueTemp1 >> 2);

	uint16_t aGreenTemp2 = (aGreenTemp1 << 2) + (aGreenTemp1 >> 4);

	uint32_t aCompRed = 12 * (aRed - aRedTemp2) / (8 - ((aRedTemp1 & 0x11) == 0x11 ? 1 : 0));
	uint32_t aCompBlue = 12 * (aBlue - aBlueTemp2) / (8 - ((aBlueTemp1 & 0x11) == 0x11 ? 1 : 0));

	uint32_t aCompGreen = 12 * (aGreen - aGreenTemp2) / (8 - ((aGreenTemp1 & 0x1111) == 0x1111 ? 1 : 0));

	// Handle Red

	uint32_t aValueRed1;
	uint32_t aValueRed2;

	if (aCompRed < 2)
	{
		aValueRed1 = aRedTemp1;
		aValueRed2 = aRedTemp1;
	}
	else if (aCompRed < 6)
	{
		aValueRed1 = aRedTemp1;
		aValueRed2 = aRedTemp1 + 1;
	}
	else if (aCompRed < 10)
	{
		aValueRed1 = aRedTemp1 + 1;
		aValueRed2 = aRedTemp1;
	}
	else
	{
		aValueRed1 = aRedTemp1 + 1;
		aValueRed2 = aRedTemp1 + 1;
	}

	// Handle Blue

	uint32_t aValueBlue1;
	uint32_t aValueBlue2;

	if (aCompBlue < 2)
	{
		aValueBlue1 = aBlueTemp1;
		aValueBlue2 = aBlueTemp1;
	}
	else if (aCompBlue < 6)
	{
		aValueBlue1 = aBlueTemp1;
		aValueBlue2 = aBlueTemp1 + 1;
	}
	else if (aCompBlue < 10)
	{
		aValueBlue1 = aBlueTemp1 + 1;
		aValueBlue2 = aBlueTemp1;
	}
	else
	{
		aValueBlue1 = aBlueTemp1 + 1;
		aValueBlue2 = aBlueTemp1 + 1;
	}

	// Handle Green

	uint32_t aValueGreen1;
	uint32_t aValueGreen2;

	if (aCompGreen < 2)
	{
		aValueGreen1 = aGreenTemp1;
		aValueGreen2 = aGreenTemp1;
	}
	else if (aCompGreen < 6)
	{
		aValueGreen1 = aGreenTemp1;
		aValueGreen2 = aGreenTemp1 + 1;
	}
	else if (aCompGreen < 10)
	{
		aValueGreen1 = aGreenTemp1 + 1;
		aValueGreen2 = aGreenTemp1;
	}
	else
	{
		aValueGreen1 = aGreenTemp1 + 1;
		aValueGreen2 = aGreenTemp1 + 1;
	}

	// Final Colors

	uint32_t aValueColor1;
	uint32_t aValueColor2;

	aValueColor1 = aValueRed1 | ((aValueGreen1 | (aValueBlue1 << 6)) << 5);
	aValueColor2 = aValueRed2 | ((aValueGreen2 | (aValueBlue2 << 6)) << 5);

	uint32_t aTempValue1 = 0;
	uint32_t aTempValue2 = 0;

	if (aValueRed1 != aValueRed2)
	{
		if (aValueRed1 == aRedTemp1)
		{
			aTempValue1 += aCompRed;
		}
		else
		{
			aTempValue1 += (12 - aCompRed);
		}
		aTempValue2 += 1;
	}

	if (aValueBlue1 != aValueBlue2)
	{
		if (aValueBlue1 == aBlueTemp1)
		{
			aTempValue1 += aCompBlue;
		}
		else
		{
			aTempValue1 += (12 - aCompBlue);
		}
		aTempValue2 += 1;
	}

	if (aValueGreen1 != aValueGreen2)
	{
		if (aValueGreen1 == aGreenTemp1)
		{
			aTempValue1 += aCompGreen;
		}
		else
		{
			aTempValue1 += (12 - aCompGreen);
		}
		aTempValue2 += 1;
	}

	if (aTempValue2 > 0)
	{
		aTempValue1 = (aTempValue1 + (aTempValue2 / 2)) / aTempValue2;
	}

	bool aDxt1SpecialCase = (iFullFormat.format.flags & FF_DEDUCEDALPHACOMP) && (aTempValue1 == 5 || aTempValue1 == 6 || aTempValue2 != 0);

	if (aTempValue2 > 0 && !aDxt1SpecialCase)
	{
		if (aValueColor2 == 0xFFFF)
		{
			aTempValue1 = 12;
			--aValueColor1;
		}
		else
		{
			aTempValue1 = 0;
			++aValueColor2;
		}
	}

	if (aValueColor2 >= aValueColor1)
	{
		uint32_t aSwapTemp = aValueColor1;
		aValueColor1 = aValueColor2;
		aValueColor2 = aSwapTemp;

		aTempValue1 = 12 - aTempValue1;
	}

	uint32_t aColorChosen;

	if (aDxt1SpecialCase)
	{
		aColorChosen = 2;
	}
	else
	{
		if (aTempValue1 < 2)
		{
			aColorChosen = 0;
		}
		else if (aTempValue1 < 6)
		{
			aColorChosen = 2;
		}
		else if (aTempValue1 < 10)
		{
			aColorChosen = 3;
		}
		else
		{
			aColorChosen = 1;
		}
	}
	// TEMP

	uint64_t aTempValue = aColorChosen | (aColorChosen << 2) | ((aColorChosen | (aColorChosen << 2)) << 4);
	aTempValue = aTempValue | (aTempValue << 8);
	aTempValue = aTempValue | (aTempValue << 16);
	uint64_t aFinalValue = aValueColor1 | (aValueColor2 << 16) | (aTempValue << 32);

	uint32_t aPixelBlockPos = 0;

	while (aPixelBlockPos < iFullFormat.nbObPixelBlocks)
    {
		// Reading next code
		uint16_t aCode = 0;
		readCode(sHuffmanTreeDict, ioState, aCode);
		
		needBits(ioState, 1);
		uint32_t aValue = readBits(ioState, 1);
		dropBits(ioState, 1);

		while (aCode > 0)
		{
			if (!ioColorBitMap[aPixelBlockPos])
			{
				if (aValue)
				{
					uint32_t aOffset = iFullFormat.bytesPerPixelBlock * (aPixelBlockPos) + (iFullFormat.hasTwoComponents ? iFullFormat.bytesPerComponent : 0);
					memcpy(&(ioOutputTab[aOffset]), &aFinalValue, iFullFormat.bytesPerComponent);
					ioColorBitMap[aPixelBlockPos] = true;
				}
				--aCode;
			}
			++aPixelBlockPos;
		}

		while (aPixelBlockPos < iFullFormat.nbObPixelBlocks && ioColorBitMap[aPixelBlockPos])
		{
			++aPixelBlockPos;
		}
    }
}

void inflateData(State& iState, const FullFormat& iFullFormat, uint32_t ioOutputSize, uint8_t* ioOutputTab)
{
	// Bitmaps
	std::vector<bool> aColorBitmap;
	std::vector<bool> aAlphaBitmap;

	uint32_t aChunkStartPosition = iState.inputPos;

	iState.inputPos = aChunkStartPosition;

	iState.head = 0;
    iState.bits = 0;
    iState.buffer = 0;

	// Getting size of compressed data
	needBits(iState, 32);
	uint32_t aDataSize = readBits(iState, 32);
	dropBits(iState, 32);

	// Compression Flags
	needBits(iState, 32);
	uint32_t aCompressionFlags = readBits(iState, 32);
	dropBits(iState, 32);

	aColorBitmap.assign(iFullFormat.nbObPixelBlocks, false);
	aAlphaBitmap.assign(iFullFormat.nbObPixelBlocks, false);

	if (aCompressionFlags & CF_DECODE_WHITE_COLOR)
	{
		decodeWhiteColor(iState, aAlphaBitmap, aColorBitmap, iFullFormat, ioOutputTab);
	}

	if (aCompressionFlags & CF_DECODE_CONSTANT_ALPHA_FROM4BITS)
	{
		decodeConstantAlphaFrom4Bits(iState, aAlphaBitmap, iFullFormat, ioOutputTab);
	}

	if (aCompressionFlags & CF_DECODE_CONSTANT_ALPHA_FROM8BITS)
	{
		decodeConstantAlphaFrom8Bits(iState, aAlphaBitmap, iFullFormat, ioOutputTab);
	}

	if (aCompressionFlags & CF_DECODE_PLAIN_COLOR)
	{
		decodePlainColor(iState, aColorBitmap, iFullFormat, ioOutputTab);
	}	

	uint32_t aLoopIndex;

	if (iState.bits >= 32)
	{
		--iState.inputPos;
	}

	if ((((iFullFormat.format.flags) & FF_ALPHA) && !((iFullFormat.format.flags) & FF_DEDUCEDALPHACOMP)) || (iFullFormat.format.flags) & FF_BICOLORCOMP)
	{
		for (aLoopIndex = 0; aLoopIndex < aAlphaBitmap.size() && iState.inputPos < iState.inputSize; ++aLoopIndex)
		{
			if (!aAlphaBitmap[aLoopIndex])
			{
				(*reinterpret_cast<uint32_t*>(&(ioOutputTab[iFullFormat.bytesPerPixelBlock * aLoopIndex]))) = iState.input[iState.inputPos];
				++iState.inputPos;
				if (iFullFormat.bytesPerComponent > 4)
				{
					(*reinterpret_cast<uint32_t*>(&(ioOutputTab[iFullFormat.bytesPerPixelBlock * aLoopIndex + 4]))) = iState.input[iState.inputPos];
					++iState.inputPos;
				}
			}
		}
	}

	if ((iFullFormat.format.flags) & FF_COLOR || (iFullFormat.format.flags) & FF_BICOLORCOMP)
	{
		for (aLoopIndex = 0; aLoopIndex < aColorBitmap.size() && iState.inputPos < iState.inputSize; ++aLoopIndex)
		{
			if (!aColorBitmap[aLoopIndex])
			{
				uint32_t aOffset = iFullFormat.bytesPerPixelBlock * aLoopIndex + (iFullFormat.hasTwoComponents ? iFullFormat.bytesPerComponent : 0);
				(*reinterpret_cast<uint32_t*>(&(ioOutputTab[aOffset]))) = iState.input[iState.inputPos];
				++iState.inputPos;
			}
		}
		if (iFullFormat.bytesPerComponent > 4)
		{
			for (aLoopIndex = 0; aLoopIndex < aColorBitmap.size() && iState.inputPos < iState.inputSize; ++aLoopIndex)
			{
				if (!aColorBitmap[aLoopIndex])
				{
					uint32_t aOffset = iFullFormat.bytesPerPixelBlock * aLoopIndex + 4 + (iFullFormat.hasTwoComponents ? iFullFormat.bytesPerComponent : 0);
					(*reinterpret_cast<uint32_t*>(&(ioOutputTab[aOffset]))) = iState.input[iState.inputPos];
					++iState.inputPos;
				}
			}
		}
	}
}
}

GW2DATTOOLS_API uint8_t* GW2DATTOOLS_APIENTRY inflateTextureFileBuffer(uint32_t iInputSize, const uint8_t* iInputTab,  uint32_t& ioOutputSize, uint8_t* ioOutputTab)
{
    if (iInputTab == nullptr)
    {
        throw exception::Exception("Input buffer is null.");
    }

    if (ioOutputTab != nullptr && ioOutputSize == 0)
    {
        throw exception::Exception("Output buffer is not null and outputSize is not defined.");
    }

    uint8_t* anOutputTab(nullptr);
    bool isOutputTabOwned(true);

    try
    {
        if (!texture::sStaticValuesInitialized)
        {
            texture::initializeStaticValues();
            texture::sStaticValuesInitialized = true;
        }
        
        // Initialize state
        State aState;
        aState.input = reinterpret_cast<const uint32_t*>(iInputTab);
        aState.inputSize = iInputSize / 4;
        aState.inputPos = 0;

        aState.head = 0;
        aState.bits = 0;
        aState.buffer = 0;

		aState.isEmpty = false;

        // Skipping header
        needBits(aState, 32);
        dropBits(aState, 32);
		
		// Format
		needBits(aState, 32);
		uint32_t aFormatFourCc = readBits(aState, 32);
		dropBits(aState, 32);

		texture::FullFormat aFullFormat;

		aFullFormat.format = texture::deduceFormat(aFormatFourCc);
        
        // Getting width/height
        needBits(aState, 32);
		aFullFormat.width = readBits(aState, 16);
		dropBits(aState, 16);
		aFullFormat.height = readBits(aState, 16);
		dropBits(aState, 16);

		aFullFormat.nbObPixelBlocks = ((aFullFormat.width + 3) / 4) * ((aFullFormat.height + 3) / 4);
		aFullFormat.bytesPerPixelBlock = (aFullFormat.format.pixelSizeInBits * 4 * 4) / 8;
		aFullFormat.hasTwoComponents = 
			((aFullFormat.format.flags & (texture::FF_PLAINCOMP | texture::FF_COLOR | texture::FF_ALPHA)) == (texture::FF_PLAINCOMP | texture::FF_COLOR | texture::FF_ALPHA))
			|| (aFullFormat.format.flags & texture::FF_BICOLORCOMP);

		aFullFormat.bytesPerComponent = aFullFormat.bytesPerPixelBlock / (aFullFormat.hasTwoComponents ? 2 : 1);
		
		uint32_t anOutputSize = aFullFormat.bytesPerPixelBlock * aFullFormat.nbObPixelBlocks;

		if (ioOutputSize != 0 && ioOutputSize < anOutputSize)
        {
            throw exception::Exception("Output buffer is too small.");
        }

        ioOutputSize = anOutputSize;

        if (ioOutputTab == nullptr)
        {
            anOutputTab = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * anOutputSize));
        }
        else
        {
            isOutputTabOwned = false;
            anOutputTab = ioOutputTab;
        }

		texture::inflateData(aState, aFullFormat, ioOutputSize, anOutputTab);
        
        return anOutputTab;
    }
    catch(exception::Exception& iException)
    {
        if (isOutputTabOwned)
        {
            free(anOutputTab);
        }
        throw iException; // Rethrow exception
    }
    catch(std::exception& iException)
    {
        if (isOutputTabOwned)
        {
            free(anOutputTab);
        }
        throw iException; // Rethrow exception
    }
}

GW2DATTOOLS_API uint8_t* GW2DATTOOLS_APIENTRY inflateTextureBlockBuffer(uint16_t iWidth, uint16_t iHeight, uint32_t iFormatFourCc, uint32_t iInputSize, const uint8_t* iInputTab, 
    uint32_t& ioOutputSize, uint8_t* ioOutputTab)
{
    if (iInputTab == nullptr)
    {
        throw exception::Exception("Input buffer is null.");
    }

    if (ioOutputTab != nullptr && ioOutputSize == 0)
    {
        throw exception::Exception("Output buffer is not null and outputSize is not defined.");
    }

    uint8_t* anOutputTab(nullptr);
    bool isOutputTabOwned(true);

    try
    {
        if (!texture::sStaticValuesInitialized)
        {
            texture::initializeStaticValues();
            texture::sStaticValuesInitialized = true;
        }

        // Initialize format
		texture::FullFormat aFullFormat;

		aFullFormat.format = texture::deduceFormat(iFormatFourCc);
        aFullFormat.width  = iWidth;
        aFullFormat.height = iHeight;

        aFullFormat.nbObPixelBlocks = ((aFullFormat.width + 3) / 4) * ((aFullFormat.height + 3) / 4);
        aFullFormat.bytesPerPixelBlock = (aFullFormat.format.pixelSizeInBits * 4 * 4) / 8;
        aFullFormat.hasTwoComponents = 
            ((aFullFormat.format.flags & (texture::FF_PLAINCOMP | texture::FF_COLOR | texture::FF_ALPHA)) == (texture::FF_PLAINCOMP | texture::FF_COLOR | texture::FF_ALPHA))
            || (aFullFormat.format.flags & texture::FF_BICOLORCOMP);

        aFullFormat.bytesPerComponent = aFullFormat.bytesPerPixelBlock / (aFullFormat.hasTwoComponents ? 2 : 1);
        
        // Initialize state
        State aState;
        aState.input = reinterpret_cast<const uint32_t*>(iInputTab);
        aState.inputSize = iInputSize / 4;
        aState.inputPos = 0;

        aState.head = 0;
        aState.bits = 0;
        aState.buffer = 0;

		aState.isEmpty = false;

        // Allocate output buffer
		uint32_t anOutputSize = aFullFormat.bytesPerPixelBlock * aFullFormat.nbObPixelBlocks;

		if (ioOutputSize != 0 && ioOutputSize < anOutputSize)
        {
            throw exception::Exception("Output buffer is too small.");
        }

        ioOutputSize = anOutputSize;

        if (ioOutputTab == nullptr)
        {
            anOutputTab = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * anOutputSize));
        }
        else
        {
            isOutputTabOwned = false;
            anOutputTab = ioOutputTab;
        }

		texture::inflateData(aState, aFullFormat, ioOutputSize, anOutputTab);
        
        return anOutputTab;
    }
    catch(exception::Exception& iException)
    {
        if (isOutputTabOwned)
        {
            free(anOutputTab);
        }
        throw iException; // Rethrow exception
    }
    catch(std::exception& iException)
    {
        if (isOutputTabOwned)
        {
            free(anOutputTab);
        }
        throw iException; // Rethrow exception
    }
}

}
}
