/*
 * LicenseSigner.cpp (Windows)
 *
 *  Created on: Apr 6, 2014
 *      
 */

#include <stdexcept>
#include <string.h>
#include <iostream>
#include <cmath>

#pragma comment(lib, "crypt32.lib")

#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#include "../private-key.h"

#include "../LicenseSigner.h"
#include "../../library/base/logger.h"
namespace license {
	using namespace std;

	LicenseSigner::LicenseSigner() {
		os_initialize();

		if (CryptAcquireContext(
			&hProv,
			"license-manager2++",
			MS_ENHANCED_PROV,
			PROV_RSA_FULL, //CRYPT_NEWKEYSET
			0))	{
			LOG_DEBUG("CSP context acquired.");
		}
		else
		{
			LOG_ERROR("Error during CryptAcquireContextc %d.",GetLastError());
			throw exception();
		}
		if (CryptImportKey(
			hProv,
			PRIVATE_KEY,
			sizeof(PRIVATE_KEY),
			0,
			0,
			&hPubKey))
		{
			LOG_DEBUG("The key has been imported.\n");
		}
		else
		{
			LOG_ERROR("Private key import failed.\n");
			throw exception();
		}
		
	}

	LicenseSigner::LicenseSigner(const std::string& alternatePrimaryKey) {
		os_initialize();
	}

	string LicenseSigner::Opensslb64Encode(size_t slen, unsigned char* signature) {

		return NULL;
	}

	string LicenseSigner::signString(const string& license) {

		//-------------------------------------------------------------------
		// Declare and initialize variables.
		BYTE *pbBuffer = (BYTE *)license.c_str();
		DWORD dwBufferLen = strlen((char *)pbBuffer) + 1;
		HCRYPTHASH hHash;

		HCRYPTKEY hKey;
		BYTE *pbKeyBlob;
		BYTE *pbSignature;
		DWORD dwSigLen;
		DWORD dwBlobLen;

		//-------------------------------------------------------------------
		// Acquire a cryptographic provider context handle.


		//-------------------------------------------------------------------
		// Get the public at signature key. This is the public key
		// that will be used by the receiver of the hash to verify
		// the signature. In situations where the receiver could obtain the
		// sender's public key from a certificate, this step would not be
		// needed.

		if (CryptGetUserKey(
			hProv,
			AT_SIGNATURE,
			&hKey))
		{
			printf("The signature key has been acquired. \n");
		}
		else
		{
			printf("Error during CryptGetUserKey for signkey. %d", GetLastError());
		}

		//-------------------------------------------------------------------
		// Create the hash object.

		if (CryptCreateHash(
			hProv,
			CALG_SHA1,
			0,
			0,
			&hHash))
		{
			printf("Hash object created. \n");
		}
		else
		{
			LOG_ERROR("Error during CryptCreateHash.");
		}
		//-------------------------------------------------------------------
		// Compute the cryptographic hash of the buffer.

		if (CryptHashData(
			hHash,
			pbBuffer,
			dwBufferLen,
			0))
		{
			printf("The data buffer has been hashed.\n");
		}
		else
		{
			LOG_ERROR("Error during CryptHashData.");
		}
		//-------------------------------------------------------------------
		// Determine the size of the signature and allocate memory.

		dwSigLen = 0;
		if (CryptSignHash(
			hHash,
			AT_SIGNATURE,
			NULL,
			0,
			NULL,
			&dwSigLen))
		{
			printf("Signature length %d found.\n", dwSigLen);
		}
		else
		{
			LOG_ERROR("Error during CryptSignHash.");
		}
		//-------------------------------------------------------------------
		// Allocate memory for the signature buffer.

		if (pbSignature = (BYTE *)malloc(dwSigLen))
		{
			printf("Memory allocated for the signature.\n");
		}
		else
		{
			LOG_ERROR("Out of memory.");
		}
		//-------------------------------------------------------------------
		// Sign the hash object.

		if (CryptSignHash(
			hHash,
			AT_SIGNATURE,
			NULL,
			0,
			pbSignature,
			&dwSigLen))
		{
			printf("pbSignature is the hash signature.\n");
		}
		else
		{
			LOG_ERROR("Error during CryptSignHash.");
		}
		//-------------------------------------------------------------------
		// Destroy the hash object.

		if (hHash)
			CryptDestroyHash(hHash);

		printf("The hash object has been destroyed.\n");
		printf("The signing phase of this program is completed.\n\n");

		//-------------------------------------------------------------------
		// In the second phase, the hash signature is verified.
		// This would most often be done by a different user in a
		// separate program. The hash, signature, and the PUBLICKEYBLOB
		// would be read from a file, an email message, 
		// or some other source.

		// Here, the original pbBuffer, pbSignature, szDescription. 
		// pbKeyBlob, and their lengths are used.

		// The contents of the pbBuffer must be the same data 
		// that was originally signed.

		//-------------------------------------------------------------------
		// Get the public key of the user who created the digital signature 
		// and import it into the CSP by using CryptImportKey. This returns
		// a handle to the public key in hPubKey.

		/*if (CryptImportKey(
			hProv,
			pbKeyBlob,
			dwBlobLen,
			0,
			0,
			&hPubKey))
		{
			printf("The key has been imported.\n");
		}
		else
		{
			MyHandleError("Public key import failed.");
		}
		//-------------------------------------------------------------------
		// Create a new hash object.

		if (CryptCreateHash(
			hProv,
			CALG_MD5,
			0,
			0,
			&hHash))
		{
			printf("The hash object has been recreated. \n");
		}
		else
		{
			MyHandleError("Error during CryptCreateHash.");
		}
		//-------------------------------------------------------------------
		// Compute the cryptographic hash of the buffer.

		if (CryptHashData(
			hHash,
			pbBuffer,
			dwBufferLen,
			0))
		{
			printf("The new hash has been created.\n");
		}
		else
		{
			MyHandleError("Error during CryptHashData.");
		}
		//-------------------------------------------------------------------
		// Validate the digital signature.

		if (CryptVerifySignature(
			hHash,
			pbSignature,
			dwSigLen,
			hPubKey,
			NULL,
			0))
		{
			printf("The signature has been verified.\n");
		}
		else
		{
			printf("Signature not validated!\n");
		}
		//-------------------------------------------------------------------
		// Free memory to be used to store signature.

		if (pbSignature)
			free(pbSignature);

		//-------------------------------------------------------------------
		// Destroy the hash object.

		if (hHash)
			CryptDestroyHash(hHash);*/

		//-------------------------------------------------------------------
		// Release the provider handle.

		if (hProv)
			CryptReleaseContext(hProv, 0);
		return string("");
	} //  End of main





	void LicenseSigner::signLicense(FullLicenseInfo& licenseInfo) {
		string license = licenseInfo.printForSign();
		string signature = signString(license);
		licenseInfo.license_signature = signature;
	}

	LicenseSigner::~LicenseSigner() {

	}

} /* namespace license */
