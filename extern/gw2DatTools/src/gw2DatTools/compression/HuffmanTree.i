#ifndef GW2DATTOOLS_COMPRESSION_HUFFMANTREE_I
#define GW2DATTOOLS_COMPRESSION_HUFFMANTREE_I

#include "../utils/BitArray.h"

namespace gw2dt
{
namespace compression
{

template <typename SymbolType, 
          uint8_t sNbBitsHash, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
void HuffmanTree<SymbolType, sNbBitsHash, sMaxCodeBitsLength, sMaxSymbolValue>::clear()
{
    _codeComparisonArray.fill(0);
    _symbolValueArrayOffsetArray.fill(0);
    _symbolValueArray.fill(0);
    _codeBitsArray.fill(0);
    
    _symbolValueHashExistenceArray.fill(false);
    _symbolValueHashArray.fill(0);
    _codeBitsHashArray.fill(0);
}

template <typename SymbolType, 
          uint8_t sNbBitsHash, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
template <typename IntType>
void HuffmanTree<SymbolType, sNbBitsHash, sMaxCodeBitsLength, sMaxSymbolValue>::readCode(utils::BitArray<IntType>& iBitArray, SymbolType& oSymbol) const
{
    uint32_t aHashValue;
    iBitArray.readLazy<sNbBitsHash>(aHashValue);
    
    if (_symbolValueHashExistenceArray[aHashValue])
    {
        oSymbol = _symbolValueHashArray[aHashValue];
        iBitArray.drop(_codeBitsHashArray[aHashValue]);
    }
    else
    {
        iBitArray.readLazy(aHashValue);
        
        uint16_t anIndex = 0;
        while (aHashValue < _codeComparisonArray[anIndex])
        {
            ++anIndex;
        }
        
        uint8_t aNbBits = _codeBitsArray[anIndex];
        oSymbol = _symbolValueArray[_symbolValueArrayOffsetArray[anIndex] -
                                        ((aHashValue - _codeComparisonArray[anIndex]) >> (32 - aNbBits))];
        iBitArray.drop(aNbBits);
    }
}

template <typename SymbolType, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
void HuffmanTreeBuilder<SymbolType, sMaxCodeBitsLength, sMaxSymbolValue>::clear()
{
    _symbolListByBitsHeadExistenceArray.fill(false);
    _symbolListByBitsHeadArray.fill(0);
    
    _symbolListByBitsBodyExistenceArray.fill(false);
    _symbolListByBitsBodyArray.fill(0);
}

template <typename SymbolType, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
void HuffmanTreeBuilder<SymbolType, sMaxCodeBitsLength, sMaxSymbolValue>::addSymbol(SymbolType iSymbol, uint8_t iNbBits)
{
    if (_symbolListByBitsHeadExistenceArray[iNbBits])
    {
        _symbolListByBitsBodyArray[iSymbol] = _symbolListByBitsHeadArray[iNbBits];
        _symbolListByBitsBodyExistenceArray[iSymbol] = true;
        _symbolListByBitsHeadArray[iNbBits] = iSymbol;
    }
    else
    {
        _symbolListByBitsHeadArray[iNbBits] = iSymbol;
        _symbolListByBitsHeadExistenceArray[iNbBits] = true;
    }
}

template <typename SymbolType, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
bool HuffmanTreeBuilder<SymbolType, sMaxCodeBitsLength, sMaxSymbolValue>::empty() const
{
    for (auto it = _symbolListByBitsHeadExistenceArray.begin(); it != _symbolListByBitsHeadExistenceArray.end(); ++it)
    {
        if (*it == true)
        {
            return false;
        }
    }
    return true;
}

template <typename SymbolType, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
template <uint8_t sNbBitsHash>
bool HuffmanTreeBuilder<SymbolType, sMaxCodeBitsLength, sMaxSymbolValue>::buildHuffmanTree(HuffmanTree<SymbolType, sNbBitsHash, sMaxCodeBitsLength, sMaxSymbolValue>& oHuffmanTree)
{
    if (empty())
    {
        return false;
    }
    
    oHuffmanTree.clear();
    
    // Building the HuffmanTree
    uint32_t aCode = 0;
    uint8_t aNbBits = 0;

    // First part, filling hashTable for codes that are of less than 8 bits
    while (aNbBits <= sNbBitsHash)
    {
        bool anExistence = _symbolListByBitsHeadExistenceArray[aNbBits];
        
        if (anExistence)
        {
            SymbolType aCurrentSymbol = _symbolListByBitsHeadArray[aNbBits];
            
            while (anExistence)
            {
                // Processing hash values
                uint16_t aHashValue = aCode << (sNbBitsHash - aNbBits);
                uint16_t aNextHashValue = (aCode + 1) << (sNbBitsHash - aNbBits);

                while (aHashValue < aNextHashValue)
                {
                    oHuffmanTree._symbolValueHashExistenceArray[aHashValue] = true;
                    oHuffmanTree._symbolValueHashArray[aHashValue] = aCurrentSymbol;
                    oHuffmanTree._codeBitsHashArray[aHashValue] = aNbBits;
                    ++aHashValue;
                }

                anExistence = _symbolListByBitsBodyExistenceArray[aCurrentSymbol];
                aCurrentSymbol = _symbolListByBitsBodyArray[aCurrentSymbol];
                --aCode;
            }
        }
        
        aCode = (aCode << 1) + 1;
        ++aNbBits;
    }
    
    uint16_t aCodeComparisonArrayIndex = 0;
    uint16_t aSymbolOffset = 0;

    // Second part, filling classical structure for other codes
    while (aNbBits < sMaxCodeBitsLength)
    {
        bool anExistence = _symbolListByBitsHeadExistenceArray[aNbBits];
        
        if (anExistence)
        {
            SymbolType aCurrentSymbol = _symbolListByBitsHeadArray[aNbBits];
            
            while (anExistence)
            {
                // Registering the code
                oHuffmanTree._symbolValueArray[aSymbolOffset] = aCurrentSymbol;

                ++aSymbolOffset;
                anExistence = _symbolListByBitsBodyExistenceArray[aCurrentSymbol];
                aCurrentSymbol = _symbolListByBitsBodyArray[aCurrentSymbol];
                --aCode;
            }

            // Minimum code value for aNbBits bits
            oHuffmanTree._codeComparisonArray[aCodeComparisonArrayIndex] = ((aCode + 1) << (32 - aNbBits));

            // Number of bits for l_codeCompIndex index
            oHuffmanTree._codeBitsArray[aCodeComparisonArrayIndex] = aNbBits;

            // Offset in symbolValueTab table to reach the value
            oHuffmanTree._symbolValueArrayOffsetArray[aCodeComparisonArrayIndex] = aSymbolOffset - 1;

            ++aCodeComparisonArrayIndex;
        }
        
        aCode = (aCode << 1) + 1;
        ++aNbBits;
    }
    
    return true;
}

}
}

#endif // GW2DATTOOLS_COMPRESSION_HUFFMANTREE_I
