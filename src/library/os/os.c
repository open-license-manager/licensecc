//#include <stdlib.h>
#include <stdio.h>
#include "../base/logger.h"
#include "os.h"
#include "public-key.h"

#ifndef _MSC_VER

#include <openssl/pem.h>

static void free_resources(EVP_PKEY* pkey, EVP_MD_CTX* mdctx) {
	if (pkey) {
		EVP_PKEY_free(pkey);
	}
	if (mdctx) {
		EVP_MD_CTX_destroy(mdctx);
	}
}

FUNCTION_RETURN verifySignature(const char* stringToVerify,
		const char* signatureB64) {
	EVP_MD_CTX *mdctx = NULL;
	const char *pubKey = PUBLIC_KEY;
	int func_ret = 0;

	BIO* bio = BIO_new_mem_buf((void*) (pubKey), strlen(pubKey));
	RSA *rsa = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
	BIO_free(bio);
	if (rsa == NULL) {
		LOG_ERROR("Error reading public key");
		return FUNC_RET_ERROR;
	}
	EVP_PKEY *pkey = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(pkey, rsa);

	/*BIO* bo = BIO_new(BIO_s_mem());
	 BIO_write(bo, pubKey, strlen(pubKey));
	 RSA *key = 0;
	 PEM_read_bio_RSAPublicKey(bo, &key, 0, 0);
	 BIO_free(bo);*/

//RSA* rsa = EVP_PKEY_get1_RSA( key );
//RSA * pubKey = d2i_RSA_PUBKEY(NULL, <der encoded byte stream pointer>, <num bytes>);
	unsigned char buffer[512];
	BIO* b64 = BIO_new(BIO_f_base64());
	BIO* encoded_signature = BIO_new_mem_buf((void *) signatureB64,
			strlen(signatureB64));
	BIO* biosig = BIO_push(b64, encoded_signature);
	BIO_set_flags(biosig, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
	unsigned int len = BIO_read(biosig, (void *) buffer, strlen(signatureB64));
//Can test here if len == decodeLen - if not, then return an error
	buffer[len] = 0;

	BIO_free_all(biosig);

	/* Create the Message Digest Context */
	if (!(mdctx = EVP_MD_CTX_create())) {
		free_resources(pkey, mdctx);
		LOG_ERROR("Error creating context");
		return FUNC_RET_ERROR;
	}
	if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pkey)) {
		LOG_ERROR("Error initializing digest");
		free_resources(pkey, mdctx);
		return FUNC_RET_ERROR;
	}
	int en = strlen(stringToVerify);
	func_ret = EVP_DigestVerifyUpdate(mdctx, stringToVerify, en);
	if (1 != func_ret) {
		LOG_ERROR("Error verifying digest %d", func_ret);
		free_resources(pkey, mdctx);
		return FUNC_RET_ERROR;
	}
	FUNCTION_RETURN result;
	func_ret = EVP_DigestVerifyFinal(mdctx, buffer, len);
	if (1 != func_ret) {
		LOG_ERROR("Error verifying digest %d", func_ret);
	}
	result = (1 == func_ret ? FUNC_RET_OK : FUNC_RET_ERROR);

	free_resources(pkey, mdctx);
	return result;
}

#else

#include <iphlpapi.h>
#include <Windows.h>
#pragma comment(lib, "IPHLPAPI.lib")

unsigned char* unbase64(const char* ascii, int len, int *flen);

static void printHash(HCRYPTHASH* hHash) {
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
			} LOG_DEBUG("Hash to verify: %s", hashStr);
		}
		free(pbHash);
		free(hashStr);
	}
}

FUNCTION_RETURN verifySignature(const char* stringToVerify,
		const char* signatureB64) {
	//--------------------------------------------------------------------
	// Declare variables.
	//
	// hProv:           Cryptographic service provider (CSP). This example
	//                  uses the Microsoft Enhanced Cryptographic 
	//                  Provider.
	// hKey:            Key to be used. In this example, you import the 
	//                  key as a PLAINTEXTKEYBLOB.
	// dwBlobLen:       Length of the plaintext key.
	// pbKeyBlob:       Pointer to the exported key.
	BYTE pubKey[] = PUBLIC_KEY;

	HCRYPTPROV hProv = 0;
	HCRYPTKEY hKey = 0;
	HCRYPTHASH hHash = 0;
	DWORD dwSigLen;
	BYTE* sigBlob;

	//--------------------------------------------------------------------
	// Acquire a handle to the CSP.

	if (!CryptAcquireContext(&hProv,
	NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		// If the key container cannot be opened, try creating a new
		// container by specifying a container name and setting the 
		// CRYPT_NEWKEYSET flag.
		LOG_INFO("Error in AcquireContext 0x%08x \n", GetLastError());
		if (NTE_BAD_KEYSET == GetLastError()) {
			if (!CryptAcquireContext(&hProv, "license++verify",
					MS_ENHANCED_PROV, PROV_RSA_FULL,
					CRYPT_NEWKEYSET | CRYPT_VERIFYCONTEXT)) {
				LOG_ERROR("Error in AcquireContext 0x%08x \n", GetLastError());
				return FUNC_RET_ERROR;
			}
		} else {
			LOG_ERROR(" Error in AcquireContext 0x%08x \n", GetLastError());
			return FUNC_RET_ERROR;
		}
	}

	// Use the CryptImportKey function to import the PLAINTEXTKEYBLOB
	// BYTE array into the key container. The function returns a 
	// pointer to an HCRYPTKEY variable that contains the handle of
	// the imported key.
	if (!CryptImportKey(hProv, &pubKey[0], sizeof(pubKey), 0, 0, &hKey)) {
		LOG_ERROR("Error 0x%08x in importing the PublicKey \n", GetLastError());
		return FUNC_RET_ERROR;
	}

	if (CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {
		LOG_DEBUG("Hash object created.");
	} else {
		LOG_ERROR("Error in hash creation 0x%08x ", GetLastError());
		CryptReleaseContext(hProv, 0);
		return FUNC_RET_ERROR;
	}

	if (!CryptHashData(hHash, stringToVerify, (DWORD) strlen(stringToVerify), 0)) {
		LOG_ERROR("Error in hashing data 0x%08x ", GetLastError());
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return FUNC_RET_ERROR;
	}
#ifdef _DEBUG
	LOG_DEBUG("Lenght %d, hashed Data: [%s]", strlen(stringToVerify), stringToVerify);
	printHash(&hHash);
#endif
	sigBlob = unbase64(signatureB64, (int) strlen(signatureB64), &dwSigLen);
	LOG_DEBUG("raw signature lenght %d", dwSigLen);
	if (!CryptVerifySignature(hHash, sigBlob, dwSigLen, hKey, NULL, 0)) {
		LOG_ERROR("Signature not validated!  0x%08x ", GetLastError());
		free(sigBlob);
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return FUNC_RET_ERROR;
	}
	CryptDestroyHash(hHash);
	free(sigBlob);
	CryptReleaseContext(hProv, 0);
	return FUNC_RET_OK;
}
#endif
