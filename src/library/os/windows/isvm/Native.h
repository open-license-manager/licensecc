#pragma once

#include <stdint.h>
#include <Windows.h>

struct RawSMBIOSData
{
    BYTE    Used20CallingMethod;
    BYTE    SMBIOSMajorVersion;
    BYTE    SMBIOSMinorVersion;
    BYTE    DmiRevision;
    DWORD   Length;
    BYTE    SMBIOSTableData[];
};

bool InitEntryPoints();
void *LocateSMBIOS(uint32_t *smbios_size);

