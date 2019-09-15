#include <memory>
#include "CryptoHelper.h"
#ifdef __linux__
#include"linux/CryptoHelperLinux.h"
#elif _WIN32
#include"win/CryptoHelperWindows.h"
#endif

using namespace std;
namespace license {

unique_ptr<CryptoHelper> CryptoHelper::getInstance() {
#ifdef __linux__ 
	unique_ptr<CryptoHelper> ptr((CryptoHelper*) new CryptoHelperLinux());
#elif _WIN32
	unique_ptr<CryptoHelper> ptr((CryptoHelper*) new CryptoHelperWindows());	
#endif
	return ptr;
}
}

