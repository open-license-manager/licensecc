/*
 * LicenseSigner.cpp
 *
 *  Created on: Apr 6, 2014
 *      
 */

#include "../LicenseSigner.h"
#include "../private-key.h"
#include <stdexcept>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <iostream>
#include <cmath>

namespace license {
using namespace std;

LicenseSigner::LicenseSigner() {
	os_initialize();
}

LicenseSigner::LicenseSigner(const std::string& alternatePrimaryKey) {
	os_initialize();
}

string LicenseSigner::Opensslb64Encode(size_t slen, unsigned char* signature) {
	/*
	 FILE*  stream = fmemopen(*buffer, encodedSize+1, "w");
	 */
	//bio = BIO_new_fp(stdout, BIO_NOCLOSE);
	/*int encodedSize = 4 * ceil(slen / 3);
	 char* buffer = (char*) (malloc(encodedSize + 1));
	 memset(buffer,0,encodedSize+1);*/
	BIO* mem_bio = BIO_new(BIO_s_mem());
	BIO* b64 = BIO_new(BIO_f_base64());
	BIO* bio1 = BIO_push(b64, mem_bio);
	BIO_set_flags(bio1, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio1, signature, slen);
	BIO_flush(bio1);
	char* charBuf;
	int sz = BIO_get_mem_data(mem_bio, &charBuf);
	string signatureStr;
	signatureStr.assign(charBuf, sz);
	BIO_free_all(bio1);
	return signatureStr;
}

string LicenseSigner::signString(const string& license) {

	size_t slen;
	unsigned char* signature;
	signature = NULL;
	/* Create the Message Digest Context */
	EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
	if (!mdctx) {
		throw logic_error("Message digest creation context");
	}
	const char *private_key = PRIVATE_KEY
	;
	BIO* bio = BIO_new_mem_buf((void*) (private_key), strlen(private_key));
	EVP_PKEY *pktmp = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
	BIO_free(bio);
	/*Initialise the DigestSign operation - SHA-256 has been selected
	 * as the message digest function in this example */
	if (1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, pktmp)) {
		EVP_MD_CTX_destroy(mdctx);
	}
	/* Call update with the message */
	if (EVP_DigestSignUpdate(mdctx, license.c_str(), license.length()) != 1) {
		EVP_MD_CTX_destroy(mdctx);
		throw logic_error("Message signing exception");
	}
	/* Finalise the DigestSign operation */
	/* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
	 * signature. Length is returned in slen */
	if (EVP_DigestSignFinal(mdctx, NULL, &slen) != 1) {
		EVP_MD_CTX_destroy(mdctx);
		throw logic_error("Message signature finalization exception");
	}
	/* Allocate memory for the signature based on size in slen */
	if (!(signature = (unsigned char *) OPENSSL_malloc(
			sizeof(unsigned char) * slen))) {
		EVP_MD_CTX_destroy(mdctx);
		throw logic_error("Message signature memory allocation exception");
	}
	/* Obtain the signature */
	if (1 != EVP_DigestSignFinal(mdctx, signature, &slen)) {
		OPENSSL_free(signature);
		EVP_MD_CTX_destroy(mdctx);
		throw logic_error("Message signature exception");
	}
	/*
	 FILE*  stream = fmemopen(*buffer, encodedSize+1, "w");
	 */
	//bio = BIO_new_fp(stdout, BIO_NOCLOSE);
	/*int encodedSize = 4 * ceil(slen / 3);
	 char* buffer = (char*) (malloc(encodedSize + 1));
	 memset(buffer,0,encodedSize+1);*/
	string signatureStr = Opensslb64Encode(slen, signature);
	/*
	 * BIO *bio, *b64;
	 char message[] = "Hello World \n";
	 b64 = BIO_new(BIO_f_base64());
	 bio = BIO_new_fp(stdout, BIO_NOCLOSE);
	 bio = BIO_push(b64, bio);
	 BIO_write(bio, message, strlen(message));
	 BIO_flush(bio);
	 BIO_free_all(bio);
	 Read Base64 encoded data from standard input and write the decoded data to standard output:

	 BIO *bio, *b64, *bio_out;
	 char inbuf[512];
	 int inlen;
	 b64 = BIO_new(BIO_f_base64());
	 bio = BIO_new_fp(stdin, BIO_NOCLOSE);
	 bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);
	 bio = BIO_push(b64, bio);
	 while((inlen = BIO_read(bio, inbuf, 512)) > 0)
	 BIO_write(bio_out, inbuf, inlen);
	 BIO_free_all(bio);
	 */
	/* Clean up */
	//free(buffer);
	if (pktmp)
		EVP_PKEY_free(pktmp);
	if (signature)
		OPENSSL_free(signature);

	if (mdctx)
		EVP_MD_CTX_destroy(mdctx);
	return signatureStr;
}

void LicenseSigner::signLicense(FullLicenseInfo& licenseInfo) {
	string license = licenseInfo.printForSign();
	string signature = signString(license);
	licenseInfo.license_signature = signature;
}

LicenseSigner::~LicenseSigner() {

}

} /* namespace license */
