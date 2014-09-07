#ifndef logger_INCLUDED
#define logger_INCLUDED

#ifndef LOG_ENABLED
#ifdef NDEBUG
#define LOG_DEBUG(M, ...) _log("[INFO] %s (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#define LOG_INFO(M, ...) _log("[INFO] %s (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(M, ...) _log("[WARN] %s (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define LOG_ERROR(M, ...) _log("[ERROR] %s (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#else
#define LOG_DEBUG(M,...)
#define LOG_INFO(M, ...)
#define LOG_WARN(M, ...)
#define LOG_ERROR(M, ...)
#endif

void _log(char* format, ...);
void _shutdown_log();
#endif
