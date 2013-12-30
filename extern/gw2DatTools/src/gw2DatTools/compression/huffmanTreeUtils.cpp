#include "huffmanTreeUtils.h"

#include <memory.h>

namespace gw2dt
{
namespace compression
{

void readCode(const HuffmanTree& iHuffmanTree, State& ioState, uint16_t& ioCode)
{    
    if (iHuffmanTree.isEmpty)
    {
        throw exception::Exception("Trying to read code from an empty HuffmanTree.");
    }

    needBits(ioState, 32);

    if (iHuffmanTree.symbolValueHashTab[readBits(ioState, MaxNbBitsHash)] != -1)
    {
        ioCode = iHuffmanTree.symbolValueHashTab[readBits(ioState, MaxNbBitsHash)];
        dropBits(ioState, iHuffmanTree.codeBitsHashTab[readBits(ioState, MaxNbBitsHash)]);
    }
    else
    {
        uint16_t anIndex = 0;
        while (readBits(ioState, 32) < iHuffmanTree.codeCompTab[anIndex])
        {
            ++anIndex;
        }

        uint8_t aNbBits = iHuffmanTree.codeBitsTab[anIndex];
        ioCode = iHuffmanTree.symbolValueTab[iHuffmanTree.symbolValueTabOffsetTab[anIndex] -
                                             ((readBits(ioState, 32) - iHuffmanTree.codeCompTab[anIndex]) >> (32 - aNbBits))];
        dropBits(ioState, aNbBits);
    }
}

void buildHuffmanTree(HuffmanTree& ioHuffmanTree, int16_t* ioWorkingBitTab, int16_t* ioWorkingCodeTab)
{
    // Resetting Huffmantrees
    memset(&ioHuffmanTree.codeCompTab, 0, sizeof(ioHuffmanTree.codeCompTab));
    memset(&ioHuffmanTree.symbolValueTabOffsetTab, 0, sizeof(ioHuffmanTree.symbolValueTabOffsetTab));
    memset(&ioHuffmanTree.symbolValueTab, 0, sizeof(ioHuffmanTree.symbolValueTab));
    memset(&ioHuffmanTree.codeBitsTab, 0, sizeof(ioHuffmanTree.codeBitsTab));
    memset(&ioHuffmanTree.codeBitsHashTab, 0, sizeof(ioHuffmanTree.codeBitsHashTab));

    memset(&ioHuffmanTree.symbolValueHashTab, 0xFF, sizeof(ioHuffmanTree.symbolValueHashTab));

    ioHuffmanTree.isEmpty = true;

    // Building the HuffmanTree
    uint32_t aCode = 0;
    uint8_t aNbBits = 0;

    // First part, filling hashTable for codes that are of less than 8 bits
    while (aNbBits <= MaxNbBitsHash)
    {
        if (ioWorkingBitTab[aNbBits] != -1)
        {
            ioHuffmanTree.isEmpty = false;

            int16_t aCurrentSymbol = ioWorkingBitTab[aNbBits];
            while (aCurrentSymbol != -1)
            {
                // Processing hash values
                uint16_t aHashValue = aCode << (MaxNbBitsHash - aNbBits);
                uint16_t aNextHashValue = (aCode + 1) << (MaxNbBitsHash - aNbBits);

                while (aHashValue < aNextHashValue)
                {
                    ioHuffmanTree.symbolValueHashTab[aHashValue] = aCurrentSymbol;
                    ioHuffmanTree.codeBitsHashTab[aHashValue] = aNbBits;
                    ++aHashValue;
                }

                aCurrentSymbol = ioWorkingCodeTab[aCurrentSymbol];
                --aCode;
            }
        }
        aCode = (aCode << 1) + 1;
        ++aNbBits;
    }

    uint16_t aCodeCompTabIndex = 0;
    uint16_t aSymbolOffset = 0;

    // Second part, filling classical structure for other codes
    while (aNbBits < MaxCodeBitsLength)
    {
        if (ioWorkingBitTab[aNbBits] != -1)
        {
            ioHuffmanTree.isEmpty = false;

            int16_t aCurrentSymbol = ioWorkingBitTab[aNbBits];
            while (aCurrentSymbol != -1)
            {
                // Registering the code
                ioHuffmanTree.symbolValueTab[aSymbolOffset] = aCurrentSymbol;

                ++aSymbolOffset;
                aCurrentSymbol = ioWorkingCodeTab[aCurrentSymbol];
                --aCode;
            }

            // Minimum code value for aNbBits bits
            ioHuffmanTree.codeCompTab[aCodeCompTabIndex] = ((aCode + 1) << (32 - aNbBits));

            // Number of bits for l_codeCompIndex index
            ioHuffmanTree.codeBitsTab[aCodeCompTabIndex] = aNbBits;

            // Offset in symbolValueTab table to reach the value
            ioHuffmanTree.symbolValueTabOffsetTab[aCodeCompTabIndex] = aSymbolOffset - 1;

            ++aCodeCompTabIndex;
        }
        aCode = (aCode << 1) + 1;
        ++aNbBits;
    }
}

void fillWorkingTabsHelper(const uint8_t iBits, const int16_t iSymbol, int16_t* ioWorkingBitTab, int16_t* ioWorkingCodeTab)
{
    // checking out of bounds
    if (iBits >= MaxCodeBitsLength)
    {
        throw exception::Exception("Too many bits.");
    }
    
    if (iSymbol >= MaxSymbolValue)
    {
        throw exception::Exception("Too high symbol.");
    }

    if (ioWorkingBitTab[iBits] == -1)
    {
        ioWorkingBitTab[iBits] = iSymbol;
    }
    else
    {
        ioWorkingCodeTab[iSymbol] = ioWorkingBitTab[iBits];
        ioWorkingBitTab[iBits] = iSymbol;
    }
}

}
}
