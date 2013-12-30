#ifndef GW2DATTOOLS_UTILS_BITARRAY_H
#define GW2DATTOOLS_UTILS_BITARRAY_H

#include <cstdint>

namespace gw2dt
{
namespace utils
{

template <typename IntType>
class BitArray
{
    public:
        BitArray(const uint8_t* ipBuffer, uint32_t iSize, uint32_t iSkippedBytes = 0);
        
        template <typename OutputType>
        void readLazy(uint8_t iBitNumber, OutputType& oValue) const;
        template <uint8_t isBitNumber, typename OutputType>
        void readLazy(OutputType& oValue) const;
        template <typename OutputType>
        void readLazy(OutputType& oValue) const;
        
        template <typename OutputType>
        void read(uint8_t iBitNumber, OutputType& oValue) const;
        template <uint8_t isBitNumber, typename OutputType>
        void read(OutputType& oValue) const;
        template <typename OutputType>
        void read(OutputType& oValue) const;
        
        void drop(uint8_t iBitNumber);
        template <uint8_t isBitNumber>
        void drop();
        template <typename OutputType>
        void drop();
        
    private:
        template <typename OutputType>
        void readImpl(uint8_t iBitNumber, OutputType& oValue) const;
        void dropImpl(uint8_t iBitNumber);
        
        void pull(IntType& oValue, uint8_t& oNbPulledBits);
        
        const uint8_t* const _pBufferStartPos;
        const uint8_t* _pBufferPos;
        uint32_t _bytesAvail;

        uint32_t _skippedBytes;
        
        IntType _head;
        IntType _buffer;
        uint8_t _bitsAvail;
};

}
}

#include "BitArray.i"

#endif // GW2DATTOOLS_UTILS_BITARRAY_H
