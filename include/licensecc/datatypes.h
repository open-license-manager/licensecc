#ifndef DATATYPES_H_
#define DATATYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

// definition of size_t
#include <stdlib.h>
#include <stdint.h>
#ifndef _MSC_VER
#include <stdbool.h>
#endif

#ifdef __unix__
#define DllExport
#ifndef MAX_PATH
#define MAX_PATH 1024
#endif
#else
#include <windows.h>
#define DllExport __declspec(dllexport)
#endif

#define API_LICENSE_DATA_LENGTH 1024 * 4

// define api structure sizes
#define API_PC_IDENTIFIER_SIZE 19
#define API_PROPRIETARY_DATA_SIZE 16
#define API_AUDIT_EVENT_NUM 5
#define API_AUDIT_EVENT_PARAM2 255
#define API_VERSION_LENGTH 15
#define API_PROJECT_NAME_SIZE 15
#define API_EXPIRY_DATE_SIZE 10
#define API_ERROR_BUFFER_SIZE 256

typedef enum {
	LICENSE_OK = 0,  // OK
	LICENSE_FILE_NOT_FOUND = 1,  // license file not found
	LICENSE_SERVER_NOT_FOUND = 2,  // license server can't be contacted
	ENVIRONMENT_VARIABLE_NOT_DEFINED = 3,  // environment variable not defined
	FILE_FORMAT_NOT_RECOGNIZED = 4,  // license file has invalid format (not .ini file)
	LICENSE_MALFORMED = 5,  // some mandatory field are missing, or data can't be fully read.
	PRODUCT_NOT_LICENSED = 6,  // this product was not licensed
	PRODUCT_EXPIRED = 7,
	LICENSE_CORRUPTED = 8,  // License signature didn't match with current license
	IDENTIFIERS_MISMATCH = 9,  // Calculated identifier and the one provided in license didn't match

	LICENSE_SPECIFIED = 100,  // license location was specified
	LICENSE_FOUND = 101,  // License file has been found or license data has been located
	PRODUCT_FOUND = 102,  // License has been loaded and the declared product has been found
	SIGNATURE_VERIFIED = 103
} EVENT_TYPE;

typedef enum {
	LOCAL,
	REMOTE  // remote licenses are not supported now.
} LICENSE_TYPE;

typedef enum { SVRT_INFO, SVRT_WARN, SVRT_ERROR } SEVERITY;

typedef struct {
	SEVERITY severity;
	EVENT_TYPE event_type;
	/**
	 * License file name or location where the license is stored.
	 */
	char license_reference[MAX_PATH];
	char param2[API_AUDIT_EVENT_PARAM2 + 1];
} AuditEvent;

typedef enum {
	/**
	 * A list of absolute path separated by ';' containing the eventual location
	 * of the license files. Can be NULL.
	 */
	LICENSE_PATH,
	/**
	 * The license is provided as plain data
	 */
	LICENSE_PLAIN_DATA,
	/**
	 * The license is encoded
	 */
	LICENSE_ENCODED
} LICENSE_DATA_TYPE;

/**
 * This structure contains informations on the raw license data. Software authors
 * can specify the location of the license file or its full content.
 *
 * Can be NULL, in this case OpenLicenseManager will try to figure out the
 * license file location on its own.
 */
typedef struct {
	LICENSE_DATA_TYPE license_data_type;
	char licenseData[API_LICENSE_DATA_LENGTH];
} LicenseLocation;

/**
 * Informations on the software requiring the license
 */
typedef struct {
	char version[API_VERSION_LENGTH + 1];  // software version in format xxxx[.xxxx.xxxx] //TODO
	char project_name[API_PROJECT_NAME_SIZE + 1];  // name of the project (must correspond to the name in the license)
	/**
	 * this number passed in by the application must correspond to the magic number used when compiling the library.
	 * See cmake parameter -DLCC_PROJECT_MAGIC_NUM and licensecc_properties.h macro VERIFY_MAGIC
	 */
	unsigned int magic;
} CallerInformations;

typedef struct {
	/**
	 * Detailed reason of success/failure. Reasons for a failure can be
	 * multiple (for instance, license expired and signature not verified).
	 * Only the last AUDIT_EVENT_NUM are reported.
	 */
	AuditEvent status[API_AUDIT_EVENT_NUM];
	/**
	 * Eventual expiration date of the software,
	 * can be '\0' if the software don't expire
	 * */
	char expiry_date[API_EXPIRY_DATE_SIZE + 1];
	unsigned int days_left;
	bool has_expiry;
	bool linked_to_pc;
	LICENSE_TYPE license_type;  // Local or Remote
	/* A string of character inserted into the license understood
	 * by the calling application.
	 * '\0' if the application didn't specify one */
	char proprietary_data[API_PROPRIETARY_DATA_SIZE + 1];
	int license_version;  // license file version
} LicenseInfo;

/**
 * Enum to select a specific pc identification_strategy. DEFAULT Should be used
 * in most cases.
 */
typedef enum {
	STRATEGY_DEFAULT = -1,
	STRATEGY_ETHERNET = 0,
	STRATEGY_IP_ADDRESS = 1,
	STRATEGY_DISK_NUM = 1,
	STRATEGY_DISK_LABEL = 2,
	STRATEGY_PLATFORM_SPECIFIC = 3,
	STRATEGY_UNKNOWN = -2
} IDENTIFICATION_STRATEGY;

#ifdef __cplusplus
}
#endif

#endif
