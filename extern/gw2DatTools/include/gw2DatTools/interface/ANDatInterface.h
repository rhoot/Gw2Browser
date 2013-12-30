#ifndef GW2DATTOOLS_INTERFACE_ANDATINTERFACE_H
#define GW2DATTOOLS_INTERFACE_ANDATINTERFACE_H

#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>

#include "gw2DatTools/dllMacros.h"

namespace gw2dt
{
namespace interface
{

class GW2DATTOOLS_API ANDatInterface
{
    public:
        struct FileRecord
        {
            uint64_t offset;
            uint32_t size;
            
            uint32_t baseId;
            uint32_t fileId;
            
            bool isCompressed;
        };
        
        virtual ~ANDatInterface() {};
        
        virtual void getBuffer(const ANDatInterface::FileRecord& iFileRecord, uint32_t& ioOutputSize, uint8_t* ioBuffer) = 0;
        
        virtual const FileRecord& getFileRecordForFileId(const uint32_t& iFileId) const = 0;
        virtual const FileRecord& getFileRecordForBaseId(const uint32_t& iBaseId) const = 0;
        
        virtual const std::vector<FileRecord>& getFileRecordVect() const = 0;
};

GW2DATTOOLS_API std::unique_ptr<ANDatInterface> GW2DATTOOLS_APIENTRY createANDatInterface(const char* iDatPath);

}
}

#endif // GW2DATTOOLS_INTERFACE_ANDATINTERFACE_H
