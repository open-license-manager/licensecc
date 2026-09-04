/*
 * verifier.cpp
 *
 *  Created on: Nov 16, 2019
 *      Author: devel
 */

#include "../os.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <bcrypt.h>
#include <wincrypt.h>
#include <iphlpapi.h>
#include <windows.h>
// #pragma comment(lib, "bcrypt.lib")

#include <public_key.h>
#include "../../base/logger.h"
#include "../../base/base64.h"
#include "../signature_verifier.hpp"
#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>

namespace license {
namespace os {
using namespace std;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

static const void formatError(DWORD status, const char* description) {
	char msgBuffer[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &msgBuffer[0],
				  sizeof(msgBuffer) - 1, nullptr);
	LOG_DEBUG("error %s : %s %h", description, msgBuffer, status);
}

static FUNCTION_RETURN openHashProvider(BCRYPT_ALG_HANDLE& hash_alg) {
	DWORD status;
	hash_alg = nullptr;
	if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(&hash_alg, BCRYPT_SHA256_ALGORITHM, NULL, 0))) {
		return FUNC_RET_NOT_AVAIL;
	}
	return FUNC_RET_OK;
}

static DWORD hashData(BCRYPT_HASH_HANDLE& hHash, const string& data, PBYTE pbHash, DWORD hashDataLenght) {
	DWORD status;
	if (NT_SUCCESS(status = BCryptHashData(hHash, (BYTE*)data.c_str(), (ULONG)data.length(), 0))) {
		status = BCryptFinishHash(hHash, pbHash, hashDataLenght, 0);
	}
	return status;
}

/**
 * Reads the length field of an ASN.1 encoded element
 * @param ptr pointer to the length field in the ASN.1 data
 * @return the decoded length value
 */
static size_t read_length(uint8_t*& ptr) {
	uint8_t len = *ptr++;
	size_t result = 0;
	if ((len & 0x80) > 0) {
		size_t blen = len & 0x7F;
		for (size_t i = 0; i < blen; i++) {
			result += (*(ptr++) << (i * 8));
		}
	} else {
		result = len;
	}
	return result;
}

/**
 * Reads an ASN.1 SEQUENCE header and returns its length
 * @param ptr pointer to the SEQUENCE tag in the ASN.1 data
 * @param seq_len reference to store the sequence length
 * @return FUNCTION_RETURN indicating success or failure
 */
static FUNCTION_RETURN read_sequence(uint8_t*& ptr, size_t& seq_len) {
	uint8_t tag = *ptr++;
	if (tag != 0x30) {
		return FUNC_RET_ERROR;
	}
	seq_len = read_length(ptr);
	return FUNC_RET_OK;
}

/**
 * Reads an ASN.1 INTEGER value from the data stream
 * @param ptr pointer to the INTEGER tag in the ASN.1 data
 * @param location buffer to store the integer value
 * @param buffer_length maximum size of the location buffer
 * @param actual_length reference to store the actual length of the integer read
 * @return FUNCTION_RETURN indicating success or failure
 *
 * Note: The INTEGER value is stored in big-endian format in the location buffer.
 * If the integer has the high bit set, a leading zero byte may be present to
 * ensure it's interpreted as positive (ASN.1 INTEGER is signed).
 */
static FUNCTION_RETURN read_integer(uint8_t*& ptr, BYTE* buffer, const size_t buffer_length, size_t& actual_length) {
	uint8_t tag = *ptr++;
	if (tag != 0x02) {
		return FUNC_RET_ERROR;
	}
	size_t length = read_length(ptr);
	// skip the padding byte if present
	if (*ptr == 0) {
		length--;
		ptr++;
	}
	actual_length = length;
	if (buffer_length < length || buffer == nullptr) {
		ptr += length;
		return FUNC_RET_BUFFER_TOO_SMALL;
	}
	for (size_t i = 0; i < length; i++) {
		buffer[i] = *(ptr++);
	}
	return FUNC_RET_OK;
}

/*
 * ASN.1 structure of a PKCS#1 encoded RSA public key:
 *
 * RSAPublicKey ::= SEQUENCE {
 *     modulus           INTEGER,    -- n
 *     publicExponent    INTEGER     -- e
 * }
 *
 * The structure starts with a SEQUENCE tag (0x30), followed by the length,
 * then two INTEGER values: the modulus and the public exponent.
 *
 * The target structure in the windows API to hold the public key format has the following structure:
 *
 * BCRYPT_RSAKEY_BLOB
 * PublicExponent[cbPublicExp] // Big-endian.
 * Modulus[cbModulus] // Big-endian.
 */
static FUNCTION_RETURN readPublicKey(const BCRYPT_ALG_HANDLE sig_alg, BCRYPT_KEY_HANDLE* hKey) {
	FUNCTION_RETURN result = FUNC_RET_ERROR;
	DWORD status;

	// First pass: determine the size of the modulus by parsing the public key
	uint8_t pubKey[] = PUBLIC_KEY;
	uint8_t* pub_key_idx = &pubKey[0];

	size_t seq_len = 0;
	if (read_sequence(pub_key_idx, seq_len) != FUNC_RET_OK) {
		return FUNC_RET_ERROR;
	}

	uint8_t* modulus_idx = pub_key_idx;	 // remembers the modulus position.
	size_t mod_size = 0;  // read the modulus size
	if (read_integer(pub_key_idx, nullptr, 0, mod_size) != FUNC_RET_BUFFER_TOO_SMALL) {
		return FUNC_RET_ERROR;
	}

	size_t exp_size = 0;  // read the exponent size
	uint8_t* exponent_idx = pub_key_idx;  // remembers the exponent position
	if (read_integer(pub_key_idx, nullptr, 0, exp_size) != FUNC_RET_BUFFER_TOO_SMALL) {
		return FUNC_RET_ERROR;
	}
	if (exp_size != 3) {
		LOG_DEBUG("Error reading public key exponent size is not 3 bytes");
		return FUNC_RET_ERROR;
	}
	// Calculate the key bit length
	size_t key_bitlen = mod_size * 8;
	// Now allocate memory for the key blob with the correct size
	size_t total_blob_size =
		sizeof(BCRYPT_RSAKEY_BLOB) + exp_size + mod_size;  // 3 for exponent, modulus_size for modulus
	vector<BYTE> blob_buffer(total_blob_size);

	// Set up the key blob
	BCRYPT_RSAKEY_BLOB* pubk_header = (BCRYPT_RSAKEY_BLOB*)blob_buffer.data();
	pubk_header->Magic = BCRYPT_RSAPUBLIC_MAGIC;
	pubk_header->BitLength = (ULONG)key_bitlen;
	pubk_header->cbPublicExp = (ULONG)exp_size;
	pubk_header->cbModulus = (ULONG)mod_size;
	pubk_header->cbPrime1 = 0;
	pubk_header->cbPrime2 = 0;

	// Get pointers to the exponent and modulus areas
	BYTE* blob_exp_ptr = blob_buffer.data() + sizeof(BCRYPT_RSAKEY_BLOB);
	BYTE* blob_modulus_ptr = blob_exp_ptr + exp_size;
	// read the modulus into the blob
	if (read_integer(modulus_idx, blob_modulus_ptr, mod_size, mod_size) != FUNC_RET_OK) {
		return FUNC_RET_ERROR;
	}

	if (read_integer(exponent_idx, blob_exp_ptr, exp_size, exp_size) != FUNC_RET_OK) {
		return FUNC_RET_ERROR;
	}
	/*
	auto first = blob_buffer.begin() + sizeof(BCRYPT_RSAKEY_BLOB);
	auto last = first + mod_size;  // +1 to include index 'end'

	std::cout << "Hex [5..10]: ";
	for (auto it = first; it != last; ++it) {
		std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(*it) << ":";
	}*/
	if (NT_SUCCESS(status = BCryptImportKeyPair(sig_alg, nullptr, BCRYPT_RSAPUBLIC_BLOB, hKey,
												(PUCHAR)blob_buffer.data(), total_blob_size, 0))) {
		result = FUNC_RET_OK;
	} else {
#ifndef NDEBUG
		formatError(status, "error importing public key");
#endif
	}

	// free(blob_buffer);
	return result;
}

static FUNCTION_RETURN verifyHash(const PBYTE pbHash, const DWORD hashDataLenght, const string& signatureBuffer) {
	BCRYPT_KEY_HANDLE phKey = nullptr;
	DWORD status;
	FUNCTION_RETURN result = FUNC_RET_ERROR;
	PBYTE pbSignature = nullptr;
	BCRYPT_ALG_HANDLE hSignAlg = nullptr;

	vector<uint8_t> signatureBlob = unbase64(signatureBuffer);
	DWORD dwSigLen = (DWORD)signatureBlob.size();
	BYTE* sigBlob = &signatureBlob[0];

	if (NT_SUCCESS(status = BCryptOpenAlgorithmProvider(&hSignAlg, BCRYPT_RSA_ALGORITHM, NULL, 0))) {
		if ((result = readPublicKey(hSignAlg, &phKey)) == FUNC_RET_OK) {
			BCRYPT_PKCS1_PADDING_INFO paddingInfo;
			ZeroMemory(&paddingInfo, sizeof(paddingInfo));
			paddingInfo.pszAlgId = BCRYPT_SHA256_ALGORITHM;
			if (NT_SUCCESS(status = BCryptVerifySignature(phKey, &paddingInfo, pbHash, hashDataLenght, sigBlob,
														  dwSigLen, BCRYPT_PAD_PKCS1))) {
				result = FUNC_RET_OK;
			} else {
				result = FUNC_RET_ERROR;
#ifndef NDEBUG
				formatError(status, "error verifying signature");
#endif
			}
		} else {
			LOG_DEBUG("Error reading public key");
		}
	} else {
		result = FUNC_RET_NOT_AVAIL;
#ifndef NDEBUG
		formatError(status, "error opening RSA provider");
#endif
	}

	if (phKey != nullptr) {
		BCryptDestroyKey(phKey);
	}
	if (hSignAlg != nullptr) {
		BCryptCloseAlgorithmProvider(hSignAlg, 0);
	}
	// if (sigBlob) {
	//	free(sigBlob);
	// }
	return result;
}

FUNCTION_RETURN verify_signature(const std::string& stringToVerify, const std::string& signatureB64) {
	BCRYPT_HASH_HANDLE hHash = nullptr;
	PBYTE pbHashObject = nullptr, pbHashData = nullptr;
	DWORD status;

	const HANDLE hProcessHeap = GetProcessHeap();
	// BCRYPT_ALG_HANDLE sig_alg = openSignatureProvider();

	BCRYPT_ALG_HANDLE hash_alg = nullptr;
	FUNCTION_RETURN result = openHashProvider(hash_alg);
	if (result != FUNC_RET_OK) {
		return result;
	}

	// calculate the size of the buffer to hold the hash object
	DWORD cbData = 0, cbHashObject = 0;
	// and the size to keep the hashed data
	DWORD cbHashDataLenght = 0;
	if (NT_SUCCESS(status = BCryptGetProperty(hash_alg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD),
											  &cbData, 0)) &&
		NT_SUCCESS(status = BCryptGetProperty(hash_alg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHashDataLenght, sizeof(DWORD),
											  &cbData, 0))) {
		// allocate the hash object on the heap
		pbHashObject = (PBYTE)HeapAlloc(hProcessHeap, 0, cbHashObject);
		pbHashData = (PBYTE)HeapAlloc(hProcessHeap, 0, cbHashDataLenght);
		if (NULL != pbHashObject && nullptr != pbHashData) {
			if (NT_SUCCESS(status = BCryptCreateHash(hash_alg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0))) {
				if (NT_SUCCESS(status = hashData(hHash, stringToVerify, pbHashData, cbHashDataLenght))) {
					result = verifyHash(pbHashData, cbHashDataLenght, signatureB64);
				} else {
					result = FUNC_RET_NOT_AVAIL;
#ifndef NDEBUG
					formatError(status, "error hashing data");
#endif
				}
			} else {
				result = FUNC_RET_NOT_AVAIL;
#ifndef NDEBUG
				formatError(status, "error creating hash");
#endif
			}
		} else {
			result = FUNC_RET_BUFFER_TOO_SMALL;
			LOG_DEBUG("Error allocating memory");
		}
	} else {
		result = FUNC_RET_NOT_AVAIL;
#ifndef NDEBUG
		formatError(status, "**** Error returned by BCryptGetProperty");
#endif
	}

	if (hHash) {
		BCryptDestroyHash(hHash);
	}
	if (pbHashObject) {
		HeapFree(hProcessHeap, 0, pbHashObject);
	}
	if (pbHashData) {
		HeapFree(hProcessHeap, 0, pbHashData);
	}
	if (hash_alg != nullptr) {
		BCryptCloseAlgorithmProvider(hash_alg, 0);
	}
	return result;
}
}  // namespace os
} /* namespace license */
