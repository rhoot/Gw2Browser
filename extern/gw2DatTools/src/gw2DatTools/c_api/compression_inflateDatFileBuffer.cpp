#include "gw2DatTools/c_api/compression_inflateDatFileBuffer.h"

#include <exception>

#include "gw2DatTools/compression/inflateDatFileBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif 

GW2DATTOOLS_API uint8_t* GW2DATTOOLS_APIENTRY compression_inflateDatFileBuffer(const uint32_t iInputSize, uint8_t* iInputTab, uint32_t* ioOutputSize, uint8_t* ioOutputTab)
{
    if (ioOutputSize == nullptr)
    {
        printf("GW2DATTOOLS_C_API(compression_inflateDatFileBuffer): ioOutputSize is NULL.");
        return NULL;
    }
    
    try
    {
        return gw2dt::compression::inflateDatFileBuffer(iInputSize, iInputTab, *ioOutputSize, ioOutputTab == NULL ? nullptr : ioOutputTab);
    }
    catch(std::exception& iException)
    {
        printf("GW2DATTOOLS_C_API(compression_inflateDatFileBuffer): %s", iException.what());
        return NULL;
    }
}

#ifdef __cplusplus
}
#endif 
