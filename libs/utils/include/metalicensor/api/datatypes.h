#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <stdlib.h>
#ifdef __unix__
#include <stdbool.h>
#define DllExport
#else
#include <windows.h>
#define DllExport  __declspec( dllexport )
#endif

#define ENVIRONMENT_VAR_NAME_MAX 64
#define PC_IDENTIFIER_SIZE 18
#define PROPRIETARY_DATA_SIZE 16

#define LICENESE_INT_VERSION 100
#define LICENSEPP_VERSION "1.0.0"

enum EVENT_TYPE {
	LICENSE_OK = 0, 						//OK
	LICENSE_FILE_NOT_FOUND = 1, 		//license file not found
	LICENSE_SERVER_NOT_FOUND = 2, 		//license server can't be contacted
	ENVIRONMENT_VARIABLE_NOT_DEFINED = 3, //environment variable not defined
	FILE_FORMAT_NOT_RECOGNIZED = 4,	//license file has invalid format (not .ini file)
	LICENSE_MALFORMED = 5, //some mandatory field are missing, or data can't be fully read.
	PRODUCT_NOT_LICENSED = 6,		//this product was not licensed
	PRODUCT_EXPIRED = 7,
	LICENSE_CORRUPTED = 8,//License signature didn't match with current license
	IDENTIFIERS_MISMATCH = 9, //Calculated identifier and the one provided in license didn't match

	LICENSE_FILE_FOUND = 100,
	LICENSE_VERIFIED = 101

};

enum LICENSE_TYPE {
	LOCAL,
    REMOTE //remote licenses are not supported now.
};

enum SEVERITY{
	SVRT_INFO,
    SVRT_WARN,
    SVRT_ERROR
};

struct AuditEvent {
	SEVERITY severity;
	EVENT_TYPE event_type;
	char param1[256];
	char param2[256];
};

struct LicenseLocation {
	const char *licenseFileLocation;
	const char *environmentVariableName;
	bool openFileNearModule;
};

struct LicenseInfo {
	/**
	 * Detailed reason of success/failure. Reasons for a failure can be
	 * multiple (for instance, license expired and signature not verified).
	 * Only the last 5 are reported.
	 */
	AuditEvent status[5];
	/**
	 * Eventual expiration date of the software,
	 * can be '\0' if the software don't expire
	 * */
	char expiry_date[11];
	unsigned int days_left;bool has_expiry;bool linked_to_pc;
	LICENSE_TYPE license_type; // Local or Remote
	/* A string of character inserted into the license understood
	 * by the calling application.
	 * '\0' if the application didn't specify one */
	char proprietary_data[PROPRIETARY_DATA_SIZE + 1];
	int license_version; //license file version
};

/**
 * Enum to select a specific pc identification_strategy. DEFAULT Should be used
 * in most cases.
 */
enum IDENTIFICATION_STRATEGY {
	DEFAULT,
	ETHERNET,
	IP_ADDRESS,
	DISK_NUM,
	DISK_LABEL,
	PLATFORM_SPECIFIC,
	STRATEGY_UNKNOWN
};

#endif
