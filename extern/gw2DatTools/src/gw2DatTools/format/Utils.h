#ifndef GW2DATTOOLS_FORMATS_UTILS_H
#define GW2DATTOOLS_FORMATS_UTILS_H

#include <vector>

namespace gw2dt
{
namespace format
{

template <typename Struct>
void readStructs(std::istream& iStream, Struct& iStruct, const uint32_t iNum = 1)
{
    iStream.read(reinterpret_cast<char*>(&iStruct), sizeof(Struct) * iNum);
}

template <typename Struct>
void readStructVect(std::istream& iStream, std::vector<Struct>& iStructVect)
{
    iStream.read(reinterpret_cast<char*>(iStructVect.data()), sizeof(Struct) * iStructVect.size());
}


}
}

#endif // GW2DATTOOLS_FORMATS_UTILS_H
