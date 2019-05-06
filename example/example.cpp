#include <iostream>
#include <map>
#include "api/license++.h"
#include "pc-identifiers.h"
#include "ini/SimpleIni.h"

using namespace std;

int main(int argc, char *argv[])
{
    map<EVENT_TYPE, string> stringByEventType;
    stringByEventType[LICENSE_OK                      ] = "OK ";
    stringByEventType[LICENSE_FILE_NOT_FOUND          ] = "license file not found ";
    stringByEventType[LICENSE_SERVER_NOT_FOUND        ] = "license server can't be contacted ";
    stringByEventType[ENVIRONMENT_VARIABLE_NOT_DEFINED] = "environment variable not defined ";
    stringByEventType[FILE_FORMAT_NOT_RECOGNIZED      ] = "license file has invalid format (not .ini file) ";
    stringByEventType[LICENSE_MALFORMED               ] = "some mandatory field are missing, or data can't be fully read. ";
    stringByEventType[PRODUCT_NOT_LICENSED            ] = "this product was not licensed ";
    stringByEventType[PRODUCT_EXPIRED                 ] = "license expired ";
    stringByEventType[LICENSE_CORRUPTED               ] = "license signature didn't match with current license ";
    stringByEventType[IDENTIFIERS_MISMATCH            ] = "Calculated identifier and the one provided in license didn't match";
    stringByEventType[LICENSE_FILE_FOUND              ] = "license file not found ";
    stringByEventType[LICENSE_VERIFIED                ] = "license verified ";
    
    const string licLocation("example.lic");
    
    LicenseInfo licenseInfo;
    LicenseLocation licenseLocation;
    licenseLocation.openFileNearModule = false;
    licenseLocation.licenseFileLocation = licLocation.c_str();
    licenseLocation.environmentVariableName = "";
    EVENT_TYPE result = acquire_license("example", licenseLocation, &licenseInfo);
    PcSignature signature;
    FUNCTION_RETURN generate_ok = generate_user_pc_signature(signature, DEFAULT);

	if (result == LICENSE_OK && licenseInfo.linked_to_pc) {
        CSimpleIniA ini;
    	SI_Error rc = ini.LoadFile(licLocation.c_str());
        string IDinLicense = ini.GetValue("example", "client_signature", "");
		if (IDinLicense == "") {
            cout << "No client signature in license file, generate license with -s <id>";
			result = IDENTIFIERS_MISMATCH;
		} else if (IDinLicense != signature) {
			result = IDENTIFIERS_MISMATCH;
		}
	}

    if (result != LICENSE_OK) {
        cout << "license ERROR :" << endl;
        cout << "    " << stringByEventType[result].c_str() << endl;
        cout << "the pc signature is :" << endl;
        cout << "    " << signature << endl;
    }
    else
        cout << "license OK" << endl;
}
