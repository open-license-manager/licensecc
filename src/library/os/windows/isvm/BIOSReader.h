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
	std::string family;
};

class BIOSReader
{
public:
	SystemInformation readSystemInfo();
};

#endif
