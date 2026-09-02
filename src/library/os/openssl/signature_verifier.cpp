/*
 * verifier.cpp
 *
 *  Created on: Nov 16, 2019
 *      Author: GC
 */

#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdlib.h>
#include <errno.h>
#include <vector>

// #ifdef _WIN32
// #include <windows.h>
// #endif

#include <public_key.h>

#include "../signature_verifier.hpp"
#include "../../base/logger.h"

namespace license {
namespace os {

static void free_resources(EVP_PKEY* pkey, EVP_MD_CTX* mdctx) {
	if (pkey) {
		EVP_PKEY_free(pkey);
	}
	if (mdctx) {
		EVP_MD_CTX_destroy(mdctx);
	}
}

static void initialize() {
	static int initialized = 0;
	if (initialized == 0) {
		initialized = 1;
		ERR_load_ERR_strings();
		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
	}
}

FUNCTION_RETURN verify_signature(const std::string& stringToVerify, const std::string& signatureB64) {
	EVP_MD_CTX* mdctx = NULL;
	const unsigned char pubKey[] = PUBLIC_KEY;
	int func_ret = 0;
	initialize();

	BIO* bio = BIO_new_mem_buf((void*)(pubKey), sizeof(pubKey));
	RSA* rsa = d2i_RSAPublicKey_bio(bio, NULL);
	if (rsa == NULL) {
		BIO_free(bio);
		LOG_ERROR("Error reading public key");
		return FUNC_RET_ERROR;
	}
	EVP_PKEY* pkey = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(pkey, rsa);
	BIO_free(bio);

	if (signatureB64.empty()) {
		LOG_ERROR("Empty signature");
		free_resources(pkey, mdctx);
		return FUNC_RET_ERROR;
	}

	// Estimate maximum signature size from base64 length
	// Base64 encoded data is roughly 4/3 of the original size
	// Add 64 bits padding for safety
	size_t estimated_max_size = (signatureB64.length() * 3) / 4 + 64;
	std::vector<unsigned char> buffer(estimated_max_size);

	BIO* b64 = BIO_new(BIO_f_base64());
	BIO* encoded_signature = BIO_new_mem_buf((const void*)signatureB64.c_str(), signatureB64.size());
	BIO* biosig = BIO_push(b64, encoded_signature);
	BIO_set_flags(biosig, BIO_FLAGS_BASE64_NO_NL);	// Do not use newlines to flush buffer
	const int decoded = BIO_read(biosig, (void*)buffer.data(), estimated_max_size);
	// Can test here if len == decodeLen - if not, then return an error
	if (decoded <= 0) {
		BIO_free_all(biosig);
		LOG_ERROR("Error decoding signature");
		free_resources(pkey, mdctx);
		return FUNC_RET_ERROR;
	}
	const size_t len = static_cast<size_t>(decoded);

	BIO_free_all(biosig);

	/* Create the Message Digest Context */
	if (!(mdctx = EVP_MD_CTX_create())) {
		free_resources(pkey, mdctx);
		LOG_ERROR("Error creating context");
		return FUNC_RET_ERROR;
	}
	if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pkey)) {
		free_resources(pkey, mdctx);
		LOG_ERROR("Error initializing digest");
		return FUNC_RET_ERROR;
	}

	func_ret = EVP_DigestVerifyUpdate(mdctx, (const void*)stringToVerify.c_str(), stringToVerify.size());
	if (1 != func_ret) {
		LOG_ERROR("Error verifying digest %d", func_ret);
		free_resources(pkey, mdctx);
		return FUNC_RET_ERROR;
	}
	FUNCTION_RETURN result;
	func_ret = EVP_DigestVerifyFinal(mdctx, buffer.data(), len);
	if (1 != func_ret) {
		LOG_ERROR("Error verifying digest %d", func_ret);
	}
	result = (1 == func_ret ? FUNC_RET_OK : FUNC_RET_ERROR);

	free_resources(pkey, mdctx);
	return result;
}
}  // namespace os
} /* namespace license */
