#ifndef BASE_H_
#define BASE_H_

#ifdef __unix__
#include <limits.h>
#define DllExport
#ifndef MAX_PATH
	#define MAX_PATH PATH_MAX
#endif //ndef(MAX_PATH)
#else //windows
#include <windows.h>
#define DllExport  __declspec( dllexport )
#endif //windows

enum FUNCTION_RETURN {
	FUNC_RET_OK,
    FUNC_RET_NOT_AVAIL,
    FUNC_RET_ERROR,
    FUNC_RET_BUFFER_TOO_SMALL
};

#endif
