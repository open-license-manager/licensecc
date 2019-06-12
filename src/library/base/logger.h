#ifndef logger_INCLUDED
#define logger_INCLUDED

#ifndef LOG_DISABLED
#include <errno.h>
#ifdef __cplusplus
extern "C" {
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#ifdef _DEBUG
#define LOG_DEBUG(M, ...) _log("[DEBUG] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(M,...)
#endif
#define LOG_INFO(M, ...) _log("[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(M, ...) _log("[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define LOG_ERROR(M, ...) _log("[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#else
#define LOG_DEBUG(M,...)
#define LOG_INFO(M, ...)
#define LOG_WARN(M, ...)
#define LOG_ERROR(M, ...)
#endif

void _log(const char* format, ...);
void _shutdown_log();

#ifdef __cplusplus
}
#endif
#endif
