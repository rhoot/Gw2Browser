#include "ANDat.h"

#include <istream>

#include "Utils.h"

namespace gw2dt
{
namespace format
{

std::unique_ptr<ANDat> parseANDat(std::istream& iStream, const uint64_t& iOffset, const uint32_t iSize)
{
    iStream.seekg(iOffset, std::ios::beg);
    
    std::unique_ptr<ANDat> pANDat(new ANDat());
    readStructs(iStream, pANDat->header);

    return std::move(pANDat);
}

}
}
