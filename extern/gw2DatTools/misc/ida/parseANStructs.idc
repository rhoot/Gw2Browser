#include <idc.idc>

#define IS_ASCII(a) (((Byte(a) >= 48) && (Byte(a) <= 57)) || ((Byte(a) >= 65) && (Byte(a) <= 90)) || ((Byte(a) >= 97) && (Byte(a) <= 122)))

static getAsciiName(iAddress)
{
    auto aResult, aByte;
    aResult = "";
    while (aByte = Byte(iAddress))
    {
        aResult = form("%s%c", aResult, aByte);
        iAddress = iAddress + 1;
    }
    return aResult;
}

static add(iArrayId, iElement)
{
    auto aArrayId, aNextIndex;
 
    aNextIndex = GetLastIndex(AR_LONG, iArrayId) + 1;
    SetArrayLong(iArrayId, aNextIndex, iElement);
}

static isIn(iArrayId, iElement)
{
    auto aCurrentIndex;
    aCurrentIndex = GetFirstIndex(AR_LONG, iArrayId);
 
    while (aCurrentIndex != -1)
    {
        if (GetArrayElement(AR_LONG, iArrayId, aCurrentIndex) == iElement)
        {
            return 1;
        }
 
        aCurrentIndex = GetNextIndex(AR_LONG, iArrayId, aCurrentIndex);
    }
    return 0;
}

static isANStruct(iAddress)
{
    auto aCurrentAddress, aLoopGuard;
    
    aLoopGuard = 50;
    aCurrentAddress = iAddress;
    
    while (Word(aCurrentAddress) != 0 && aLoopGuard > 0)
    {
        if (Word(aCurrentAddress) > 0x1D)
        {
            return 0;
        }
        
        aCurrentAddress = aCurrentAddress +  16;
        aLoopGuard = aLoopGuard - 1;
    }
    
    return (aLoopGuard != 0 && Dword(aCurrentAddress + 4) != BADADDR && IS_ASCII(Dword(aCurrentAddress + 4)));
}

static isANStructTab(iAddress, iNumber)
{
    auto aCurrentAddress, aLoopIndex;
    
    aLoopIndex = 0;
    aCurrentAddress = iAddress;
    
    while (aLoopIndex < iNumber)
    {
        if (Dword(aCurrentAddress) !=0)
        {
            if (!isANStruct(Dword(aCurrentAddress)))
            {
                break;
            }
        }
        aCurrentAddress = aCurrentAddress + 12;
        aLoopIndex = aLoopIndex + 1;
    }
    
    return (aLoopIndex == iNumber);
}

static getSimpleTypeName(iAddress)
{
    auto aTypeId;
    aTypeId = Word(iAddress);
    if (aTypeId == 0x05)
        return "byte";
    else if (aTypeId == 0x06)
        return "byte4";
    else if (aTypeId == 0x07)
        return "double";
    else if (aTypeId == 0x0A)
        return "dword";
    else if (aTypeId == 0x0B)
        return "filename";
    else if (aTypeId == 0x0C)
        return "float";
    else if (aTypeId == 0x0D)
        return "float2";
    else if (aTypeId == 0x0E)
        return  "float3";
    else if (aTypeId == 0x0F)
        return "float4";
    else if (aTypeId == 0x11)
        return "qword";
    else if (aTypeId == 0x12)
        return "wchar_ptr";
    else if (aTypeId == 0x13)
        return "char_ptr";
    else if (aTypeId == 0x15)
        return "word";
    else if (aTypeId == 0x16)
        return "byte16";
    else if (aTypeId == 0x17)
        return "byte3";
    else if (aTypeId == 0x18)
        return "dword2";
    else if (aTypeId == 0x19)
        return "dword4";
    else if (aTypeId == 0x1A)
        return "word3";
    else if (aTypeId == 0x1B)
        return "fileref";
    else
        return "ERROR";
}

static parseMember(iAddress, iParsedStructsId, iOutputFile)
{
    auto aTypeId, aMemberName, aOptimized, aTempOutput;
    aMemberName = getAsciiName(Dword(iAddress + 4));
    aTypeId = Word(iAddress);
    
    if (aTypeId == 0x00)
    {
        aTempOutput = form("ERROR %s", aMemberName);
        Message("ERROR: Encountered 0x00 as a member typeId.");
        aOptimized = 1;
    }
    else if (aTypeId == 0x01)
    {
        aTempOutput = form("%s %s[%d]", parseStruct(Dword(iAddress + 8), iParsedStructsId, iOutputFile), aMemberName, Dword(iAddress + 12));
        aOptimized = 1;
    }
    else if (aTypeId == 0x02)
    {
        aTempOutput = form("TSTRUCT_ARRAY_PTR_START %s %s TSTRUCT_ARRAY_PTR_END", parseStruct(Dword(iAddress + 8), iParsedStructsId, iOutputFile), aMemberName);
        aOptimized = 0;
    }
    else if (aTypeId == 0x03)
    {
        aTempOutput = form("TSTRUCT_PTR_ARRAY_PTR_START %s %s TSTRUCT_PTR_ARRAY_PTR_END", parseStruct(Dword(iAddress + 8), iParsedStructsId, iOutputFile), aMemberName);
        aOptimized = 0;
    }
    else if (aTypeId == 0x04)
    {
        aTempOutput = form("%s %s", "Unknown0x04", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x05)
    {
        aTempOutput = form("%s %s", "byte", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x06)
    {
        aTempOutput = form("%s %s", "byte4", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x07)
    {
        aTempOutput = form("%s %s", "double", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x08)
    {
        aTempOutput = form("%s %s", "Unknown0x08", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x09)
    {
        aTempOutput = form("%s %s", "Unknown0x09", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x0A)
    {
        aTempOutput = form("%s %s", "dword", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x0B)
    {
        aTempOutput = form("%s %s", "filename", aMemberName);
        aOptimized = 0;
    }
    else if (aTypeId == 0x0C)
    {
        aTempOutput = form("%s %s", "float", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x0D)
    {
        aTempOutput = form("%s %s", "float2", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x0E)
    {
        aTempOutput = form("%s %s", "float3", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x0F)
    {
        aTempOutput = form("%s %s", "float4", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x10)
    {
        aTempOutput = form("TPTR_START %s %s TPTR_END", parseStruct(Dword(iAddress + 8), iParsedStructsId, iOutputFile), aMemberName);
        aOptimized = 0;
    }
    else if (aTypeId == 0x11)
    {
        aTempOutput = form("%s %s", "qword", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x12)
    {
        aTempOutput = form("%s %s", "wchar_ptr", aMemberName);
        aOptimized = 0;
    }
    else if (aTypeId == 0x13)
    {
        aTempOutput = form("%s %s", "char_ptr", aMemberName);
        aOptimized = 0;
    }
    else if (aTypeId == 0x14)
    {
        aTempOutput = form("%s %s", parseStruct(Dword(iAddress + 8), iParsedStructsId, iOutputFile), aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x15)
    {
        aTempOutput = form("%s %s", "word", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x16)
    {
        aTempOutput = form("%s %s", "byte16", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x17)
    {
        aTempOutput = form("%s %s", "byte3", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x18)
    {
        aTempOutput = form("%s %s", "dword2", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x19)
    {
        aTempOutput = form("%s %s", "dword4", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x1A)
    {
        aTempOutput = form("%s %s", "word3", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x1B)
    {
        aTempOutput = form("%s %s", "fileref", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x1C)
    {
        aTempOutput = form("%s %s", "Unknown0x1C", aMemberName);
        aOptimized = 1;
    }
    else if (aTypeId == 0x1D)
    {
        aTempOutput = form("%s %s", parseStruct(Dword(iAddress + 8), iParsedStructsId, iOutputFile), aMemberName);
        aOptimized = 1;
    }
    else
    {
        aTempOutput = form("ERROR %s\n", aMemberName);
        Message("ERROR: Encountered > 0x1D as a member typeId.");
        aOptimized = 1;
    }
    
    aTempOutput = form("    %s;\n", aTempOutput);
    
    return aTempOutput;
}

static parseStruct(iAddress, iParsedStructsId, iOutputFile)
{
    auto aOutput, aStructName, aCurrentAddress, aOptimized, aAlreadyParsed, aMemberOutput;
    
    aOptimized = 0;
    aAlreadyParsed = isIn(iParsedStructsId, iAddress);
    add(iParsedStructsId, iAddress);
    
    aCurrentAddress = iAddress; 
    
    // Special case for simple types
    if (Byte(Dword(aCurrentAddress + 4)) == 0)
    {
        return getSimpleTypeName(iAddress);
    }
    
    while (Word(aCurrentAddress) != 0)
    {
        if (!aAlreadyParsed)
        {
            aMemberOutput = parseMember(aCurrentAddress, iParsedStructsId, iOutputFile);
            aOutput = form("%s%s", aOutput, aMemberOutput);
        }
        
        aCurrentAddress = aCurrentAddress +  16;
    }
    
    aStructName = getAsciiName(Dword(aCurrentAddress + 4));
    
    if (!aAlreadyParsed)
    {
        if (aOptimized)
        {
            aOutput = form("typedef struct \n{\n%s} %s;\n\n", aOutput, aStructName);
        }
        else
        {
            aOutput = form("typedef struct \n{\n%s} %s<optimize=false>;\n\n", aOutput, aStructName);
        }
        
        fprintf(iOutputFile, "%s", aOutput);
    }
    
    return aStructName;
}

static parseStructTab(iANSTructTabOffset, iNbOfVersions, iOutputFile)
{
    auto aCurrentAddress, aLoopIndex, aParsedStructsId, aSubAddress;
    
    aLoopIndex = iNbOfVersions - 1;
    aCurrentAddress = iANSTructTabOffset;
    
    while (aLoopIndex >= 0)
    {
        DeleteArray(GetArrayId("PARSED_STRUCTS"));
        aParsedStructsId = CreateArray("PARSED_STRUCTS");
        
        aCurrentAddress = Dword(iANSTructTabOffset + 12 * aLoopIndex);
		aSubAddress = Dword(iANSTructTabOffset + 12 * aLoopIndex + 4);
        if (aCurrentAddress !=0)
        {
			if (aSubAddress != 0)
			{
				fprintf(iOutputFile, "=> Version: %d, ReferencedFunction: 0x%X\n", aLoopIndex, aSubAddress);
			}
			else
			{
				fprintf(iOutputFile, "=> Version: %d\n", aLoopIndex);
			}
            parseStruct(aCurrentAddress, aParsedStructsId, iOutputFile);
        }
        aLoopIndex = aLoopIndex - 1;
    }
}

static main(void)
{
    auto aParsedTablesId;
    
    // First step detecting rdata and text segments
    auto aCurrentSeg, aCurrentAddress, aMiscAddress;
    auto aMinDataSeg, aMaxDataSeg;
    auto aMinRDataSeg, aMaxRDataSeg;
    auto aMinTextSeg, aMaxTextSeg;
    
    auto aChunkName, aNbOfVersions, aANSTructTabOffset;
    
    auto aOutputFile, aReportFile;
    aOutputFile = fopen("output.txt", "w");
    
    Message("ANet structs script started.\n");
    
    aMinDataSeg = 0;
    aMaxDataSeg = 0;
    aMinRDataSeg = 0;
    aMaxRDataSeg = 0;
    aMinTextSeg = 0;
    aMaxTextSeg = 0;
    
    DeleteArray(GetArrayId("PARSED_TABLES"));
    aParsedTablesId = CreateArray("PARSED_TABLES");
    
    aCurrentSeg = FirstSeg();
    
    while (aCurrentSeg != BADADDR)
    {
        if (SegName(aCurrentSeg)==".rdata")
        {
            aMinRDataSeg = aCurrentSeg;
            aMaxRDataSeg = NextSeg(aCurrentSeg);
        }
        else if (SegName(aCurrentSeg)==".text")
        {
            aMinTextSeg = aCurrentSeg;
            aMaxTextSeg = NextSeg(aCurrentSeg);
        }
        else if (SegName(aCurrentSeg)==".data")
        {
            aMinDataSeg = aCurrentSeg;
            aMaxDataSeg = NextSeg(aCurrentSeg);
        }
        aCurrentSeg = NextSeg(aCurrentSeg);
    }
    
    if (aMinRDataSeg == 0)
    {
        aMinRDataSeg=aMinTextSeg;
        aMaxRDataSeg=aMaxTextSeg;
    }
    
    Message(".data: %08.8Xh - %08.8Xh, .rdata: %08.8Xh - %08.8Xh, .text %08.8Xh - %08.8Xh\n", aMinDataSeg, aMaxDataSeg, aMinRDataSeg, aMaxRDataSeg, aMinTextSeg, aMaxTextSeg);
    
    Message("Parsing .rdata for chunk_infos.\n");
    
    aCurrentAddress = aMinRDataSeg;
    while (aCurrentAddress < aMaxRDataSeg)
    {
        if (IS_ASCII(aCurrentAddress) && IS_ASCII(aCurrentAddress + 1) && IS_ASCII(aCurrentAddress + 2) && (Byte(aCurrentAddress + 3) == 0 || IS_ASCII(aCurrentAddress + 3)))
        {
            aChunkName = form("%c%c%c", Byte(aCurrentAddress), Byte(aCurrentAddress + 1), Byte(aCurrentAddress + 2));
            if (Byte(aCurrentAddress + 3) != 0)
            {
                aChunkName = form("%s%c", aChunkName, Byte(aCurrentAddress + 3));
            }
            
            aNbOfVersions = Dword(aCurrentAddress + 4);
            if (aNbOfVersions > 0 && aNbOfVersions < 100)
            {
                aANSTructTabOffset = Dword(aCurrentAddress + 8);
                if ((aMinRDataSeg < aANSTructTabOffset) && (aMaxRDataSeg > aANSTructTabOffset))
                {
                    if (isANStructTab(aANSTructTabOffset, aNbOfVersions))
                    {
                        if (!isIn(aParsedTablesId, aANSTructTabOffset))
                        {
                            add(aParsedTablesId, aANSTructTabOffset);
                            
                            fprintf(aOutputFile, "==================================================\n");
                            fprintf(aOutputFile, " Chunk: %s, versions: %d, strucTab: 0x%X\n", aChunkName, aNbOfVersions, aANSTructTabOffset);
                            fprintf(aOutputFile, "==================================================\n");
                            parseStructTab(aANSTructTabOffset, aNbOfVersions, aOutputFile);
                            fprintf(aOutputFile, "\n");
                        }
                    }
                }
            }
        }
        
        aCurrentAddress = aCurrentAddress + 4;
    }
    
    DeleteArray(aParsedTablesId);
    
    Message("ANet structs script ended.\n");
    
    fclose(aOutputFile);
    
    Exec("output.txt");
}