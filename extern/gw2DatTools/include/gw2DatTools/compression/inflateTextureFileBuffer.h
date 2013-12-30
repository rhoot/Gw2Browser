#ifndef GW2DATTOOLS_COMPRESSION_INFLATETEXTUREFILEBUFFER_H
#define GW2DATTOOLS_COMPRESSION_INFLATETEXTUREFILEBUFFER_H

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

GW2DATTOOLS_API uint8_t* GW2DATTOOLS_APIENTRY inflateTextureFileBuffer(uint32_t iInputSize, const uint8_t* iInputTab,  uint32_t& ioOutputSize, uint8_t* ioOutputTab = nullptr);

/** @Inputs:
 *    - iWidth: Width of the texture
 *    - iHeight: Height of the texture
 *    - iFormatFourCc: FourCC describing the format of the data
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

GW2DATTOOLS_API uint8_t* GW2DATTOOLS_APIENTRY inflateTextureBlockBuffer(uint16_t iWidth, uint16_t iHeight, uint32_t iFormatFourCc, uint32_t iInputSize, const uint8_t* iInputTab, 
    uint32_t& ioOutputSize, uint8_t* ioOutputTab = nullptr);
}
}

#endif // GW2DATTOOLS_COMPRESSION_INFLATETEXTUREFILEBUFFER_H
