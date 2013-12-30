#include "Mft.h"

#include <istream>

#include "Utils.h"

namespace gw2dt
{
namespace format
{

std::unique_ptr<Mft> parseMft(std::istream& iStream, const uint64_t& iOffset, const uint32_t iSize)
{
    iStream.seekg(iOffset, std::ios::beg);
    
    std::unique_ptr<Mft> pMft(new Mft());
    readStructs(iStream, pMft->header);
    
    pMft->entries.resize(pMft->header.nbOfEntries - 1);
    readStructVect(iStream, pMft->entries);
    
    return std::move(pMft);
}

}
}
