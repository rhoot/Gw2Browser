#ifndef GW2DATTOOLS_FORMATS_MFT_H
#define GW2DATTOOLS_FORMATS_MFT_H

#include <cstdint>
#include <vector>

#include <memory>

namespace gw2dt
{
namespace format
{

#pragma pack(push, 1)
struct MftHeader
{
    uint8_t  magic[4];
    uint64_t unknown1;
    uint32_t nbOfEntries;
	uint32_t unknown2;
	uint32_t unknown3;
};

struct MftEntry
{
    uint64_t offset; 
    uint32_t size;
    uint16_t compressionFlag;
    uint16_t unknown1;
    uint32_t unknown2;
    uint32_t crc;
};
#pragma pack(pop)

struct Mft
{
   MftHeader header;
   std::vector<MftEntry> entries;
};

std::unique_ptr<Mft> parseMft(std::istream& iStream, const uint64_t& iOffset, const uint32_t iSize);

}
}

#endif // GW2DATTOOLS_FORMATS_MFT_H
