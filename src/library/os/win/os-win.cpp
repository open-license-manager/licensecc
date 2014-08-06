#include <string>
#include "../os-cpp.h"
#include "../../base/public-key.h"

namespace license {

using namespace std;

string OsFunctions::getModuleName() {
	char lpFilename[MAX_PATH];
	DWORD result = GetModuleFileName(NULL,lpFilename,MAX_PATH);
	return string(lpFilename);
}



bool OsFunctions::verifySignature(const char* stringToVerify,
		const char* signatureB64) {
	return false;
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