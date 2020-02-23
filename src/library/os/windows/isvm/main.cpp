#include "intrin.h"

#include <iostream>
#include "BIOSReader.h"
#include "Native.h"

int _tmain(int argc, _TCHAR* argv[])
{
    int cpui[4] = { 0 };
    __cpuid(cpui, 0x1);

    if ((cpui[2] >> 31) & 1)
    {
        std::cout << "Inside virual machine!";
        return 1;
    }

    if (InitEntryPoints())
    {
        BIOSReader reader;
        SystemInformation info = reader.readSystemInfo();

        const char *vmVendors[] =
        {
            "VMware", "Microsoft Corporation", "Virtual Machine", "innotek GmbH", "PowerVM", "Bochs", "KVM"
        };

        const int count = _countof(vmVendors);
        for (int i = 0; i != count; ++i)
        {
            const char *vendor = vmVendors[i];

            if (std::string::npos != info.Manufacturer.find(vendor) ||
                std::string::npos != info.ProductName.find(vendor) ||
                std::string::npos != info.SerialNum.find(vendor))
            {
                std::cout << "Inside virual machine!";
                return 1;
            }
        }
    }
    else
    {
        return -1;
    }

    std::cout << "Inside host machine!";

    return 0;
}

