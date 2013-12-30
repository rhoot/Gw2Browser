#ifndef GW2DATTOOLS_FORMATS_MAPPING_H
#define GW2DATTOOLS_FORMATS_MAPPING_H

#include <cstdint>
#include <vector>

#include <memory>

namespace gw2dt
{
namespace format
{

#pragma pack(push, 1)
struct MappingEntry
{
    uint32_t id;
    uint32_t mftIndex;
};
#pragma pack(pop)

struct Mapping
{
   std::vector<MappingEntry> entries;
};

std::unique_ptr<Mapping> parseMapping(std::istream& iStream, const uint64_t& iOffset, const uint32_t iSize);

}
}

#endif // GW2DATTOOLS_FORMATS_MAPPING_H
