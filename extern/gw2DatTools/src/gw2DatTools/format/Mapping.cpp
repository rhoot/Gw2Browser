#include "Mapping.h"

#include <istream>

#include "Utils.h"

#include <iostream>

namespace gw2dt
{
namespace format
{

std::unique_ptr<Mapping> parseMapping(std::istream& iStream, const uint64_t& iOffset, const uint32_t iSize)
{
    iStream.seekg(iOffset, std::ios::beg);
    
    std::unique_ptr<Mapping> pMapping(new Mapping());
    
    uint32_t aNbOfEntries = iSize / sizeof(MappingEntry);
    
    pMapping->entries.resize(aNbOfEntries);
    readStructVect(iStream, pMapping->entries);
    
    return std::move(pMapping);
}

}
}
