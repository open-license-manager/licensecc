/*
 * CryptoHelperWindows.cpp
 *
 *  Created on: Sep 14, 2014
 *
 */

#include "CryptoHelperWindows.h"
#include <sstream> 
#include <vector>
// The RSA public-key key exchange algorithm
#define ENCRYPT_ALGORITHM         CALG_RSA_SIGN
// The high order WORD 0x0200 (decimal 512)
// determines the key length in bits.
#define KEYLENGTH                 0x02000000
#pragma comment(lib, "crypt32.lib")

namespace license {

	CryptoHelperWindows::CryptoHelperWindows() {
		m_hCryptProv = NULL;
		m_hCryptKey = NULL;
		if (CryptAcquireContext(
			&m_hCryptProv,
			"license-manager2++",
			MS_ENHANCED_PROV,
			PROV_RSA_FULL, //CRYPT_NEWKEYSET
			0))	{
		}
		else
		{
			throw exception("Error during CryptAcquireContext");
		}

	}


	/**
	This method calls the CryptGenKey function to get a handle to an

	exportable key-pair. The key-pair is  generated with the RSA public-key
	key exchange algorithm using Microsoft Enhanced Cryptographic Provider.
	*/
	void CryptoHelperWindows::generateKeyPair()
	{
		HRESULT       hr = S_OK;
		DWORD         dwErrCode;
		// If the handle to key container is NULL, fail.
		if (m_hCryptProv == NULL)
			throw logic_error("Cryptocontext not correctly initialized");
		// Release a previously acquired handle to key-pair.
		if (m_hCryptKey)
			m_hCryptKey = NULL;
		// Call the CryptGenKey method to get a handle
		// to a new exportable key-pair.
		if (!CryptGenKey(m_hCryptProv,
			ENCRYPT_ALGORITHM,
			KEYLENGTH | CRYPT_EXPORTABLE,
			&m_hCryptKey))
		{
			dwErrCode = GetLastError();
			throw logic_error(string("Error generating keys ") + to_string(dwErrCode));
		}
	}

	/* This method calls the CryptExportKey function to get the Public key
	 in a string suitable for C source inclusion.
	 */
	const string CryptoHelperWindows::exportPublicKey() const
	{
		HRESULT hr = S_OK;
		DWORD    dwErrCode;
		DWORD dwBlobLen;
		BYTE *pbKeyBlob = NULL;
		stringstream ss;
		// If the handle to key container is NULL, fail.
		if (m_hCryptKey == NULL)
			throw logic_error("call GenerateKey first.");
		// This call here determines the length of the key
		// blob.
		if (!CryptExportKey(
			m_hCryptKey,
			NULL,
			PUBLICKEYBLOB,
			0,
			NULL,
			&dwBlobLen))
		{
			dwErrCode = GetLastError();
			throw logic_error(string("Error calculating size of public key ") + to_string(dwErrCode));
		}
		// Allocate memory for the pbKeyBlob.
		if ((pbKeyBlob = new BYTE[dwBlobLen]) == NULL)
		{
			throw logic_error(string("Out of memory exporting public key "));
		}
		// Do the actual exporting into the key BLOB.
		if (!CryptExportKey(
			m_hCryptKey,
			NULL,
			PUBLICKEYBLOB,
			0,
			pbKeyBlob,
			&dwBlobLen))
		{
			delete pbKeyBlob;
			dwErrCode = GetLastError();
			throw logic_error(string("Error exporting public key ") + to_string(dwErrCode));
		}
		else
		{
			ss << "\t";
			for (unsigned int i = 0; i < dwBlobLen; i++){
				if (i != 0){
					ss << ", ";
					if (i % 10 == 0){
						ss << "\\" << endl << "\t";
					}
				}
				ss << to_string(pbKeyBlob[i]);
			}
			delete pbKeyBlob;
		}
		return ss.str();
	}

	CryptoHelperWindows::~CryptoHelperWindows() {
		if (m_hCryptProv)
		{
			CryptReleaseContext(m_hCryptProv, 0);
			m_hCryptProv = NULL;
		}
		if (m_hCryptKey)
			m_hCryptKey = NULL;
	}

	//--------------------------------------------------------------------
	// This method calls the CryptExportKey function to get the Private key
	// in a byte array. The byte array is allocated on the heap and the size
	// of this is returned to the caller. The caller is responsible for releasing // this memory using a delete call.
	//--------------------------------------------------------------------
	const string CryptoHelperWindows::exportPrivateKey() const
	{
		HRESULT       hr = S_OK;
		DWORD         dwErrCode;
		DWORD dwBlobLen;
		BYTE *pbKeyBlob;
		stringstream ss;
		// If the handle to key container is NULL, fail.
		if (m_hCryptKey == NULL)
			throw logic_error(string("call GenerateKey first."));
		// This call here determines the length of the key
		// blob.
		if (!CryptExportKey(
			m_hCryptKey,
			NULL,
			PRIVATEKEYBLOB,
			0,
			NULL,
			&dwBlobLen))
		{
			dwErrCode = GetLastError();
			throw logic_error(string("Error calculating size of private key ") + to_string(dwErrCode));
		}
		// Allocate memory for the pbKeyBlob.
		if ((pbKeyBlob = new BYTE[dwBlobLen]) == NULL)
		{
			throw logic_error(string("Out of memory exporting private key "));
		}

		// Do the actual exporting into the key BLOB.
		if (!CryptExportKey(
			m_hCryptKey,
			NULL,
			PRIVATEKEYBLOB,
			0,
			pbKeyBlob,
			&dwBlobLen))
		{
			delete pbKeyBlob;
			dwErrCode = GetLastError();
			throw logic_error(string("Error exporting private key ") + to_string(dwErrCode));
		}
		else
		{
			ss << "\t";
			for (unsigned int i = 0; i < dwBlobLen; i++){
				if (i != 0){
					ss << ", ";
					if (i % 15 == 0){
						ss << "\\" << endl << "\t";
					}
				}
				ss << to_string(pbKeyBlob[i]);
			}
			delete pbKeyBlob;
		}
		return ss.str();
	}

	const string CryptoHelperWindows::signString(const void* privateKey, size_t pklen,
		const string& license) const{
		BYTE *pbBuffer = (BYTE *)license.c_str();
		DWORD dwBufferLen = strlen((char *)pbBuffer) + 1;
		HCRYPTHASH hHash;

		HCRYPTKEY hKey;
		BYTE *pbKeyBlob;
		BYTE *pbSignature;
		DWORD dwSigLen;
		DWORD dwBlobLen;
		DWORD strLen;


		//-------------------------------------------------------------------
		// Acquire a cryptographic provider context handle.


		//-------------------------------------------------------------------
		// Get the public at signature key. This is the public key
		// that will be used by the receiver of the hash to verify
		// the signature. In situations where the receiver could obtain the
		// sender's public key from a certificate, this step would not be
		// needed.

		if (CryptGetUserKey(
			m_hCryptProv,
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
			m_hCryptProv,
			CALG_SHA1,
			0,
			0,
			&hHash))
		{
			printf("Hash object created. \n");
		}
		else
		{
			throw logic_error(string("Error during CryptCreateHash."));
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
			throw logic_error(string("Error during CryptHashData."));
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
			throw logic_error(string("Error during CryptSignHash."));
		}
		//-------------------------------------------------------------------
		// Allocate memory for the signature buffer.

		if (pbSignature = (BYTE *)malloc(dwSigLen))
		{
			printf("Memory allocated for the signature.\n");
		}
		else
		{
			throw logic_error(string("Out of memory."));
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
			throw logic_error(string("Error during CryptSignHash."));
		}
		//-------------------------------------------------------------------
		// Destroy the hash object.

		if (hHash)
			CryptDestroyHash(hHash);

		printf("The hash object has been destroyed.\n");
		printf("The signing phase of this program is completed.\n\n");

		CryptBinaryToString(pbSignature,dwSigLen,
			CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &strLen);
		vector<char> buffer(strLen);
		CryptBinaryToString(pbSignature, dwSigLen,
			CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &buffer[0], &strLen);

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

		/*if (hProv)
			CryptReleaseContext(hProv, 0);*/
		return string(&buffer[0]);
	}
} /* namespace license */
