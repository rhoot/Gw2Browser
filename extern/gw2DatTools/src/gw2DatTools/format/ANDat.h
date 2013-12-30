#ifndef GW2DATTOOLS_FORMATS_ANDAT_H
#define GW2DATTOOLS_FORMATS_ANDAT_H

#include <memory>
#include <cstdint>

namespace gw2dt
{
namespace format
{

#pragma pack(push, 1)
struct ANDatHeader
{
    uint8_t  version;
    uint8_t  magic[3];
    uint32_t headerSize;
    uint32_t unknown1;
    uint32_t chunkSize;
    uint32_t crc;
    uint32_t unknown2;
    uint64_t mftOffset;
    uint32_t mftSize;
    uint32_t flags;
};
#pragma pack(pop)

struct ANDat
{
    ANDatHeader header;
};

std::unique_ptr<ANDat> parseANDat(std::istream& iStream, const uint64_t& iOffset, const uint32_t iSize);

}
}

#endif // GW2DATTOOLS_FORMATS_ANDAT_H
