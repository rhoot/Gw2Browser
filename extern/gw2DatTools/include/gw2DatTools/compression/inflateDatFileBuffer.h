#ifndef GW2DATTOOLS_COMPRESSION_INFLATEDATFILEBUFFER_H
#define GW2DATTOOLS_COMPRESSION_INFLATEDATFILEBUFFER_H

#include <cstdint>
#include <string>

#include "gw2DatTools/dllMacros.h"

namespace gw2dt
{
namespace compression
{

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
 *    - Pointer to the outputBuffer, nullptr if it failed
 *  @Throws:
 *    - gw2dt::exception::Exception or std::exception in case of error
 */

GW2DATTOOLS_API uint8_t* GW2DATTOOLS_APIENTRY inflateDatFileBuffer(uint32_t iInputSize, const uint8_t* iInputTab,  uint32_t& ioOutputSize, uint8_t* ioOutputTab = nullptr);

}
}

#endif // GW2DATTOOLS_COMPRESSION_INFLATEDATFILEBUFFER_H
