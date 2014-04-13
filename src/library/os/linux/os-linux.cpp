#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <system_error>
#include <unistd.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "../os.hpp"
#include "../../base/public-key.h"
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

namespace license {

using namespace std;

vector<AdapterInfo> OsFunctions::getAdapterInfos() {
	return vector<AdapterInfo>();
}
vector<DiskInfo> OsFunctions::getDiskInfos() {
	struct stat filename_stat, mount_stat;
	static char discard[1024];
	char device[64], name[64], type[64];
	FILE *mounts = fopen(_PATH_MOUNTED, "r");

	while (fscanf(mounts, "%64s %64s %64s %1024[^\n]", device, name, type,
			discard) != EOF) {
		if (stat(device, &mount_stat) != 0)
			continue;
		if (filename_stat.st_dev == mount_stat.st_rdev)
			fprintf(stderr, "device: %s; name: %s; type: %s\n", device, name,
					type);
	}

	return vector<DiskInfo>();
}

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

string OsFunctions::getUserHomePath() {
	return "";
}

bool OsFunctions::verifySignature(const char* stringToVerify,
		const char* signature) {
	EVP_MD_CTX *mdctx = NULL;

	char *pubKey = PUBLIC_KEY
	;

	BIO* bio = BIO_new_mem_buf((void*) (pubKey), strlen(pubKey));
	RSA *rsa = PEM_read_bio_RSAPublicKey(bio, NULL,NULL,NULL);
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
	/* Create the Message Digest Context */
	if (!(mdctx = EVP_MD_CTX_create())) {
		throw new logic_error("Error creating context");
	}
	if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pkey)) {
		throw new logic_error("Error initializing digest");
	}

	if (1
			!= EVP_DigestVerifyUpdate(mdctx, stringToVerify,
					strlen(stringToVerify))) {
		throw new logic_error("Error initializing digest");
	}
	if (1
			== EVP_DigestVerifyFinal(mdctx, (unsigned char *) signature,
					(unsigned long int) strlen(signature))) {
		return true;
	} else {

		return false;
	}
}

VIRTUALIZATION OsFunctions::getVirtualization() {
	//http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
	//
	bool rc = true;
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
