#include "Native.h"

#include <tchar.h>

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS; // windbgkd

typedef LONG NTSTATUS;
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2)] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

typedef enum _SECTION_INHERIT
{
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_VALID_ATTRIBUTES    0x000001F2L

static bool bIsWindowsXPLater = false;
static DWORD dwPageSize = 0;

#ifdef _UNICODE
#define GetVersionExProc "GetVersionExW"
#else
#define GetVersionExProc "GetVersionExA"
#endif

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef struct SMBIOSEntryPoint
{
    char EntryPointString[4];
    uint8_t Checksum;
    uint8_t Length;
    uint8_t MajorVersion;
    uint8_t MinorVersion;
    uint16_t MaxStructureSize;
    uint8_t EntryPointRevision;
    char FormattedArea[5];
    char EntryPointString2[5];
    uint8_t Checksum2;
    uint16_t TableLength;
    uint32_t TableAddress;
    uint16_t NumberOfStructures;
    uint8_t BCDRevision;
} SMBIOSEntryPoint, *PSMBIOSEntryPoint;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES);          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
}

NTSTATUS(WINAPI *NtUnmapViewOfSection)(
    IN HANDLE  ProcessHandle,
    IN PVOID  BaseAddress
    );

NTSTATUS(WINAPI *NtOpenSection)(
    OUT PHANDLE  SectionHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes
    );

NTSTATUS(WINAPI *NtMapViewOfSection)(
    IN HANDLE  SectionHandle,
    IN HANDLE  ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG  ZeroBits,
    IN ULONG  CommitSize,
    IN OUT PLARGE_INTEGER  SectionOffset,	/* optional */
    IN OUT PULONG  ViewSize,
    IN SECTION_INHERIT  InheritDisposition,
    IN ULONG  AllocationType,
    IN ULONG  Protect
    );

VOID(WINAPI *RtlInitUnicodeString)(
    IN OUT PUNICODE_STRING  DestinationString,
    IN PCWSTR  SourceString
    );

ULONG(WINAPI *RtlNtStatusToDosError) (
    IN NTSTATUS Status
    );

UINT(WINAPI *Win32GetSystemFirmwareTable)(
    _In_ DWORD FirmwareTableProviderSignature,
    _In_ DWORD FirmwareTableID,
    _Out_writes_bytes_to_opt_(BufferSize, return) PVOID pFirmwareTableBuffer,
    _In_ DWORD BufferSize);

BOOL(WINAPI *Win32GetVersionEx)(
    _Inout_  LPOSVERSIONINFO lpVersionInfo
    );

VOID(WINAPI *Win32GetSystemInfo)(
    _Out_  LPSYSTEM_INFO lpSystemInfo
    );

BOOL(WINAPI *Win32VirtualProtect)(
    _In_   LPVOID lpAddress,
    _In_   SIZE_T dwSize,
    _In_   DWORD flNewProtect,
    _Out_  PDWORD lpflOldProtect
    );

//----------------------------------------------------------------------
//
// PrintError
//
// Formats an error message for the last error
//
// Mark Russinovich
// Systems Internals
// http://www.sysinternals.com
//----------------------------------------------------------------------
void PrintError(char *message, NTSTATUS status)
{
    char *errMsg;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, RtlNtStatusToDosError(status),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errMsg, 0, NULL);
    LocalFree(errMsg);
}

//--------------------------------------------------------
//
// UnmapPhysicalMemory
//

// Maps a view of a section.
//
// Mark Russinovich
// Systems Internals
// http://www.sysinternals.com
//--------------------------------------------------------
static VOID UnmapPhysicalMemory(PVOID Address)
{
    NTSTATUS		status;

    status = NtUnmapViewOfSection((HANDLE)-1, Address);
    if (!NT_SUCCESS(status))
    {
        PrintError("Unable to unmap view", status);
    }
}


//--------------------------------------------------------
//
// MapPhysicalMemory
//
// Maps a view of a section.
//
// Mark Russinovich
// Systems Internals
// http://www.sysinternals.com
//--------------------------------------------------------
static BOOLEAN MapPhysicalMemory(HANDLE PhysicalMemory,
    PVOID Address, PDWORD Length,
    PVOID *VirtualAddress)
{
    NTSTATUS			ntStatus;
    PHYSICAL_ADDRESS	viewBase;
    char				error[256];

    viewBase.QuadPart = (ULONGLONG)(Address);
    ntStatus = NtMapViewOfSection(PhysicalMemory,
        (HANDLE)-1,
        VirtualAddress,
        0L, *Length,
        &viewBase,
        Length,
        ViewShare,
        0,
        PAGE_READONLY);

    if (!NT_SUCCESS(ntStatus)) {

        PrintError(error, ntStatus);
        return FALSE;
    }

    return TRUE;
}


//--------------------------------------------------------
//
// OpensPhysicalMemory
//
// This function opens the physical memory device. It
// uses the native API since 
//
// Mark Russinovich
// Systems Internals
// http://www.sysinternals.com
//--------------------------------------------------------
static HANDLE OpenPhysicalMemory()
{
    NTSTATUS		status;
    HANDLE			physmem;
    UNICODE_STRING	physmemString;
    OBJECT_ATTRIBUTES attributes;
    WCHAR			physmemName[] = L"\\device\\physicalmemory";

    RtlInitUnicodeString(&physmemString, physmemName);

    InitializeObjectAttributes(&attributes, &physmemString,
        OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtOpenSection(&physmem, SECTION_MAP_READ, &attributes);

    if (!NT_SUCCESS(status))
    {
        PrintError("Could not open \\device\\physicalmemory", status);
        return NULL;
    }

    return physmem;
}

static PVOID MapPhysicalMemoryWithBase(HANDLE hPhyHandle, PVOID pBase, PDWORD pLen, PVOID *pVirtualBase)
{
    DWORD dwOffset = (ULONGLONG)pBase % dwPageSize;
    DWORD dwLen = *pLen + dwOffset;

    PVOID pVAddr = NULL;

    if (MapPhysicalMemory(hPhyHandle, pBase, &dwLen, &pVAddr))
    {
        *pVirtualBase = pVAddr;
        *pLen = dwLen;

        return (PBYTE)pVAddr + dwOffset;
    }
    else
    {
        return NULL;
    }
}

bool InitEntryPoints()
{
    Win32GetVersionEx = decltype(Win32GetVersionEx)(GetProcAddress(::GetModuleHandle(_T("kernel32.dll")), GetVersionExProc));
    if (!Win32GetVersionEx)
        return false;

    Win32GetSystemInfo = decltype(Win32GetSystemInfo)(GetProcAddress(::GetModuleHandle(_T("kernel32.dll")), "GetSystemInfo"));
    if (!Win32GetSystemInfo)
        return false;

    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (!Win32GetVersionEx(&osvi))
        return false;

    bIsWindowsXPLater = ((osvi.dwMajorVersion > 5) || ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion > 1)));

    SYSTEM_INFO sysinfo;
    ::ZeroMemory(&sysinfo, sizeof (SYSTEM_INFO));
    Win32GetSystemInfo(&sysinfo);

    dwPageSize = sysinfo.dwPageSize;

    if (bIsWindowsXPLater)
    {
        Win32GetSystemFirmwareTable = decltype(Win32GetSystemFirmwareTable)(GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetSystemFirmwareTable"));
        if (!Win32GetSystemFirmwareTable)
            return false;
    }
    else
    {
        Win32VirtualProtect = decltype(Win32VirtualProtect)(GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "VirtualProtect"));
        if (!Win32VirtualProtect)
            return false;

        RtlInitUnicodeString = (decltype(RtlInitUnicodeString))(GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "RtlInitUnicodeString"));
        if (!RtlInitUnicodeString)
            return false;

        NtUnmapViewOfSection = (decltype(NtUnmapViewOfSection))(GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtUnmapViewOfSection"));
        if (!NtUnmapViewOfSection)
            return false;

        NtOpenSection = (decltype(NtOpenSection))(GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtOpenSection"));
        if (!NtOpenSection)
            return false;

        NtMapViewOfSection = (decltype(NtMapViewOfSection))(GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtMapViewOfSection"));
        if (!NtMapViewOfSection)
            return false;

        RtlNtStatusToDosError = (decltype(RtlNtStatusToDosError))(GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "RtlNtStatusToDosError"));
        if (!NtMapViewOfSection)
            return false;
    }

    return true;
}

void *LocateSMBIOS(uint32_t *smbios_size)
{
    void *buf = NULL;
    if (bIsWindowsXPLater)
    {
        uint32_t size = 0;
        size = Win32GetSystemFirmwareTable('RSMB', 0, buf, size);
        if (0 == size)
        {
            return NULL;
        }

        buf = malloc(size);
        if (buf)
        {
            if (0 == Win32GetSystemFirmwareTable('RSMB', 0, buf, size))
            {
                free(buf);
                buf = NULL;
            }
            else
            {
                *smbios_size = size;
            }
        }
    }
    else
    {
        HANDLE hPhysMem = OpenPhysicalMemory();
        if (NULL == hPhysMem)
            return NULL;

        DWORD dwReadLen = 0x10000;
        DWORD dwActualLen = dwReadLen;
        PVOID pBaseVAddr = NULL;
        PVOID pVAddr = MapPhysicalMemoryWithBase(hPhysMem, (PVOID)0xF0000, &dwActualLen, &pBaseVAddr);
        if (!pVAddr)
        {
            ::CloseHandle(hPhysMem);
            return NULL;
        }

        DWORD dwReadOffset = 0;

        PBYTE pbVAddr = (PBYTE)pVAddr;
        PBYTE pbGuardVAddr = pbVAddr + dwReadLen;

        while (pbVAddr < pbGuardVAddr)
        {
            if (pbVAddr[0] == '_' && pbVAddr[1] == 'S' && pbVAddr[2] == 'M' && pbVAddr[3] == '_')
            {
                break;
            }

            pbVAddr += 16;
        }

        //!	no SMBIOS found
        if (pbVAddr >= pbGuardVAddr)
        {
            UnmapPhysicalMemory(pVAddr);
            ::CloseHandle(hPhysMem);

            return NULL;
        }

        PSMBIOSEntryPoint pEntryPoint = (PSMBIOSEntryPoint)pbVAddr;

        RawSMBIOSData *pData = (RawSMBIOSData *)::malloc(pEntryPoint->TableLength + sizeof(RawSMBIOSData));
        PVOID pTableBaseVAddr = NULL;
        if (NULL != pData)
        {
            DWORD dwTableLen = pEntryPoint->TableLength;
            PVOID pTableVAddr = MapPhysicalMemoryWithBase(hPhysMem, (PVOID)pEntryPoint->TableAddress, &dwTableLen, &pTableBaseVAddr);
            if (!pTableVAddr)
            {
                UnmapPhysicalMemory(pBaseVAddr);
                ::CloseHandle(hPhysMem);
                return NULL;
            }

            pData->Used20CallingMethod = 0;
            pData->DmiRevision = 0;
            pData->SMBIOSMajorVersion = pEntryPoint->MajorVersion;
            pData->SMBIOSMinorVersion = pEntryPoint->MinorVersion;
            pData->Length = pEntryPoint->TableLength;

            ::memcpy(pData->SMBIOSTableData, (PVOID)pTableVAddr, pEntryPoint->TableLength);
            *smbios_size = pEntryPoint->TableLength;
        }

        if (NULL != pTableBaseVAddr)
            UnmapPhysicalMemory(pTableBaseVAddr);
        if (NULL != pBaseVAddr)
            UnmapPhysicalMemory(pBaseVAddr);

        ::CloseHandle(hPhysMem);

        buf = pData;
    }

    return buf;
}
