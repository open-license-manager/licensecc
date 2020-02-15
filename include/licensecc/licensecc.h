/** \addtogroup api
 * @{
 */
#ifndef LICENSEPP_H_
#define LICENSEPP_H_

/*
 * This include file is the public api di Licensecc
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <licensecc_properties.h>

#include "datatypes.h"

/**
 * Method used to convert the LicenseInfo into a human readable
 * representation.
 */
void print_error(char out_buffer[LCC_API_ERROR_BUFFER_SIZE], LicenseInfo* licenseInfo);

/**
 * This method calculates the hardware identifier. The string need to be shown to the user and given back to the software
 * editor when issuing a license.
 *  pc_id_method = STRATEGY_DEFAULT usually works.
 */
bool identify_pc(LCC_API_IDENTIFICATION_STRATEGY pc_id_method, char* identifier_out, size_t* bufSize);

/**
 * This method is used to request the use of one license for a product.
 * In case of local license it's used to check if the product is licensed.
 *
 * @return LICENSE_OK(0) if successful. Other values if there are errors.
 * @param productName[in]
 * 			a vendor defined string containing the name of the product we want to request.
 * @param licenseLocation[in] otpional, can be NULL.
 * 					licenseLocation, either the name of the file
 * 								or the name of the environment variable should be !='\0'
 * @param license[out] optional, can be NULL, if set it will return extra informations about the license.
 */

LCC_EVENT_TYPE acquire_license(const CallerInformations* callerInformation, const LicenseLocation* licenseLocation,
						   LicenseInfo* license_out);

/**
 * Do nothing for now, useful for network licenses.
 * Should be called from time to time to confirm we're still using the
 * license.
 */
LCC_EVENT_TYPE confirm_license(char* featureName, LicenseLocation* licenseLocation);
/**
 * Do nothing for now, useful for network licenses.
 */
LCC_EVENT_TYPE release_license(char* featureName, LicenseLocation licenseLocation);

#ifdef __cplusplus
}
#endif

#endif
/**
 * @}
 */
