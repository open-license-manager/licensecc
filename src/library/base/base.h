#ifndef BASE_H_
#define BASE_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __unix__

#include <limits.h>
#define DllExport
#ifndef MAX_PATH
	#define MAX_PATH PATH_MAX
#endif

#else //windows
#include <windows.h>
#define DllExport  __declspec( dllexport )
#endif

/* #define _DEBUG */
/*
#define cmax(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define cmin(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
*/
typedef enum  {
	OK, NOOK, ERROR, BUFFER_TOO_SMALL
} FUNCTION_RETURN;

#ifdef __cplusplus
}
#endif

#endif
