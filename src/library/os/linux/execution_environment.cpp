/*
 * virtualization.cpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */
#include <paths.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/utsname.h>

#include "../../base/base.h"
#include "../cpu_info.hpp"
#include "../execution_environment.hpp"

namespace license {
using namespace std;

// 0=NO 1=Docker/2=Lxc
static int checkContainerProc() {
	// in docer /proc/self/cgroups contains the "docker" or "lxc" string
	// https://stackoverflow.com/questions/23513045/how-to-check-if-a-process-is-running-inside-docker-container
	char path[MAX_PATH] = {0};
	char proc_path[MAX_PATH], pidStr[64];
	pid_t pid = getpid();
	sprintf(pidStr, "%d", pid);
	strcpy(proc_path, "/proc/");
	strcat(proc_path, pidStr);
	strcat(proc_path, "/cgroup");

	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int result = 0;

	fp = fopen(proc_path, "r");
	if (fp == NULL) {
		return 0;
	}

	while ((read = getline(&line, &len, fp)) != -1 && result == 0) {
		// line[len]=0;
		// printf("Retrieved line of length %zu:\n", read);
		// printf("%s", line);
		if (strstr(line, "docker") != NULL) {
			result = 1;
		}
		if (strstr(line, "lxc") != NULL) {
			result = 2;
		}
	}

	fclose(fp);
	if (line) free(line);
	return result;
}

// 0=NO 1=Docker/2=Lxc
static int checkSystemdContainer() {
	ifstream systemd_container("/var/run/systemd/container");
	int result = 0;
	if (systemd_container.good()) {
		result = 1;
		for (string line; getline(systemd_container, line);) {
			if (line.find("docker") != string::npos) {
				result = 1;
				break;
			} else if (line.find("lxc") != string::npos) {
				result = 2;
				break;
			}
		}
	}
	return result;
}

VIRTUALIZATION ExecutionEnvironment::getVirtualization() {
	VIRTUALIZATION result;
	CpuInfo cpuInfo;
	bool isContainer = checkContainerProc() != 0 || checkSystemdContainer() != 0;
	if (isContainer) {
		result = CONTAINER;
	} else if (cpuInfo.cpu_virtual() || is_cloud()) {
		result = VM;
	} else {
		result = NONE;
	}
	return result;
}

bool ExecutionEnvironment::is_cloud() { return getCloudProvider() == NONE; }

bool ExecutionEnvironment::is_docker() { return (checkContainerProc() == 1 || checkSystemdContainer() == 1); }

CLOUD_PROVIDER ExecutionEnvironment::getCloudProvider() {}


}  // namespace license
