#ifndef GW2DATTOOLS_UTILS_BITARRAY_I
#define GW2DATTOOLS_UTILS_BITARRAY_I

#include "gw2DatTools/exception/Exception.h"

#include <cassert>

namespace gw2dt
{
namespace utils
{

template <typename IntType>
BitArray<IntType>::BitArray(const uint8_t* ipBuffer, uint32_t iSize, uint32_t iSkippedBytes) :
    _pBufferStartPos(ipBuffer),
    _pBufferPos(ipBuffer),
    _bytesAvail(iSize),
    _skippedBytes(iSkippedBytes),
    _head(0),
    _buffer(0),
    _bitsAvail(0)
{
    assert(iSize % sizeof(IntType) == 0);
    
    pull(_head, _bitsAvail);
}

template <typename IntType>
void BitArray<IntType>::pull(IntType& oValue, uint8_t& oNbPulledBits)
{
    if (_bytesAvail >= sizeof(IntType))
    {
        if (_skippedBytes != 0)
        {
            if ((((_pBufferPos - _pBufferStartPos)/sizeof(IntType)) + 1) % _skippedBytes == 0)
            {
                _bytesAvail -= sizeof(IntType);
                _pBufferPos += sizeof(IntType);
            }
        }
        oValue = *(reinterpret_cast<const IntType*>(_pBufferPos));
        _bytesAvail -= sizeof(IntType);
        _pBufferPos += sizeof(IntType);
        oNbPulledBits = sizeof(IntType) * 8;
    }
    else
    {
        oValue = 0;
        oNbPulledBits = 0;
    }
}

template <typename IntType>
template <typename OutputType>
void BitArray<IntType>::readImpl(uint8_t iBitNumber, OutputType& oValue) const
{
    oValue = (_head >> ((sizeof(IntType) * 8) - iBitNumber));
}

template <typename IntType>
template <typename OutputType>
void BitArray<IntType>::readLazy(uint8_t iBitNumber, OutputType& oValue) const
{
    if (iBitNumber > sizeof(OutputType) * 8)
    {
        throw exception::Exception("Invalid number of bits requested.");
    }
    if (iBitNumber > sizeof(IntType) * 8)
    {
        throw exception::Exception("Invalid number of bits requested.");
    }
    
    readImpl(iBitNumber, oValue);
}

template <typename IntType>
template <uint8_t isBitNumber, typename OutputType>
void BitArray<IntType>::readLazy(OutputType& oValue) const
{
    static_assert(isBitNumber <= sizeof(OutputType) * 8, "isBitNumber must be inferior to the size of the requested type.");
    static_assert(isBitNumber <= sizeof(IntType) * 8, "isBitNumber must be inferior to the size of the internal type.");
    
    readImpl(isBitNumber, oValue);
}

template <typename IntType>
template <typename OutputType>
void BitArray<IntType>::readLazy(OutputType& oValue) const
{
    readLazy<sizeof(OutputType) * 8>(oValue);
}

template <typename IntType>
template <typename OutputType>
void BitArray<IntType>::read(uint8_t iBitNumber, OutputType& oValue) const
{
    if (_bitsAvail < iBitNumber)
    {
        throw exception::Exception("Not enough bits available to read the value.");
    }
    readLazy(iBitNumber, oValue);
}

template <typename IntType>
template <uint8_t isBitNumber, typename OutputType>
void BitArray<IntType>::read(OutputType& oValue) const
{
    if (_bitsAvail < isBitNumber)
    {
        throw exception::Exception("Not enough bits available to read the value.");
    }
    readLazy<isBitNumber>(oValue);
}

template <typename IntType>
template <typename OutputType>
void BitArray<IntType>::read(OutputType& oValue) const
{
    read<sizeof(OutputType) * 8>(oValue);
}

template <typename IntType>
void BitArray<IntType>::dropImpl(uint8_t iBitNumber)
{
    if (_bitsAvail < iBitNumber)
    {
        throw exception::Exception("Too much bits were asked to be dropped.");
    }
    
    uint8_t aNewBitsAvail = _bitsAvail - iBitNumber;
    if (aNewBitsAvail >= sizeof(IntType) * 8)
    {
        if (iBitNumber == sizeof(IntType) * 8)
        {
            _head = _buffer;
            _buffer = 0;
        }
        else
        {
            _head = (_head << iBitNumber) | (_buffer >> ((sizeof(IntType) * 8) - iBitNumber));
            _buffer = _buffer << iBitNumber;
        }
        _bitsAvail = aNewBitsAvail;
    }
    else
    {
        IntType aNewValue;
        uint8_t aNbPulledBits;
        pull(aNewValue, aNbPulledBits);
        
        if (iBitNumber == sizeof(IntType) * 8)
        {
            _head = 0;
        }
        else
        {
            _head = _head << iBitNumber;
        }
        _head |= (_buffer >> ((sizeof(IntType) * 8) - iBitNumber)) | (aNewValue >> (aNewBitsAvail));
        if (aNewBitsAvail > 0)
        {
            _buffer = aNewValue << ((sizeof(IntType) * 8) - aNewBitsAvail);
        }
        _bitsAvail = aNewBitsAvail + aNbPulledBits;
    }
}

template <typename IntType>
void BitArray<IntType>::drop(uint8_t iBitNumber)
{
    if (iBitNumber > sizeof(IntType) * 8)
    {
        throw exception::Exception("Invalid number of bits to be dropped.");
    }
    dropImpl(iBitNumber);
}

template <typename IntType>
template <uint8_t isBitNumber>
void BitArray<IntType>::drop()
{
    static_assert(isBitNumber <= sizeof(IntType) * 8, "isBitNumber must be inferior to the size of the internal type.");
    dropImpl(isBitNumber);
}

template <typename IntType>
template <typename OutputType>
void BitArray<IntType>::drop()
{
    drop<sizeof(OutputType) * 8>();
}


}
}

#endif // GW2DATTOOLS_UTILS_BITARRAY_I