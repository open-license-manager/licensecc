/*
 * execution_environment.cpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */
#define __STDC_WANT_LIB_EXT1__1
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
#include "../../base/file_utils.hpp"
#include "../../base/string_utils.h"

namespace license {
namespace os {
using namespace std;

// 0=NO 1=Docker/2=Lxc
static CONTAINER_TYPE checkContainerProc() {
	// in docer /proc/self/cgroups contains the "docker" or "lxc" string
	// https://stackoverflow.com/questions/23513045/how-to-check-if-a-process-is-running-inside-docker-container
	char path[MAX_PATH] = {0};
	char proc_path[MAX_PATH], pidStr[64];
	pid_t pid = getpid();
	snprintf(pidStr, sizeof(pidStr), "%d", pid);
	strncpy(proc_path, "/proc/", sizeof(proc_path));
	strncat(proc_path, pidStr, sizeof(proc_path));
	strncpy(proc_path, "/cgroup", sizeof(proc_path));

	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	CONTAINER_TYPE result = CONTAINER_TYPE::NONE;

	fp = fopen(proc_path, "r");
	if (fp == NULL) {
		return CONTAINER_TYPE::NONE;
	}

	while ((read = getline(&line, &len, fp)) != -1 && result == 0) {
		if (strstr(line, "docker") != NULL) {
			result = CONTAINER_TYPE::DOCKER;
		}
		if (strstr(line, "lxc") != NULL) {
			result = CONTAINER_TYPE::LXC;
		}
	}

	fclose(fp);
	if (line) free(line);
	return result;
}

// 0=NO 1=Docker/2=Lxc
static CONTAINER_TYPE checkSystemdContainer() {
	ifstream systemd_container("/var/run/systemd/container");
	CONTAINER_TYPE result = CONTAINER_TYPE::NONE;
	if (systemd_container.good()) {
		result = CONTAINER_TYPE::DOCKER;
		for (string line; getline(systemd_container, line);) {
			if (line.find("docker") != string::npos) {
				result = CONTAINER_TYPE::DOCKER;
				break;
			} else if (line.find("lxc") != string::npos) {
				result = CONTAINER_TYPE::LXC;
				break;
			}
		}
	}
	return result;
}

static CONTAINER_TYPE get_container_type() {
	CONTAINER_TYPE result = checkContainerProc();
	if (result == CONTAINER_TYPE::NONE) {
		result = checkSystemdContainer();
	}
	return result;
}

ExecutionEnvironment::ExecutionEnvironment() : m_container_type(get_container_type()) {}

}  // namespace os
}  // namespace license
