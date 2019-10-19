#include <iostream>
#include <map>
#include "api/license++.h"
#include "pc-identifiers.h"

using namespace std;

int main(int argc, char *argv[]) {
	map<EVENT_TYPE, string> stringByEventType;
	stringByEventType[LICENSE_OK] = "OK ";
	stringByEventType[LICENSE_FILE_NOT_FOUND] = "license file not found ";
	stringByEventType[LICENSE_SERVER_NOT_FOUND] = "license server can't be contacted ";
	stringByEventType[ENVIRONMENT_VARIABLE_NOT_DEFINED] = "environment variable not defined ";
	stringByEventType[FILE_FORMAT_NOT_RECOGNIZED] = "license file has invalid format (not .ini file) ";
	stringByEventType[LICENSE_MALFORMED] = "some mandatory field are missing, or data can't be fully read. ";
	stringByEventType[PRODUCT_NOT_LICENSED] = "this product was not licensed ";
	stringByEventType[PRODUCT_EXPIRED] = "license expired ";
	stringByEventType[LICENSE_CORRUPTED] = "license signature didn't match with current license ";
	stringByEventType[IDENTIFIERS_MISMATCH] = "Calculated identifier and the one provided in license didn't match";

	LicenseInfo licenseInfo;
	EVENT_TYPE result = acquire_license("example", nullptr, &licenseInfo);

	if (result == LICENSE_OK) {
		//for this example we want to link the license to the execution hardware.
		if (licenseInfo.linked_to_pc) {
			cout
					<< "No client signature in license file, generate license with -s <id>";
			result = IDENTIFIERS_MISMATCH;
		} else {
			cout << "license OK" << endl;
		}
	}
	if (result != LICENSE_OK) {
		cout << "license ERROR :" << endl;
		cout << "    " << stringByEventType[result].c_str() << endl;
		if (result == IDENTIFIERS_MISMATCH) {
			PcSignature signature;
			FUNCTION_RETURN generate_ok = generate_user_pc_signature(signature,
					DEFAULT);
			cout << "the pc signature is :" << endl;
			cout << "    " << signature << endl;
		}
	}

	return result;
}
