//============================================================================
// Name        : license.cpp
// Author      :
// Version     :
// Copyright   : BSD
//============================================================================

#define __STDC_WANT_LIB_EXT1__ 1
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <cstdlib>

#include <licensecc/datatypes.h>
#include <licensecc/licensecc.h>
#include <licensecc_properties.h>

#include "base/logger.h"
#include "base/string_utils.h"
#include "hw_identifier/hw_identifier_facade.hpp"
#include "os/execution_environment.hpp"
#include "LicenseParser.hpp"
#include "Licensecc.hpp"

using namespace std;

void print_error(char out_buffer[LCC_API_ERROR_BUFFER_SIZE], LicenseInfo* licenseInfo) {}

bool identify_pc(LCC_API_HW_IDENTIFICATION_STRATEGY pc_id_method, char* chbuffer, size_t* bufSize,
				 ExecutionEnvironmentInfo* execution_environment_info) {
	static license::Licensecc facade;
	return facade.identify_pc(pc_id_method, chbuffer, bufSize, execution_environment_info);
}

LCC_EVENT_TYPE acquire_license(const CallerInformations* callerInformation, const LicenseLocation* licenseLocation,
							   LicenseInfo* license_out) {
	static license::Licensecc facade;
	return facade.acquire_license(callerInformation, licenseLocation, license_out);
}
