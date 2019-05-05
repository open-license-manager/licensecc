#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifdef __unix__
#include <unistd.h>
#define MAX_PATH 255
#else
#include <windows.h>
#endif

static FILE *logFile = NULL;

static void timenow(char * buffer) {
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", timeinfo);
}

static void getLogFname(char* logpath) {
#ifdef __unix__
	char const *folder = getenv("TMPDIR");
	if (folder == 0) {
		folder = "/tmp";
	}
	strncpy(logpath, folder, MAX_PATH);
	strncat(logpath, "/open-license.log", MAX_PATH - strlen(logpath));
#else
	int plen = GetTempPath(MAX_PATH, logpath);
	if(plen == 0) {
		fprintf(stderr, "Error getting temporary directory path");
	}
	strncat(logpath, "open-license.log", MAX_PATH - strlen(logpath));
#endif
}

void _log(const char* format, ...) {
	char logpath[MAX_PATH];
	va_list args;
	char * buffer;
	if (logFile == NULL) {
		getLogFname(logpath);
		logFile = fopen(logpath, "a");
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
