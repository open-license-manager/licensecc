#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <sys/socket.h>
#include <netpacket/packet.h>

#include <paths.h>

#include <stdlib.h>
#include <cstring>
#include <string>
#include <system_error>
#include <unistd.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "../os-cpp.h"
#include "../../base/public-key.h"

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>

namespace license {

using namespace std;

string OsFunctions::getModuleName() {
	char path[2048] = { 0 };
	string strPath;
	string result;
	pid_t pid = getpid();
	const string pidstr = to_string(((long) pid));
	strPath = string("/proc/") + pidstr + "/exe";
	const char * proc_path = strPath.c_str();
	int ch = readlink(proc_path, path, 2048);
	if (ch != -1) {
		path[ch] = '\0';
		result = string(path);
	} else {
		throw runtime_error(
				string("Can't find: ") + strPath + " proc filesystem mounted?");
	}
	return result;
}



bool OsFunctions::verifySignature(const char* stringToVerify,
		const char* signatureB64) {
	EVP_MD_CTX *mdctx = NULL;

	char *pubKey = PUBLIC_KEY;

	BIO* bio = BIO_new_mem_buf((void*) (pubKey), strlen(pubKey));
	RSA *rsa = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
	BIO_free(bio);
	if (rsa == NULL) {
		throw new logic_error("Error reading public key");
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
		throw new logic_error("Error creating context");
	}
	if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pkey)) {
		throw new logic_error("Error initializing digest");
	}
	int en = strlen(stringToVerify);
	if (1 != EVP_DigestVerifyUpdate(mdctx, stringToVerify, en)) {
		throw new logic_error("Error verifying digest");
	}
	bool result;
	int res = EVP_DigestVerifyFinal(mdctx, buffer, len);
	if (1 == res) {
		result = true;
	} else {
		result = false;
	}
	if (pkey) {
		EVP_PKEY_free(pkey);
	}
	if (mdctx) {
		EVP_MD_CTX_destroy(mdctx);
	}
	return result;
}



VIRTUALIZATION getVirtualization() {
//http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
//
//bool rc = true;
	/*__asm__ (
	 "push   %edx\n"
	 "push   %ecx\n"
	 "push   %ebx\n"
	 "mov    %eax, 'VMXh'\n"
	 "mov    %ebx, 0\n" // any value but not the MAGIC VALUE
	 "mov    %ecx, 10\n"// get VMWare version
	 "mov    %edx, 'VX'\n"// port number
	 "in     %eax, dx\n"// read port on return EAX returns the VERSION
	 "cmp    %ebx, 'VMXh'\n"// is it a reply from VMWare?
	 "setz   [rc] \n"// set return value
	 "pop    %ebx \n"
	 "pop    %ecx \n"
	 "pop    %edx \n"
	 );*/

	return NONE;
}

}
