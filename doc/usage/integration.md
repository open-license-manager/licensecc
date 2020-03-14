# Integrate license in your application

This short guide explain how to integrate `open-license-manager` in your application.

Working examples are provided in the [examples](https://github.com/open-license-manager/examples) project. 

## Public api
The file containing the public api is `include/licensecc/licensecc.h`. Functions in there are considered stable.


### Print a hardware identifier

```CPP
bool identify_pc(IDENTIFICATION_STRATEGY pc_id_method, char* identifier_out, size_t bufSize);
```

### Verify a license

```CPP
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

EVENT_TYPE acquire_license(const CallerInformations* callerInformation, const LicenseLocation* licenseLocation,
						   LicenseInfo* license_out);
```