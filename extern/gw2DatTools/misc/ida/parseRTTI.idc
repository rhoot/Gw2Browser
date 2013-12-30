#include <idc.idc>

static GetAsciizStr(anOffset)
{
  auto aResult, aByte;
  aResult = "";
  while (aByte=Byte(anOffset))
  {
    aResult = form("%s%c", aResult, aByte);
    anOffset = anOffset + 1;
  }
  return aResult;
}

static MangleNumber(aNumber)
{
//
// 0 = A@
// X = X-1 (1<=X<=10)
// -X = ?(X-1)
// 0x0..0xF = 'A'..'P'

    auto aMangledName, aSign;

    aMangledName = "";
    aSign = "";

    if (aNumber < 0)
    {
        aSign = "?";
        aNumber = -aNumber;
    }  
    
    if (aNumber == 0)
    {
        return "A@";
    }
    else if (aNumber <= 10)
    {
        return form("%s%d", aSign, aNumber-1);
    }
    else
    {
        while (aNumber > 0)
        {
            aMangledName = form("%c%s", 'A' + aNumber%16, aMangledName);
            aNumber = aNumber / 16;
        }
        return aSign + aMangledName + "@";
    }
}

/*
struct RTTITypeDescriptor
{
    void* pVFTable;  //always pointer to type_info::vftable ?
    void* spare;     //seems to be zero for most classes, and default constructor for exceptions
    char name[0];    //mangled name, starting with .?A (.?AV=classes, .?AU=structs)
};

struct RTTIPMD
{
    int mdisp;  //member displacement
    int pdisp;  //vbtable displacement
    int vdisp;  //displacement inside vbtable
};

struct RTTIBaseClassDescriptor
{
    struct RTTITypeDescriptor* pTypeDescriptor; //type descriptor of the class
    DWORD numContainedBases; //number of nested classes following in the array
    struct RTTIPMD where;        //some displacement info
    DWORD attributes;        //usually 0, sometimes 10h
};

struct RTTIClassHierarchyDescriptor
{
    DWORD signature;      //always zero?
    DWORD attributes;     //bit 0 = multiple inheritance, bit 1 = virtual inheritance
    DWORD numBaseClasses; //number of classes in pBaseClassArray
    struct RTTIBaseClassArray* pBaseClassArray;
};

struct RTTICompleteObjectLocator
{
    DWORD signature; //always zero ?
    DWORD offset;    //offset of this vtable in the class ?
    DWORD cdOffset;  //no idea
    struct RTTITypeDescriptor* pTypeDescriptor; //TypeDescriptor of the class
    struct RTTIClassHierarchyDescriptor* pClassDescriptor; //inheritance hierarchy
};

*/


static makeRttiStruct(void)
{
    auto aTDStructId, aTDName;
    auto aPMDStructId, aPMDName;
    auto aBCDStructId, aBCDName;
    auto aCHDStructId, aCHDName;
    auto aCOLStructId, aCOLName;
    
    aTDName = "RTTITypeDescriptor";
    aPMDName = "RTTIPMD";
    aBCDName = "RTTIBaseClassDescriptor";
    aCHDName = "RTTIClassHierarchyDescriptor";
    aCOLName = "RTTICompleteObjectLocator";
    
    aTDStructId = GetStrucIdByName(aTDName);
    if (aTDStructId != -1)
    {
        DelStruc(aTDStructId);
    }
    aTDStructId = AddStruc(-1, aTDName);
    AddStrucMember(aTDStructId, "pVFTable", 0, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aTDStructId, "pSpare", 4, FF_DWRD|FF_DATA, -1, 4);
    
    aPMDStructId = GetStrucIdByName(aPMDName);
    if (aPMDStructId != -1)
    {
        DelStruc(aPMDStructId);
    }
    aPMDStructId = AddStruc(-1, aPMDName);
    AddStrucMember(aPMDStructId, "mDisp", 0, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aPMDStructId, "pDisp", 4, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aPMDStructId, "vDisp", 8, FF_DWRD|FF_DATA, -1, 4);
    
    aBCDStructId = GetStrucIdByName(aBCDName);
    if (aBCDStructId != -1)
    {
        DelStruc(aBCDStructId);
    }
    aBCDStructId = AddStruc(-1, aBCDName);
    AddStrucMember(aBCDStructId, "pTypeDescriptor", 0, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aBCDStructId, "numContainedBases", 4, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aBCDStructId, "where", 8, FF_STRU, aPMDStructId, 12);
    AddStrucMember(aBCDStructId, "attributes", 20, FF_DWRD|FF_DATA, -1, 4);
    
    aCHDStructId = GetStrucIdByName(aCHDName);
    if (aCHDStructId != -1)
    {
        DelStruc(aCHDStructId);
    }
    aCHDStructId = AddStruc(-1, aCHDName);
    AddStrucMember(aCHDStructId, "signature", 0, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aCHDStructId, "attributes", 4, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aCHDStructId, "numBaseClasses", 8, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aCHDStructId, "pBaseClassArray", 12, FF_DWRD|FF_DATA, -1, 4);
    
    aCOLStructId = GetStrucIdByName(aCOLName);
    if (aCOLStructId != -1)
    {
        DelStruc(aCOLStructId);
    }
    aCOLStructId = AddStruc(-1, aCOLName);
    AddStrucMember(aCOLStructId, "signature", 0, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aCOLStructId, "offset", 4, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aCOLStructId, "cdOffset", 8, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aCOLStructId, "pTypeDescriptor", 12, FF_DWRD|FF_DATA, -1, 4);
    AddStrucMember(aCOLStructId, "pClassHierarchyDescriptor", 16, FF_DWRD|FF_DATA, -1, 4);
}

static MakeStructForce(anAddress, aStructName)
{
    MakeUnknown(anAddress, GetStrucSize(GetStrucIdByName(aStructName)), 0);
    return MakeStruct(anAddress, aStructName);
}

static MakeNameSafe(anAddress, aName)
{
    if (!hasName(GetFlags(anAddress)))
    {
        MakeName(anAddress, aName);
    }
}

static makeCOL(anAddress, aVTableAddress, aName)
{
    auto aCHDAddress, aBCAAddress, aNumBaseClasses, aBCAItem, aFoundBaseClass, i;
    auto aBaseClassName, aVTableName, aReturnName;
    
    aReturnName = aName;
    aVTableName = "";
    if (!MakeStructForce(anAddress, "RTTICompleteObjectLocator"))
    {
        Message("An error occured while trying to change an offset to a RTTICompleteObjectLocator: %X\n", anAddress);
    }
    else
    {
        aBaseClassName = "";
        aCHDAddress = Dword(anAddress + 16);
        // COL offset == 0 && CHD attributes & 3 == 0
        if (Dword(anAddress + 4) == 0 
            && (Dword(aCHDAddress + 4) & 3 == 0))
        {
            // Single inheritance no conflict
            
            //??_R4A@@6B@ = A::`RTTI Complete Object Locator'
            MakeNameSafe(anAddress, "??_R4" + substr(aName, 4, -1) + "6B@");
            MakeNameSafe(aVTableAddress, "??_7" + substr(aName, 4, -1) + "6B@");
        }
        else
        {
            aBCAAddress = Dword(aCHDAddress + 12);
            aNumBaseClasses = Dword(aCHDAddress + 8);
            
            i = 0;
            aFoundBaseClass = 0;
            
            while (i < aNumBaseClasses)
            {
                aBCAItem = aBCAAddress + i * 4;
                if (Dword(Dword(aBCAItem) + 8) == Dword(anAddress + 4))
                {
                    aBaseClassName = GetAsciizStr(Dword(Dword(aBCAItem)) + 8);
                    aFoundBaseClass = 1;
                    break;
                }
                i = i + 1;
            }
            
            if (!aFoundBaseClass)
            {
                i = 0;
                while (i < aNumBaseClasses)
                {
                    aBCAItem = aBCAAddress + i * 4;
                    if (Dword(Dword(aBCAItem) + 12) != -1)
                    {
                        aBaseClassName = GetAsciizStr(Dword(Dword(aBCAItem)) + 8);
                        aFoundBaseClass = 1;
                        break;
                    }
                    i = i + 1;
                }
                if (!aFoundBaseClass)
                {
                    Message("An error occured while trying to fetch the name of the base class for the RTTICompleteObjectLocator at address: %X\n", anAddress);
                }
            }
            
            aReturnName = substr(aName, 4, -1) + "6B" + substr(aBaseClassName, 4, -1) + "@";
            
            MakeNameSafe(anAddress, "??_R4" + substr(aName, 4, -1) + "6B" + substr(aBaseClassName, 4, -1) + "@");
            MakeNameSafe(aVTableAddress, "??_7" + substr(aName, 4, -1) + "6B" + substr(aBaseClassName, 4, -1) + "@");
        }
        makeCHD(Dword(anAddress + 16), aName);
    }
    
    return aReturnName;
}

static makeTD(anAddress, aName)
{
    if (!MakeStructForce(anAddress, "RTTITypeDescriptor"))
    {
        Message("An error occured while trying to change an offset to a RTTITypeDescriptor: %X\n", anAddress);
    }
    else
    {
        //??_R0?AVA@@@8 = A::`RTTI Type Descriptor'
        MakeNameSafe(anAddress, "??_R0" + substr(aName, 1, -1) + "@8");
    }
}

static makeBCD(anAddress, aName)
{
    auto aBCName;
    
    if (!MakeStructForce(anAddress, "RTTIBaseClassDescriptor"))
    {
        Message("An error occured while trying to change an offset to a RTTIBaseClassDescriptor: %X\n", anAddress);
    }
    else
    {
        aBCName = GetAsciizStr(Dword(anAddress) + 8);
        
        MakeNameSafe(anAddress, "??_R1" 
                                + MangleNumber(Dword(anAddress+8))
                                + MangleNumber(Dword(anAddress+12))
                                + MangleNumber(Dword(anAddress+16))
                                + MangleNumber(Dword(anAddress+20))
                                + substr(aBCName, 4, -1)
                                + "8");
    }
}

static makeCHD(anAddress, aName)
{
    auto aBaseClassArray, aBaseClassAddress;
    auto i, aNumBaseClasses;
    if (!MakeStructForce(anAddress, "RTTIClassHierarchyDescriptor"))
    {
        Message("An error occured while trying to change an offset to a RTTIClassHierarchyDescriptor: %X\n", anAddress);
    }
    else
    {
        //??_R3A@@8 = A::`RTTI Class Hierarchy Descriptor'
        MakeNameSafe(anAddress,"??_R3" + substr(aName,4,-1) + '8');
        
        aBaseClassArray = Dword(anAddress + 12);
        
        //??_R2A@@8 = A::`RTTI Base Class Array'
        MakeNameSafe(aBaseClassArray,"??_R2" + substr(aName,4,-1) + '8');
        
        aNumBaseClasses = Dword(anAddress + 8);
        
        MakeUnknown(aBaseClassArray, aNumBaseClasses * 4, 0);
        
        i = 0;
        while (i < aNumBaseClasses)
        {
            makeBCD(Dword(aBaseClassArray + i * 4), aName);
            MakeDword(aBaseClassArray + i * 4);
            i = i + 1;
        }
    }
}

#define IS_TD(address) ((Dword(address) == aVTableTypeInfoAddress) && ((Dword(address + 8) & 0xFFFFFF) == 0x413F2E)) // .?AV

static main(void)
{
    // First step detecting rdata and text segments
    auto aCurrentSeg, aCurrentAddress, aMiscAddress;
    auto aMinDataSeg, aMaxDataSeg;
    auto aMinRDataSeg, aMaxRDataSeg;
    auto aMinTextSeg, aMaxTextSeg;
    
    auto aVTableTypeInfoAddress;
    
    auto aName, aVTableName, aBaseClassName;
    
    auto anArrayIndex, i, j;
    auto aMaxBaseClassNum, aBaseClassNum;
    auto aVTableAddressArray, aVTableBaseClassNumArray, aVTableClassNameArray;
    auto aVTableAddressArrayId, aVTableBaseClassNumArrayId, aVTableClassNameArrayId;
    
    aMaxBaseClassNum = 0;
    anArrayIndex = 0;
    
    aVTableAddressArray = "aVTableAddressArray";
    aVTableBaseClassNumArray = "aVTableBaseClassNumArray";
    aVTableClassNameArray = "aVTableClassNameArray";
    
    
    aVTableAddressArrayId = GetArrayId(aVTableAddressArray);
    aVTableBaseClassNumArrayId = GetArrayId(aVTableBaseClassNumArray);
    aVTableClassNameArrayId = GetArrayId(aVTableClassNameArray);
    
    DeleteArray(aVTableAddressArrayId);
    DeleteArray(aVTableBaseClassNumArrayId);
    DeleteArray(aVTableClassNameArrayId);
    
    aVTableAddressArrayId = CreateArray(aVTableAddressArray);
    aVTableBaseClassNumArrayId = CreateArray(aVTableBaseClassNumArray);
    aVTableClassNameArrayId = CreateArray(aVTableClassNameArray);
    
    Message("vtables_rtti script started.\n");
    
    makeRttiStruct();
    
    aMinDataSeg = 0;
    aMaxDataSeg = 0;
    aMinRDataSeg = 0;
    aMaxRDataSeg = 0;
    aMinTextSeg = 0;
    aMaxTextSeg = 0;
    
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
    
    Message("Parsing .data for type_info TypeDescriptor.\n");
    
    aVTableTypeInfoAddress = 0;
    aCurrentAddress = aMinDataSeg;
    while (aCurrentAddress < aMaxDataSeg)
    {
        // .?AVtype_info@@
        if (Dword(aCurrentAddress) == 0x56413F2E         // .?AV
            && Dword(aCurrentAddress + 4) == 0x65707974  // type
            && Dword(aCurrentAddress + 8) == 0x666e695f  // _inf
            && Dword(aCurrentAddress + 12) == 0x0040406f) // o@@\0
        {
            aVTableTypeInfoAddress = Dword(aCurrentAddress - 8);
            break;
        }
        aCurrentAddress = aCurrentAddress + 4;
    }
    
    if (!aVTableTypeInfoAddress)
    {
        Message("Type_info TypeDescriptor not found, aborting.\n");
        return;
    }
    else
    {
        Message("Found type_info TypeDescriptor VTable at: %X\n", aVTableTypeInfoAddress);
        
        Message("Parsing .data for all TypeDescriptors.\n");
        
        aCurrentAddress = aMinDataSeg;
        while (aCurrentAddress < aMaxDataSeg)
        {
            if (IS_TD(aCurrentAddress))
            {
                aName = GetAsciizStr(aCurrentAddress + 8);
                makeTD(aCurrentAddress, aName);
            }
            
            aCurrentAddress = aCurrentAddress + 4;
        }
        
        Message("Parsed all TypeDescriptors.\n");
    }
    
    Message("Parsing .rdata for VTables.\n");
    
    aCurrentAddress = aMinRDataSeg;
    while (aCurrentAddress < aMaxRDataSeg)
    {
        aMiscAddress = Dword(aCurrentAddress);
        if (aMinTextSeg < aMiscAddress
            && aMaxTextSeg > aMiscAddress)
        {
            aMiscAddress = Dword(aCurrentAddress - 4);
            if (aMinRDataSeg < aMiscAddress
                && aMaxRDataSeg > aMiscAddress)
            {
                aMiscAddress = Dword(aMiscAddress + 12);
                if (IS_TD(aMiscAddress))
                {
                    aName = GetAsciizStr(aMiscAddress + 8);
                    aBaseClassName = makeCOL(Dword(aCurrentAddress - 4), aCurrentAddress, aName);
                    MakeNameSafe(aCurrentAddress, aVTableName);
                    
                    aBaseClassNum = Dword(Dword(Dword(aCurrentAddress - 4) + 16) + 8);
                    
                    if (aBaseClassNum > aMaxBaseClassNum)
                    {
                        aMaxBaseClassNum = aBaseClassNum;
                    }
                    
                    SetArrayLong(aVTableAddressArrayId, anArrayIndex, aCurrentAddress);
                    SetArrayLong(aVTableBaseClassNumArrayId, anArrayIndex, aBaseClassNum);
                    SetArrayString(aVTableClassNameArrayId, anArrayIndex, aBaseClassName);
                    
                    anArrayIndex = anArrayIndex + 1;
                }
            }
        }
        aCurrentAddress = aCurrentAddress + 4;
    }
    
    i = 0;
    while (i <= aMaxBaseClassNum)
    {
        j = 0;
        while (j < anArrayIndex)
        {
            if (GetArrayElement(AR_LONG, aVTableBaseClassNumArrayId, j) == i)
            {
                aCurrentAddress = GetArrayElement(AR_LONG, aVTableAddressArrayId, j);
                while (aMinTextSeg < Dword(aCurrentAddress)
                       && aMaxTextSeg > Dword(aCurrentAddress))
                {
                    aMiscAddress = Dword(aCurrentAddress);
                    if (GetFunctionAttr(aMiscAddress, FUNCATTR_START) != aMiscAddress)
                    {
                        if (isUnknown(GetFlags(aMiscAddress)))
                        {
                            if (!MakeFunction(aMiscAddress, BADADDR))
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    aName = form("%s%d", "?virt_", aCurrentAddress - GetArrayElement(AR_LONG, aVTableAddressArrayId, j));
                    MakeNameSafe(aMiscAddress, aName + "@" + GetArrayElement(AR_STR, aVTableClassNameArrayId, j));
                    MakeDword(aCurrentAddress);
                    aCurrentAddress = aCurrentAddress + 4;
                }
            }
            j = j + 1;
        }
        i = i + 1;
    }
    
    DeleteArray(aVTableAddressArrayId);
    DeleteArray(aVTableBaseClassNumArrayId);
    DeleteArray(aVTableClassNameArrayId);
    
    Message("vtables_rtti script ended.\n");
}