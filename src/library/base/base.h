#ifndef BASE_H_
#define BASE_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __unix__
#include <limits.h>
#define DllExport
#define MAX_PATH PATH_MAX
#else
#include <windows.h>
#define DllExport  __declspec( dllexport )
#endif

#define _DEBUG

typedef enum  {
	OK, NOOK, ERROR, BUFFER_TOO_SMALL
} FUNCTION_RETURN;

#ifdef __cplusplus
}
#endif

#endif
