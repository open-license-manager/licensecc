#include <memory>
#include "CryptoHelper.h"
#ifdef __unix__
#include"linux/CryptoHelperLinux.h"
#else
#include"win/CryptoHelperWindows.h"
#endif

using namespace std;
namespace license {

unique_ptr<CryptoHelper> CryptoHelper::getInstance() {
#ifdef __unix__
	unique_ptr<CryptoHelper> ptr((CryptoHelper*) new CryptoHelperLinux());
#else
	unique_ptr<CryptoHelper> ptr((CryptoHelper*) new CryptpHelperWindows());
#endif
	return ptr;
}
}
