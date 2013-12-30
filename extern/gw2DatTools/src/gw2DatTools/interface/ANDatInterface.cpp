#include "gw2DatTools/interface/ANDatInterface.h"

#include <fstream>
#include <unordered_map>

#include "gw2DatTools/exception/Exception.h"

#include "../format/ANDat.h"
#include "../format/Mft.h"
#include "../format/Mapping.h"
#include "../format/Utils.h"

namespace gw2dt
{
namespace interface
{

class ANDatInterfaceImpl : public ANDatInterface
{
    public:
        ANDatInterfaceImpl(const char* iDatPath, std::unique_ptr<format::Mft>& ipMft, std::unique_ptr<format::Mapping>& ipMapping);
        virtual ~ANDatInterfaceImpl();
        
        virtual void getBuffer(const ANDatInterface::FileRecord& iFileRecord, uint32_t& ioOutputSize, uint8_t* ioBuffer);
        
        virtual const FileRecord& getFileRecordForFileId(const uint32_t& iFileId) const;
        virtual const FileRecord& getFileRecordForBaseId(const uint32_t& iBaseId) const;
        
        virtual const std::vector<FileRecord>& getFileRecordVect() const;
        
        void computeInternalData();
        
    private:
        std::ifstream _datStream;
        
        // Helper data structures
        std::unordered_map<uint32_t, FileRecord*> _fileIdDict;
        std::unordered_map<uint32_t, FileRecord*> _baseIdDict;
        
        // Computed data structures
        std::vector<FileRecord> _fileRecordVect;
        
        // Raw data structures
        std::unique_ptr<format::Mft> _pMft;
        std::unique_ptr<format::Mapping> _pMapping;
};

ANDatInterfaceImpl::ANDatInterfaceImpl(const char* iDatPath, std::unique_ptr<format::Mft>& ipMft, std::unique_ptr<format::Mapping>& ipMapping) :
    _datStream(iDatPath, std::ios::binary),
    _pMft(std::move(ipMft)),
    _pMapping(std::move(ipMapping))
{
}

ANDatInterfaceImpl::~ANDatInterfaceImpl()
{
}

void ANDatInterfaceImpl::getBuffer(const ANDatInterface::FileRecord& iFileRecord, uint32_t& ioOutputSize, uint8_t* ioBuffer)
{
    _datStream.seekg(iFileRecord.offset);
    ioOutputSize = std::min(ioOutputSize, iFileRecord.size);
    format::readStructs(_datStream, *ioBuffer, ioOutputSize);
}

const ANDatInterface::FileRecord& ANDatInterfaceImpl::getFileRecordForFileId(const uint32_t& iFileId) const
{
    auto it = _fileIdDict.find(iFileId);
    if (it != _fileIdDict.end())
    {
        if (it->second != nullptr)
        {
            return *(it->second);
        }
        else
        {
            throw exception::Exception("FileId found, but null entry.");
        }
    }
    else
    {
        throw exception::Exception("FileId not found.");
    }
}

const ANDatInterface::FileRecord& ANDatInterfaceImpl::getFileRecordForBaseId(const uint32_t& iBaseId) const
{
    auto it = _baseIdDict.find(iBaseId);
    if (it != _baseIdDict.end())
    {
        if (it->second != nullptr)
        {
            return *(it->second);
        }
        else
        {
            throw exception::Exception("BaseId found, but null entry.");
        }
    }
    else
    {
        throw exception::Exception("BaseId not found.");
    }
}

const std::vector<ANDatInterface::FileRecord>& ANDatInterfaceImpl::getFileRecordVect() const
{
    return _fileRecordVect;
}

void ANDatInterfaceImpl::computeInternalData()
{
    _fileIdDict.clear();
    _baseIdDict.clear();
    _fileRecordVect.clear();
    
    _fileRecordVect.resize(_pMapping->entries.size());
    
    std::unordered_map<uint32_t, FileRecord*> aMftIndexDictHelper;
    aMftIndexDictHelper.rehash(_pMapping->entries.size());
    
    uint32_t aCurrentIndex(0);
    
    for (auto itMapping = _pMapping->entries.begin(); itMapping != _pMapping->entries.end(); ++itMapping)
    {
        if (itMapping->mftIndex == 0 && itMapping->id == 0)
        {
            continue;
        }
        else
        {
            auto itMftDict = aMftIndexDictHelper.find(itMapping->mftIndex);
            if (itMftDict != aMftIndexDictHelper.end())
            {
                FileRecord* pFileRecord = itMftDict->second;
                
                if (itMapping->id < pFileRecord->fileId)
                {
                    pFileRecord->baseId = itMapping->id;
                }
                else if (itMapping->id > pFileRecord->fileId)
                {
                    pFileRecord->baseId = pFileRecord->fileId;
                    pFileRecord->fileId = itMapping->id;
                }
            }
            else
            {
                FileRecord& aFileRecord = _fileRecordVect[aCurrentIndex];
                ++aCurrentIndex;
                format::MftEntry& aMftEntry = _pMft->entries[itMapping->mftIndex - 1];
                
                aFileRecord.offset = aMftEntry.offset;
                aFileRecord.size = aMftEntry.size;
                
                aFileRecord.baseId = 0;
                aFileRecord.fileId = itMapping->id;
                
                aFileRecord.isCompressed = (aMftEntry.compressionFlag != 0);
                
                aMftIndexDictHelper.insert(std::make_pair(itMapping->mftIndex, &aFileRecord));
            }
        }
    }
    
    // Dropping the unecessary entries
    _fileRecordVect.resize(aCurrentIndex);
    
    // Reserving space for dicts
    _fileIdDict.rehash(_fileRecordVect.size());
    _baseIdDict.rehash(_fileRecordVect.size());
    
    for (auto itFileRecord = _fileRecordVect.begin(); itFileRecord != _fileRecordVect.end(); ++itFileRecord)
    {
        _fileIdDict.insert(std::make_pair(itFileRecord->fileId, &(*itFileRecord)));
        
        if (itFileRecord->baseId != 0)
        {
            _baseIdDict.insert(std::make_pair(itFileRecord->baseId, &(*itFileRecord)));
        }
    }
}

GW2DATTOOLS_API std::unique_ptr<ANDatInterface> GW2DATTOOLS_APIENTRY createANDatInterface(const char* iDatPath)
{
    std::ifstream aDatStream(iDatPath, std::ios::binary);
    auto pANDat = format::parseANDat(aDatStream, 0, 0);
    
    auto pMft = format::parseMft(aDatStream, pANDat->header.mftOffset, pANDat->header.mftSize);
    auto pMapping = format::parseMapping(aDatStream, pMft->entries[1].offset, pMft->entries[1].size);
    
    auto pANDatInterfaceImpl = std::unique_ptr<ANDatInterfaceImpl>(new ANDatInterfaceImpl(iDatPath, pMft, pMapping));
    pANDatInterfaceImpl->computeInternalData();
    
    return std::move(pANDatInterfaceImpl);
}

}
}
