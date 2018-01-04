#include <memory>
#include "CryptoHelper.h"
#ifndef _MSC_VER
#include"linux/CryptoHelperLinux.h"
#else
#include"win/CryptoHelperWindows.h"
#endif

using namespace std;
namespace license {

unique_ptr<CryptoHelper> CryptoHelper::getInstance() {
#ifndef _MSC_VER
	unique_ptr<CryptoHelper> ptr((CryptoHelper*) new CryptoHelperLinux());
#else
	unique_ptr<CryptoHelper> ptr((CryptoHelper*) new CryptoHelperWindows());
#endif
	return ptr;
}
}
