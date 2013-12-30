#ifndef GW2DATTOOLS_CAPI_COMPRESSION_INFLATEBUFFER_H
#define GW2DATTOOLS_CAPI_COMPRESSION_INFLATEBUFFER_H

#include <stdint.h>
#include <stdlib.h>

#include "gw2DatTools/dllMacros.h"

#ifdef __cplusplus
extern "C" {
#endif 

/** @Inputs:
 *    - iInputSize: Size of the input buffer
 *    - iInputTab: Pointer to the buffer to inflate
 *    - ioOutputSize: if the value is 0 then we decode everything
 *                    else we decode until we reach the io_outputSize
 *    - ioOutputTab: Optional output buffer, in case you provide this buffer,
 *                   ioOutputSize shall be inferior or equal to the size of this buffer
 *  @Outputs:
 *    - ioOutputSize: actual size of the outputBuffer
 *  @Return:
 *    - Pointer to the outputBuffer, NULL if it failed
 */
GW2DATTOOLS_API uint8_t* GW2DATTOOLS_APIENTRY compression_inflateBuffer(const uint32_t iInputSize, uint8_t* iInputTab, uint32_t* ioOutputSize, uint8_t* ioOutputTab = NULL);

#ifdef __cplusplus
}
#endif 

#endif // GW2DATTOOLS_CAPI_COMPRESSION_INFLATEBUFFER_H
