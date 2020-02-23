#include "BIOSReader.h"

#include <cstdint>

#include "Native.h"

struct smbios_structure_header
{
    uint8_t type;
    uint8_t length;
    uint16_t handle;
};

//
//	System information
//
struct smbios_type_1
{
    struct smbios_structure_header header;
    uint8_t manufacturer_str;
    uint8_t product_name_str;
    uint8_t version_str;
    uint8_t serial_number_str;
    uint8_t uuid[16];
    uint8_t wake_up_type;
    uint8_t sku_number_str;
    uint8_t family_str;
};



#define _TYPE_COUNT1 6

#define _CONCATE(x,y) x##y
#define CONCATE(x,y) _CONCATE(x,y)
#define TYPE_COUNT(t) CONCATE(_TYPE_COUNT, t)


//
//	Windows
//
#include <Windows.h>
#include <tchar.h>




int8_t * parse_smbiod_content(int8_t *addr, int8_t **indexes, int32_t *count)
{
    //!	ignore 0
    int8_t parsed_count = 0;

    int8_t *raw_addr = addr;

    //!	first one 
    if (indexes)
        *indexes = raw_addr;

    bool reach_terminal = false;

    while (true)
    {
        if (0 == *raw_addr++)
        {
            if (reach_terminal)
                break;
            else
            {
                ++parsed_count;

                if (count && parsed_count < *count)
                {
                    if (indexes)
                        *(indexes + parsed_count) = raw_addr;
                }

                reach_terminal = true;
            }
        }
        else
        {
            reach_terminal = false;
            continue;
        }
    }

    if (count)
        *count = parsed_count;

    return raw_addr;
}

void read_smbios_type_1(int8_t *addr, SystemInformation *info)
{
    smbios_type_1 *t1 = (smbios_type_1 *)addr;

    int32_t offset = ((0x0F) & (t1->header.length >> 4)) * 16 + (t1->header.length & 0x0F);

    int8_t *string_addr[TYPE_COUNT(1)] = { 0 };

    int32_t count = TYPE_COUNT(1);
    parse_smbiod_content((int8_t *)t1 + offset, string_addr, &count);

    if (0 != t1->manufacturer_str)
        info->Manufacturer = (std::string::traits_type::char_type *)string_addr[t1->manufacturer_str - 1];

    if (0 != t1->product_name_str)
        info->ProductName = (std::string::traits_type::char_type *)string_addr[t1->product_name_str - 1];

    if (0 != t1->serial_number_str)
        info->SerialNum = (std::string::traits_type::char_type *)string_addr[t1->serial_number_str - 1];

    if (0 != t1->version_str)
        info->SysVersion = (std::string::traits_type::char_type *)string_addr[t1->version_str - 1];
}

SystemInformation BIOSReader::readSystemInfo()
{
	SystemInformation info;

	uint32_t size = 0;
	RawSMBIOSData *data = (RawSMBIOSData *)(LocateSMBIOS(&size));

	if (NULL == data || 0 == size)
		return info;

    smbios_structure_header *header = (smbios_structure_header *)(data->SMBIOSTableData);

    while (NULL != header)
    {
        if (1 == header->type)
        {
            read_smbios_type_1((int8_t *)header, &info);
            header = NULL;	//!	stop
        }
        else
        {
            int32_t offset = ((0x0F) & (header->length >> 4)) * 16 + (header->length & 0x0F);
            header = (smbios_structure_header *)parse_smbiod_content((int8_t *)header + offset, NULL, NULL);
        }
    }

	free(data);
    return info;
}
