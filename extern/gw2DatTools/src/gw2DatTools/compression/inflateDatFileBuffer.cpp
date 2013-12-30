#include "gw2DatTools/compression/inflateDatFileBuffer.h"

#include <memory.h>
#include <iostream>

#include "gw2DatTools/exception/Exception.h"

#include "HuffmanTree.h"
#include "../utils/BitArray.h"

namespace gw2dt
{
namespace compression
{
namespace dat
{

const uint32_t sDatFileNbBitsHash        = 8;
const uint32_t sDatFileMaxCodeBitsLength = 32;
const uint32_t sDatFileMaxSymbolValue    = 285;

typedef utils::BitArray<uint32_t> DatFileBitArray;
typedef HuffmanTree<uint16_t, sDatFileNbBitsHash, sDatFileMaxCodeBitsLength, sDatFileMaxSymbolValue> DatFileHuffmanTree;
typedef HuffmanTreeBuilder<uint16_t, sDatFileMaxCodeBitsLength, sDatFileMaxSymbolValue> DatFileHuffmanTreeBuilder;

static DatFileHuffmanTree sDatFileHuffmanTreeDict;

// Parse and build a huffmanTree
bool parseHuffmanTree(DatFileBitArray& ioInputBitArray, DatFileHuffmanTree& ioHuffmanTree, DatFileHuffmanTreeBuilder& ioHuffmanTreeBuilder)
{
    // Reading the number of symbols to read
    uint16_t aNumberOfSymbols;
    ioInputBitArray.read(aNumberOfSymbols);
    ioInputBitArray.drop<uint16_t>();

    if (aNumberOfSymbols > sDatFileMaxSymbolValue)
    {
        throw exception::Exception("Too many symbols to decode.");
    }

    ioHuffmanTreeBuilder.clear();

    int16_t aRemainingSymbols = aNumberOfSymbols - 1;

    // Fetching the code repartition
    while (aRemainingSymbols >= 0)
    {
        uint16_t aCode;
        
        sDatFileHuffmanTreeDict.readCode(ioInputBitArray, aCode);

        uint8_t aCodeNumberOfBits = aCode & 0x1F;
        uint16_t aCodeNumberOfSymbols = (aCode >> 5) + 1;

        if (aCodeNumberOfBits == 0)
        {
            aRemainingSymbols -= aCodeNumberOfSymbols;
        }
        else
        {
            while (aCodeNumberOfSymbols > 0)
            {
                ioHuffmanTreeBuilder.addSymbol(aRemainingSymbols, aCodeNumberOfBits);
                --aRemainingSymbols;
                --aCodeNumberOfSymbols;
            }
        }
    }
    
    return ioHuffmanTreeBuilder.buildHuffmanTree(ioHuffmanTree);
}

void inflatedata(DatFileBitArray& ioInputBitArray, uint32_t iOutputSize,  uint8_t* ioOutputTab)
{
    uint32_t anOutputPos = 0;

    // Reading the const write size addition value
    ioInputBitArray.drop<4>();
    uint16_t aWriteSizeConstAdd;
    ioInputBitArray.read<4>(aWriteSizeConstAdd);
    aWriteSizeConstAdd += 1;
    ioInputBitArray.drop<4>();

    // Declaring our HuffmanTrees
    DatFileHuffmanTree aHuffmanTreeSymbol;
    DatFileHuffmanTree aHuffmanTreeCopy;
    
    DatFileHuffmanTreeBuilder aHuffmanTreeBuilder;

    while (anOutputPos < iOutputSize)
    {
        // Reading HuffmanTrees
        if (   !parseHuffmanTree(ioInputBitArray, aHuffmanTreeSymbol, aHuffmanTreeBuilder)
            || !parseHuffmanTree(ioInputBitArray, aHuffmanTreeCopy, aHuffmanTreeBuilder))
        {
            break;
        }

        // Reading MaxCount
        uint32_t aMaxCount;
        ioInputBitArray.read<4>(aMaxCount);
        aMaxCount = (aMaxCount + 1) << 12;
        ioInputBitArray.drop<4>();

        uint32_t aCurrentCodeReadCount = 0;

        while ((aCurrentCodeReadCount < aMaxCount) &&
               (anOutputPos < iOutputSize))
        {
            ++aCurrentCodeReadCount;

            // Reading next code
            uint16_t aSymbol = 0;
            aHuffmanTreeSymbol.readCode(ioInputBitArray, aSymbol);

            if (aSymbol < 0x100)
            {
                ioOutputTab[anOutputPos] = static_cast<uint8_t>(aSymbol);
                ++anOutputPos;
                continue;
            }

            // We are in copy mode !
            // Reading the additional info to know the write size
            aSymbol -= 0x100;

            // write size
            div_t aCodeDiv4 = div(aSymbol, 4);

            uint32_t aWriteSize = 0;
            if (aCodeDiv4.quot == 0)
            {
                aWriteSize = aSymbol;
            }
            else if (aCodeDiv4.quot < 7)
            {
                aWriteSize = ((1 << (aCodeDiv4.quot - 1)) * (4 + aCodeDiv4.rem));
            }
            else if (aSymbol == 28)
            {
                aWriteSize = 0xFF;
            }
            else
            {
                throw exception::Exception("Invalid value for writeSize code.");
            }

            //additional bits
            if (aCodeDiv4.quot > 1 && aSymbol != 28)
            {
                uint8_t aWriteSizeAddBits = aCodeDiv4.quot - 1;
                uint32_t aWriteSizeAdd;
                ioInputBitArray.read(aWriteSizeAddBits, aWriteSizeAdd);
                aWriteSize |= aWriteSizeAdd;
                ioInputBitArray.drop(aWriteSizeAddBits);
            }
            aWriteSize += aWriteSizeConstAdd;

            // write offset
            // Reading the write offset
            aHuffmanTreeCopy.readCode(ioInputBitArray, aSymbol);

            div_t aCodeDiv2 = div(aSymbol, 2);

            uint32_t aWriteOffset = 0;
            if (aCodeDiv2.quot == 0)
            {
                aWriteOffset = aSymbol;
            }
            else if (aCodeDiv2.quot < 17)
            {
                aWriteOffset = ((1 << (aCodeDiv2.quot - 1)) * (2 + aCodeDiv2.rem));
            }
            else
            {
                throw exception::Exception("Invalid value for writeOffset code.");
            }

            //additional bits
            if (aCodeDiv2.quot > 1)
            {
                uint8_t aWriteOffsetAddBits = aCodeDiv2.quot - 1;
                uint32_t aWriteOffsetAdd;
                ioInputBitArray.read(aWriteOffsetAddBits, aWriteOffsetAdd);
                aWriteOffset |= aWriteOffsetAdd;
                ioInputBitArray.drop(aWriteOffsetAddBits);
            }
            aWriteOffset += 1;

            uint32_t anAlreadyWritten = 0;
            while ((anAlreadyWritten < aWriteSize) &&
                   (anOutputPos < iOutputSize))
            {
                ioOutputTab[anOutputPos] = ioOutputTab[anOutputPos - aWriteOffset];
                ++anOutputPos;
                ++anAlreadyWritten;
            }
        }
    }
}
}

GW2DATTOOLS_API uint8_t* GW2DATTOOLS_APIENTRY inflateDatFileBuffer(uint32_t iInputSize, const uint8_t* iInputTab,  uint32_t& ioOutputSize, uint8_t* ioOutputTab)
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
        dat::DatFileBitArray anInputBitArray(iInputTab, iInputSize, 16384); // Skipping four bytes every 65k chunk

        // Skipping header & Getting size of the uncompressed data
        anInputBitArray.drop<uint32_t>();
        
        // Getting size of the uncompressed data
        uint32_t anOutputSize;
        anInputBitArray.read(anOutputSize);
        anInputBitArray.drop<uint32_t>();

        if (ioOutputSize != 0)
        {
            anOutputSize = std::min(anOutputSize, ioOutputSize);
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

        dat::inflatedata(anInputBitArray, anOutputSize, anOutputTab);
        
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

class DatFileHuffmanTreeDictStaticInitializer
{
    public:
        DatFileHuffmanTreeDictStaticInitializer(dat::DatFileHuffmanTree& ioHuffmanTree);
};

DatFileHuffmanTreeDictStaticInitializer::DatFileHuffmanTreeDictStaticInitializer(dat::DatFileHuffmanTree& ioHuffmanTree)
{
    dat::DatFileHuffmanTreeBuilder aDatFileHuffmanTreeBuilder;
    aDatFileHuffmanTreeBuilder.clear();

    aDatFileHuffmanTreeBuilder.addSymbol(0x0A, 3);
    aDatFileHuffmanTreeBuilder.addSymbol(0x09, 3);
    aDatFileHuffmanTreeBuilder.addSymbol(0x08, 3);

    aDatFileHuffmanTreeBuilder.addSymbol(0x0C, 4);
    aDatFileHuffmanTreeBuilder.addSymbol(0x0B, 4);
    aDatFileHuffmanTreeBuilder.addSymbol(0x07, 4);
    aDatFileHuffmanTreeBuilder.addSymbol(0x00, 4);

    aDatFileHuffmanTreeBuilder.addSymbol(0xE0, 5);
    aDatFileHuffmanTreeBuilder.addSymbol(0x2A, 5);
    aDatFileHuffmanTreeBuilder.addSymbol(0x29, 5);
    aDatFileHuffmanTreeBuilder.addSymbol(0x06, 5);

    aDatFileHuffmanTreeBuilder.addSymbol(0x4A, 6);
    aDatFileHuffmanTreeBuilder.addSymbol(0x40, 6);
    aDatFileHuffmanTreeBuilder.addSymbol(0x2C, 6);
    aDatFileHuffmanTreeBuilder.addSymbol(0x2B, 6);
    aDatFileHuffmanTreeBuilder.addSymbol(0x28, 6);
    aDatFileHuffmanTreeBuilder.addSymbol(0x20, 6);
    aDatFileHuffmanTreeBuilder.addSymbol(0x05, 6);
    aDatFileHuffmanTreeBuilder.addSymbol(0x04, 6);

    aDatFileHuffmanTreeBuilder.addSymbol(0x49, 7);
    aDatFileHuffmanTreeBuilder.addSymbol(0x48, 7);
    aDatFileHuffmanTreeBuilder.addSymbol(0x27, 7);
    aDatFileHuffmanTreeBuilder.addSymbol(0x26, 7);
    aDatFileHuffmanTreeBuilder.addSymbol(0x25, 7);
    aDatFileHuffmanTreeBuilder.addSymbol(0x0D, 7);
    aDatFileHuffmanTreeBuilder.addSymbol(0x03, 7);

    aDatFileHuffmanTreeBuilder.addSymbol(0x6A, 8);
    aDatFileHuffmanTreeBuilder.addSymbol(0x69, 8);
    aDatFileHuffmanTreeBuilder.addSymbol(0x4C, 8);
    aDatFileHuffmanTreeBuilder.addSymbol(0x4B, 8);
    aDatFileHuffmanTreeBuilder.addSymbol(0x47, 8);
    aDatFileHuffmanTreeBuilder.addSymbol(0x24, 8);

    aDatFileHuffmanTreeBuilder.addSymbol(0xE8, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA0, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0x89, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0x88, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0x68, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0x67, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0x63, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0x60, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0x46, 9);
    aDatFileHuffmanTreeBuilder.addSymbol(0x23, 9);

    aDatFileHuffmanTreeBuilder.addSymbol(0xE9, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC9, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC0, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA9, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA8, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x8A, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x87, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x80, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x66, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x65, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x45, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x44, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x43, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x2D, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x02, 10);
    aDatFileHuffmanTreeBuilder.addSymbol(0x01, 10);

    aDatFileHuffmanTreeBuilder.addSymbol(0xE5, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC8, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0xAA, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA5, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA4, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0x8B, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0x85, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0x84, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0x6C, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0x6B, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0x64, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0x4D, 11);
    aDatFileHuffmanTreeBuilder.addSymbol(0x0E, 11);

    aDatFileHuffmanTreeBuilder.addSymbol(0xE7, 12);
    aDatFileHuffmanTreeBuilder.addSymbol(0xCA, 12);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC7, 12);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA7, 12);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA6, 12);
    aDatFileHuffmanTreeBuilder.addSymbol(0x86, 12);
    aDatFileHuffmanTreeBuilder.addSymbol(0x83, 12);

    aDatFileHuffmanTreeBuilder.addSymbol(0xE6, 13);
    aDatFileHuffmanTreeBuilder.addSymbol(0xE4, 13);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC4, 13);
    aDatFileHuffmanTreeBuilder.addSymbol(0x8C, 13);
    aDatFileHuffmanTreeBuilder.addSymbol(0x2E, 13);
    aDatFileHuffmanTreeBuilder.addSymbol(0x22, 13);

    aDatFileHuffmanTreeBuilder.addSymbol(0xEC, 14);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC6, 14);
    aDatFileHuffmanTreeBuilder.addSymbol(0x6D, 14);
    aDatFileHuffmanTreeBuilder.addSymbol(0x4E, 14);

    aDatFileHuffmanTreeBuilder.addSymbol(0xEA, 15);
    aDatFileHuffmanTreeBuilder.addSymbol(0xCC, 15);
    aDatFileHuffmanTreeBuilder.addSymbol(0xAC, 15);
    aDatFileHuffmanTreeBuilder.addSymbol(0xAB, 15);
    aDatFileHuffmanTreeBuilder.addSymbol(0x8D, 15);
    aDatFileHuffmanTreeBuilder.addSymbol(0x11, 15);
    aDatFileHuffmanTreeBuilder.addSymbol(0x10, 15);
    aDatFileHuffmanTreeBuilder.addSymbol(0x0F, 15);

    aDatFileHuffmanTreeBuilder.addSymbol(0xFF, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xFE, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xFD, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xFC, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xFB, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xFA, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF9, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF8, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF7, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF6, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF5, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF4, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF3, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF2, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF1, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xF0, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xEF, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xEE, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xED, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xEB, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xE3, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xE2, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xE1, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xDF, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xDE, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xDD, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xDC, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xDB, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xDA, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD9, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD8, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD7, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD6, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD5, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD4, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD3, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD2, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD1, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xD0, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xCF, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xCE, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xCD, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xCB, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC5, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC3, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC2, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xC1, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xBF, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xBE, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xBD, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xBC, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xBB, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xBA, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB9, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB8, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB7, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB6, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB5, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB4, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB3, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB2, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB1, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xB0, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xAF, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xAE, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xAD, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA3, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA2, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0xA1, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x9F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x9E, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x9D, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x9C, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x9B, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x9A, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x99, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x98, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x97, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x96, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x95, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x94, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x93, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x92, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x91, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x90, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x8F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x8E, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x82, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x81, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x7F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x7E, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x7D, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x7C, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x7B, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x7A, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x79, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x78, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x77, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x76, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x75, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x74, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x73, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x72, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x71, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x70, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x6F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x6E, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x62, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x61, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x5F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x5E, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x5D, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x5C, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x5B, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x5A, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x59, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x58, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x57, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x56, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x55, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x54, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x53, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x52, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x51, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x50, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x4F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x42, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x41, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x3F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x3E, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x3D, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x3C, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x3B, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x3A, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x39, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x38, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x37, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x36, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x35, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x34, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x33, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x32, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x31, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x30, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x2F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x21, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x1F, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x1E, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x1D, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x1C, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x1B, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x1A, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x19, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x18, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x17, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x16, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x15, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x14, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x13, 16);
    aDatFileHuffmanTreeBuilder.addSymbol(0x12, 16);

    aDatFileHuffmanTreeBuilder.buildHuffmanTree(ioHuffmanTree);
}

static DatFileHuffmanTreeDictStaticInitializer aDatFileHuffmanTreeDictStaticInitializer(dat::sDatFileHuffmanTreeDict);

}
}
