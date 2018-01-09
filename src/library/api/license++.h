#ifndef LICENSEPP_H_
#define LICENSEPP_H_

/*
 * This include file is the public api di License++
 * You should include this file if your software don't plan to use
 * the part of the library dealing with features.
 * Otherwise licensepp-features.h should be included.
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __unix__
#define DllExport
#ifndef MAX_PATH
	#define MAX_PATH 1024
#endif
#else
#include <windows.h>
#define DllExport  __declspec( dllexport )
#endif

#include "datatypes.h"

/*
 * Method used to convert the LicenseInfo into a human readable
 * representation.
 */
void print_error(char out_buffer[256], LicenseInfo* licenseInfo);

/**
 * This method calculate the pc identifier. The string has to be shown
 * to the user in order to calculate the license.
 */
void identify_pc(IDENTIFICATION_STRATEGY pc_id_method,
		char chbuffer[PC_IDENTIFIER_SIZE + 1]);
/*
 * The optional parameter License contains the information the program that uses the library
 * should display:
 *
 * @return true if successful. False if there are errors.
 * @param licenseLocation[in] licenseLocation, either the name of the file
 * 								or the name of the environment variable should be !='\0'
 * @param license[out] optional, can be NULL.
 */

EVENT_TYPE acquire_license(const char * productName,
		LicenseLocation licenseLocation, LicenseInfo* license);

/**
 * Do nothing for now, useful for network licenses.
 * Should be called from time to time to confirm we're still using the
 * slicense.
 */
EVENT_TYPE confirm_license(char * productName,
		LicenseLocation licenseLocation);
/**
 * Do nothing for now, useful for network licenses.
 */
EVENT_TYPE release_license(char * productName,
		LicenseLocation licenseLocation);

#ifdef __cplusplus
}
#endif

#endif
