
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <locale>
#include <codecvt>
#include <string>
#ifdef __unix__
#include <unistd.h>
#define MAX_PATH 255
#else //windows
#include <windows.h>
#include <process.h>
#define getpid _getpid
#endif //windows
#include "metalicensor/base/logger.h"

static FILE *logFile = NULL;

static void timenow(char * buffer) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", timeinfo);
}

static std::string getLogFileName() {
#ifdef __unix__
    char const* folder = getenv("TMPDIR");
    if(folder==0)
        folder = "/tmp";
    return std::string(folder)+"/open-license.log";
#else
    wchar_t acBuffer[MAX_PATH];
    int plen = GetTempPath(MAX_PATH,acBuffer);
    if(plen==0) {
        fprintf(stderr,"Error getting temporary directory path");
        return std::string("open-license.log");
    }
    const std::string sPath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(acBuffer);
    return sPath+"/open-license.log";
#endif
}

void _log(const char* format, ...) {
    va_list args;
    char * buffer;
    if (logFile == NULL) {
        logFile = fopen(getLogFileName().c_str(), "a");
        if (logFile == NULL) {
            //what shall we do here?
            return;
        }
    }
    buffer = (char *) malloc(sizeof(char) * strlen(format) + 64);
    timenow(buffer);
    sprintf(&buffer[strlen(buffer) - 1], "-[%d]-", getpid());
    strcat(buffer, format);
    va_start(args, format);
    vfprintf(logFile, buffer, args);
    va_end(args);
    free(buffer);
}

void _shutdown_log() {
    if (logFile != NULL) {
        fclose(logFile);
        logFile = NULL;
    }
}
