#include <iostream>
#include <cstdint>

#include <sstream>
#include <fstream>

#include "gw2DatTools/interface/ANDatInterface.h"
#include "gw2DatTools/compression/inflateDatFileBuffer.h"
#include "gw2DatTools/compression/inflateTextureFileBuffer.h"

int main(int argc, char* argv[])
{
	std::cout << "Start" << std::endl;
    const uint32_t aBufferSize = 1024 * 1024 * 30; // We make the assumption that no file is bigger than 30 Mo
    
    auto pANDatInterface = gw2dt::interface::createANDatInterface("Y:\\Games\\Guild Wars 2\\Gw2.dat");

	std::cout << "Getting FileRecord Id" << std::endl;
    auto aFileRecord = pANDatInterface->getFileRecordForFileId(184887);
    
    uint8_t* pOriBuffer = new uint8_t[aBufferSize];
    uint8_t* pInfBuffer = new uint8_t[aBufferSize];
	uint8_t* pOutBuffer = new uint8_t[aBufferSize];

	uint8_t* pAtexBuffer = nullptr;
	uint32_t aAtexBufferSize = 0;
    
	uint32_t aOriSize = aBufferSize;
	pANDatInterface->getBuffer(aFileRecord, aOriSize, pOriBuffer);
	
	std::ostringstream aStringstream;
	aStringstream << "Y:\\";
	aStringstream << aFileRecord.fileId;
	
	std::ofstream aStream(aStringstream.str(), std::ios::binary);
	
	if (aOriSize == aBufferSize)
	{
		std::cout << "File " << aFileRecord.fileId << " has a size greater than (or equal to) 30Mo." << std::endl;
	}
	
	if (aFileRecord.isCompressed)
	{
		uint32_t aInfSize = aBufferSize;
		std::cout << "Compressed." << std::endl;
		
		try
		{
			gw2dt::compression::inflateDatFileBuffer(aOriSize, pOriBuffer, aInfSize, pInfBuffer);

			pAtexBuffer = pInfBuffer;
			aAtexBufferSize = aInfSize;
		}
		catch(std::exception& iException)
		{
			std::cout << "File " << aFileRecord.fileId << " failed to decompress: " << std::string(iException.what()) << std::endl;
		}
	}
	else
	{
		std::cout << "Not compressed." << std::endl;
		pAtexBuffer = pOriBuffer;
		aAtexBufferSize = aOriSize;
	}

	try
	{
		std::cout << "aAtexBufferSize: " << aAtexBufferSize << std::endl;
		uint32_t aOutSize = aBufferSize;
		gw2dt::compression::inflateTextureFileBuffer(aAtexBufferSize, pAtexBuffer, aOutSize, pOutBuffer);

		aStream.write(reinterpret_cast<char*>(pOutBuffer), aOutSize);
	}
	catch(std::exception& iException)
	{
		std::cout << "Atex File " << aFileRecord.fileId << " failed to decompress: " << std::string(iException.what()) << std::endl;
	}

	aStream.close();

	int i; std::cin >> i;

    delete[] pOriBuffer;
    delete[] pInfBuffer;

    return 0;
};