#ifndef BIOSREADER_H
#define BIOSREADER_H

#include <string>

class SystemInformation
{
public:
    std::string Manufacturer;
    std::string ProductName;
    std::string SysVersion;
    std::string SerialNum;
};

class BIOSReader
{
public:
	SystemInformation readSystemInfo();
};

#endif
