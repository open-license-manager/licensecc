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
	if (!CryptAcquireContext(&m_hCryptProv, "license++sign", MS_ENHANCED_PROV,
			PROV_RSA_FULL, 0)) {
		// If the key container cannot be opened, try creating a new
		// container by specifying a container name and setting the
		// CRYPT_NEWKEYSET flag.
		printf("Error in AcquireContext 0x%08x \n", GetLastError());
		if (NTE_BAD_KEYSET == GetLastError()) {
			if (!CryptAcquireContext(&m_hCryptProv, "license++sign",
					MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
				printf("Error in AcquireContext 0x%08x \n", GetLastError());
				throw logic_error("");
			}
		} else {
			printf(" Error in AcquireContext 0x%08x \n", GetLastError());
			throw logic_error("");
		}
	}

}

/**
 This method calls the CryptGenKey function to get a handle to an

 exportable key-pair. The key-pair is  generated with the RSA public-key
 key exchange algorithm using Microsoft Enhanced Cryptographic Provider.
 */
void CryptoHelperWindows::generateKeyPair() {
	HRESULT hr = S_OK;
	DWORD dwErrCode;
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
	KEYLENGTH | CRYPT_EXPORTABLE, &m_hCryptKey)) {
		dwErrCode = GetLastError();
		throw logic_error(
				string("Error generating keys ") + to_string(static_cast<long long>(dwErrCode)));
	}
}

/* This method calls the CryptExportKey function to get the Public key
 in a string suitable for C source inclusion.
 */
const string CryptoHelperWindows::exportPublicKey() const {
	HRESULT hr = S_OK;
	DWORD dwErrCode;
	DWORD dwBlobLen;
	BYTE *pbKeyBlob = nullptr;
	stringstream ss;
	// If the handle to key container is NULL, fail.
	if (m_hCryptKey == NULL)
		throw logic_error("call GenerateKey first.");
	// This call here determines the length of the key
	// blob.
	if (!CryptExportKey(m_hCryptKey,
	NULL, PUBLICKEYBLOB, 0,
	nullptr, &dwBlobLen)) {
		dwErrCode = GetLastError();
		throw logic_error(
				string("Error calculating size of public key ")
						+ to_string(static_cast<long long>(dwErrCode)));
	}
	// Allocate memory for the pbKeyBlob.
	if ((pbKeyBlob = new BYTE[dwBlobLen]) == nullptr) {
		throw logic_error(string("Out of memory exporting public key "));
	}
	// Do the actual exporting into the key BLOB.
	if (!CryptExportKey(m_hCryptKey,
	NULL, PUBLICKEYBLOB, 0, pbKeyBlob, &dwBlobLen)) {
		delete pbKeyBlob;
		dwErrCode = GetLastError();
		throw logic_error(
				string("Error exporting public key ") + to_string(static_cast<long long>(dwErrCode)));
	} else {
		ss << "\t";
		for (unsigned int i = 0; i < dwBlobLen; i++) {
			if (i != 0) {
				ss << ", ";
				if (i % 10 == 0) {
					ss << "\\" << endl << "\t";
				}
			}
			ss << to_string(static_cast<long long>(pbKeyBlob[i]));
		}
		delete pbKeyBlob;
	}
	return ss.str();
}

CryptoHelperWindows::~CryptoHelperWindows() {
	if (m_hCryptProv) {
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
const string CryptoHelperWindows::exportPrivateKey() const {
	HRESULT hr = S_OK;
	DWORD dwErrCode;
	DWORD dwBlobLen;
	BYTE *pbKeyBlob;
	stringstream ss;
	// If the handle to key container is NULL, fail.
	if (m_hCryptKey == NULL)
		throw logic_error(string("call GenerateKey first."));
	// This call here determines the length of the key
	// blob.
	if (!CryptExportKey(m_hCryptKey,
	NULL, PRIVATEKEYBLOB, 0,
	nullptr, &dwBlobLen)) {
		dwErrCode = GetLastError();
		throw logic_error(
				string("Error calculating size of private key ")
						+ to_string(static_cast<long long>(dwErrCode)));
	}
	// Allocate memory for the pbKeyBlob.
	if ((pbKeyBlob = new BYTE[dwBlobLen]) == nullptr) {
		throw logic_error(string("Out of memory exporting private key "));
	}

	// Do the actual exporting into the key BLOB.
	if (!CryptExportKey(m_hCryptKey,
	NULL, PRIVATEKEYBLOB, 0, pbKeyBlob, &dwBlobLen)) {
		delete pbKeyBlob;
		dwErrCode = GetLastError();
		throw logic_error(
				string("Error exporting private key ") + to_string(static_cast<long long>(dwErrCode)));
	} else {
		ss << "\t";
		for (unsigned int i = 0; i < dwBlobLen; i++) {
			if (i != 0) {
				ss << ", ";
				if (i % 15 == 0) {
					ss << "\\" << endl << "\t";
				}
			}
			ss << to_string(static_cast<long long>(pbKeyBlob[i]));
		}
		delete pbKeyBlob;
	}
	return ss.str();
}

void CryptoHelperWindows::printHash(HCRYPTHASH* hHash) const {
	BYTE *pbHash;
	DWORD dwHashLen;
	DWORD dwHashLenSize = sizeof(DWORD);
	char* hashStr;
	unsigned int i;

	if (CryptGetHashParam(*hHash, HP_HASHSIZE, (BYTE *) &dwHashLen,
			&dwHashLenSize, 0)) {
		pbHash = (BYTE*) malloc(dwHashLen);
		hashStr = (char*) malloc(dwHashLen * 2 + 1);
		if (CryptGetHashParam(*hHash, HP_HASHVAL, pbHash, &dwHashLen, 0)) {
			for (i = 0; i < dwHashLen; i++) {
				sprintf(&hashStr[i * 2], "%02x", pbHash[i]);
			}
			printf("hash To be signed: %s \n", hashStr);
		}
		free(pbHash);
		free(hashStr);
	}
}

const string CryptoHelperWindows::signString(const void* privateKey,
		size_t pklen, const string& license) const {
	BYTE *pbBuffer = (BYTE *) license.c_str();
	const DWORD dwBufferLen = (DWORD)strlen((char *)pbBuffer);
	HCRYPTHASH hHash;

	HCRYPTKEY hKey;
	BYTE *pbSignature;
	DWORD dwSigLen;
	DWORD strLen;

	//-------------------------------------------------------------------
	// Acquire a cryptographic provider context handle.

	if (!CryptImportKey(m_hCryptProv, (const BYTE *) privateKey, (DWORD) pklen, 0, 0,
			&hKey)) {
		throw logic_error(
				string("Error in importing the PrivateKey ")
						+ to_string(static_cast<long long>(GetLastError())));
	}

	//-------------------------------------------------------------------
	// Create the hash object.

	if (CryptCreateHash(m_hCryptProv, CALG_SHA1, 0, 0, &hHash)) {
		printf("Hash object created. \n");
	} else {
		CryptDestroyKey(hKey);
		throw logic_error(string("Error during CryptCreateHash."));
	}
	//-------------------------------------------------------------------
	// Compute the cryptographic hash of the buffer.

	if (CryptHashData(hHash, pbBuffer, dwBufferLen, 0)) {
#ifdef _DEBUG
		printf("Length of data to be hashed: %d \n", dwBufferLen);
		printHash(&hHash);
#endif 
	} else {
		throw logic_error(string("Error during CryptHashData."));
	}
	//-------------------------------------------------------------------
	// Determine the size of the signature and allocate memory.

	dwSigLen = 0;
	if (CryptSignHash(hHash, AT_SIGNATURE, nullptr, 0, nullptr, &dwSigLen)) {
		printf("Signature length %d found.\n", dwSigLen);
	} else {
		throw logic_error(string("Error during CryptSignHash."));
	}
	//-------------------------------------------------------------------
	// Allocate memory for the signature buffer.

	if (pbSignature = (BYTE *) malloc(dwSigLen)) {
		printf("Memory allocated for the signature.\n");
	} else {
		throw logic_error(string("Out of memory."));
	}
	//-------------------------------------------------------------------
	// Sign the hash object.

	if (CryptSignHash(hHash, AT_SIGNATURE,
	nullptr, 0, pbSignature, &dwSigLen)) {
		printf("pbSignature is the signature length. %d\n", dwSigLen);
	} else {
		throw logic_error(string("Error during CryptSignHash."));
	}
	//-------------------------------------------------------------------
	// Destroy the hash object.

	CryptDestroyHash(hHash);
	CryptDestroyKey(hKey);

	CryptBinaryToString(pbSignature, dwSigLen,
			CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &strLen);
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



	 //-------------------------------------------------------------------
	 // Destroy the hash object.



	 //-------------------------------------------------------------------
	 // Release the provider handle.

	 /*if (hProv)
	 CryptReleaseContext(hProv, 0);*/
	if (pbSignature) {
		free(pbSignature);
	}
	return string(&buffer[0]);
}
} /* namespace license */
