#ifndef GW2DATTOOLS_COMPRESSION_HUFFMANTREE_H
#define GW2DATTOOLS_COMPRESSION_HUFFMANTREE_H

#include <array>
#include <cstdint>

#include "../utils/BitArray.h"

namespace gw2dt
{
namespace compression
{

template <typename SymbolType, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
class HuffmanTreeBuilder;

// Assumption: code length <= 32
template <typename SymbolType, 
          uint8_t sNbBitsHash, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
class HuffmanTree
{
    public:
        friend class HuffmanTreeBuilder<SymbolType, sMaxCodeBitsLength, sMaxSymbolValue>;
        
        template <typename IntType>
        void readCode(utils::BitArray<IntType>& iBitArray, SymbolType& oSymbol) const;
        
    private:
        void clear();
        
        std::array<uint32_t, sMaxCodeBitsLength>   _codeComparisonArray;
        std::array<uint16_t, sMaxCodeBitsLength>   _symbolValueArrayOffsetArray;
        std::array<SymbolType, sMaxSymbolValue>    _symbolValueArray;
        std::array<uint8_t, sMaxCodeBitsLength>    _codeBitsArray;
        
        std::array<bool, (1 << sNbBitsHash)>       _symbolValueHashExistenceArray;
        std::array<SymbolType, (1 << sNbBitsHash)> _symbolValueHashArray;
        std::array<uint8_t, (1 << sNbBitsHash)>    _codeBitsHashArray;
};

template <typename SymbolType, 
          uint8_t sMaxCodeBitsLength,
          uint16_t sMaxSymbolValue>
class HuffmanTreeBuilder
{
    public:
        void clear();
        
        void addSymbol(SymbolType iSymbol, uint8_t iNbBits);
        
        template <uint8_t sNbBitsHash>
        bool buildHuffmanTree(HuffmanTree<SymbolType, sNbBitsHash, sMaxCodeBitsLength, sMaxSymbolValue>& oHuffmanTree);
        
    private:
        bool empty() const;
        
        std::array<bool, sMaxCodeBitsLength>       _symbolListByBitsHeadExistenceArray;
        std::array<SymbolType, sMaxCodeBitsLength> _symbolListByBitsHeadArray;
        
        std::array<bool, sMaxSymbolValue>          _symbolListByBitsBodyExistenceArray;
        std::array<SymbolType, sMaxSymbolValue>    _symbolListByBitsBodyArray;
};

}
}

#include "HuffmanTree.i"

#endif // GW2DATTOOLS_COMPRESSION_HUFFMANTREE_H
