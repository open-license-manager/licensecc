#include <iostream>
#include <map>
#include "api/license++.h"
#include "pc-identifiers.h"

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
    stringByEventType[PRODUCT_EXPIRED                 ] = "licence expired ";
    stringByEventType[LICENSE_CORRUPTED               ] = "license signature didn't match with current license ";
    stringByEventType[IDENTIFIERS_MISMATCH            ] = "Calculated identifier and the one provided in license didn't match";
    stringByEventType[LICENSE_FILE_FOUND              ] = "licence file not found ";
    stringByEventType[LICENSE_VERIFIED                ] = "licence verified ";
    
    const string licLocation("example.lic");
    
    LicenseInfo licenseInfo;
    LicenseLocation licenseLocation;
    licenseLocation.openFileNearModule = false;
    licenseLocation.licenseFileLocation = licLocation.c_str();
    licenseLocation.environmentVariableName = "";
    EVENT_TYPE result = acquire_license("example", licenseLocation, &licenseInfo);
    
    if (result != LICENSE_OK){
        PcSignature signature;
        FUNCTION_RETURN generate_ok = generate_user_pc_signature(signature, ETHERNET);
        
        cout << "license ERROR :" << endl;
        cout << "    " << stringByEventType[result].c_str() << endl;
        cout << "the pc signature is :" << endl;
        cout << "    " << signature << endl;
    }
    else
        cout << "licence OK" << endl;
}
