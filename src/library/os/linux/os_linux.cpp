#include <paths.h>
#include <sys/stat.h>
#include <stdio.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "../os.h"
#include "../../base/logger.h"

#include <mntent.h>
#include <dirent.h>
#include <sys/utsname.h>
#ifndef NDEBUG
#include <valgrind/memcheck.h>
#endif

#ifdef USE_DISK_MODEL
#define PARSE_ID_FUNC parse_disk_id
#define ID_FOLDER "/dev/disk/by-id"
#else
#define PARSE_ID_FUNC parseUUID
#define ID_FOLDER "/dev/disk/by-uuid"
#endif
#ifdef USE_DBUS
#include <dbus-1.0/dbus/dbus.h>
#endif

/**
 *Usually uuid are hex number separated by "-". this method read up to 8 hex
 *numbers skipping - characters.
 *@param uuid uuid as read in /dev/disk/by-uuid
 *@param buffer_out: unsigned char buffer[8] output buffer for result
 */
static void parseUUID(const char *uuid, unsigned char *buffer_out, unsigned int out_size) {
	unsigned int i, j;
	char *hexuuid;
	unsigned char cur_character;
	// remove characters not in hex set
	size_t len = strlen(uuid);
	hexuuid = (char *)malloc(sizeof(char) * len);
	memset(buffer_out, 0, out_size);
	memset(hexuuid, 0, sizeof(char) * len);

	for (i = 0, j = 0; i < len; i++) {
		if (isxdigit(uuid[i])) {
			hexuuid[j] = uuid[i];
			j++;
		} else {
			// skip
			continue;
		}
	}
	if (j % 2 == 1) {
		hexuuid[j++] = '0';
	}
	hexuuid[j] = '\0';
	for (i = 0; i < j / 2; i++) {
		sscanf(&hexuuid[i * 2], "%2hhx", &cur_character);
		buffer_out[i % out_size] = buffer_out[i % out_size] ^ cur_character;
	}

	free(hexuuid);
}

static void parse_disk_id(const char *uuid, unsigned char *buffer_out, size_t out_size) {
	unsigned int i;
	size_t len = strlen(uuid);
	memset(buffer_out, 0, out_size);
	for (i = 0; i < len; i++) {
		buffer_out[i % out_size] = buffer_out[i % out_size] ^ uuid[i];
	}
}

/**
 * 	int id;
	char device[MAX_PATH];
	unsigned char disk_sn[8];
	char label[255];
	int preferred;
 * @param blkidfile
 * @param diskInfos_out
 * @return
 */

static std::string getAttribute(const std::string &source, const std::string &attrName) {
	std::string attr_namefull = attrName + "=\"";
	std::size_t startpos = source.find(attr_namefull) + attr_namefull.size();
	std::size_t endpos = source.find("\"", startpos);
	return source.substr(startpos, endpos - startpos);
}

FUNCTION_RETURN parse_blkid(const std::string &blkid_file_content, std::vector<DiskInfo> &diskInfos_out) {
	DiskInfo diskInfo;
	int diskNum = 0;
	for (std::size_t oldpos = 0, pos = 0; (pos = blkid_file_content.find("</device>", oldpos)) != std::string::npos;
		 oldpos = pos + 1) {
		std::string cur_dev = blkid_file_content.substr(oldpos, pos);
		diskInfo.id = diskNum++;
		std::string device = cur_dev.substr(cur_dev.find_last_of(">") + 1);
		strncpy(diskInfo.device, device.c_str(), MAX_PATH);
		std::string label = getAttribute(cur_dev, "PARTLABEL");
		strncpy(diskInfo.label, label.c_str(), 255);
		std::string disk_sn = getAttribute(cur_dev, "UUID");
		parseUUID(disk_sn.c_str(), diskInfo.disk_sn, sizeof(diskInfo.disk_sn));
		std::string disk_type = getAttribute(cur_dev, "TYPE");
		// unlikely that somebody put the swap on a removable disk.
		// this is a first rough guess on what can be a preferred disk for blkid devices
		// just in case /etc/fstab can't be accessed or it is not up to date.
		diskInfo.preferred = (disk_type == "swap");
		diskInfos_out.push_back(diskInfo);
	}
	return FUNCTION_RETURN::FUNC_RET_OK;
}

#define BLKID_LOCATIONS {"/run/blkid/blkid.tab", "/etc/blkid.tab"};

static FUNCTION_RETURN getDiskInfos_blkid(std::vector<DiskInfo> &diskInfos) {
	const char *strs[] = BLKID_LOCATIONS;
	bool can_read = false;
	std::stringstream buffer;
	for (int i = 0; i < sizeof(strs) / sizeof(const char *); i++) {
		const char *location = strs[i];
		std::ifstream t(location);
		if (t.is_open()) {
			buffer << t.rdbuf();
			can_read = true;
			break;
		}
	}
	if (!can_read) {
		return FUNCTION_RETURN::FUNC_RET_NOT_AVAIL;
	}

	return parse_blkid(buffer.str(), diskInfos);
}

#define MAX_UNITS 40
FUNCTION_RETURN getDiskInfos_dev(std::vector<DiskInfo> &diskInfos) {
	struct dirent *dir = NULL;
	struct stat sym_stat;
	FUNCTION_RETURN result;

	DIR *disk_by_uuid_dir = opendir(ID_FOLDER);
	if (disk_by_uuid_dir == nullptr) {
		LOG_DEBUG("Open " ID_FOLDER " fail: %s", std::strerror(errno));
	} else {
		const std::string base_dir(ID_FOLDER "/");
		while ((dir = readdir(disk_by_uuid_dir)) != nullptr && diskInfos.size() < MAX_UNITS) {
			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 ||
				strncmp(dir->d_name, "usb", 3) == 0) {
				continue;
			}

			std::string cur_dir = base_dir + dir->d_name;
			if (stat(cur_dir.c_str(), &sym_stat) == 0) {
				DiskInfo tmpDiskInfo;
				tmpDiskInfo.id = sym_stat.st_ino;
				ssize_t len = ::readlink(cur_dir.c_str(), tmpDiskInfo.device, sizeof(tmpDiskInfo.device) - 1);
				if (len != -1) {
					tmpDiskInfo.device[len] = '\0';
					PARSE_ID_FUNC(dir->d_name, tmpDiskInfo.disk_sn, sizeof(tmpDiskInfo.disk_sn));
					tmpDiskInfo.sn_initialized = true;
					tmpDiskInfo.label_initialized = false;
					tmpDiskInfo.preferred = false;
					bool found = false;
					for (auto diskInfo : diskInfos) {
						if (tmpDiskInfo.id == diskInfo.id) {
							found = true;
							break;
						}
					}
					if (!found) {
						LOG_DEBUG("Found disk inode %d device %s, sn %s", sym_stat.st_ino, tmpDiskInfo.device,
								  dir->d_name);
						diskInfos.push_back(tmpDiskInfo);
					}
				} else {
					LOG_DEBUG("Error %s during readlink of %s", std::strerror(errno), cur_dir.c_str());
				}
			} else {
				LOG_DEBUG("Error %s during stat of %s", std::strerror(errno), cur_dir.c_str());
			}
		}
		closedir(disk_by_uuid_dir);
	}

	result = diskInfos.size() > 0 ? FUNCTION_RETURN::FUNC_RET_OK : FUNCTION_RETURN::FUNC_RET_NOT_AVAIL;
	const std::string label_dir("/dev/disk/by-label");
	DIR *disk_by_label = opendir(label_dir.c_str());
	if (disk_by_label != nullptr) {
		while ((dir = readdir(disk_by_label)) != nullptr) {
			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
				continue;
			}
			std::string cur_disk_label = label_dir + "/" + dir->d_name;
			if (stat(cur_disk_label.c_str(), &sym_stat) == 0) {
				bool found = false;
				for (auto diskInfo : diskInfos) {
					if (((int)sym_stat.st_ino) == diskInfo.id) {
						strncpy(diskInfo.label, dir->d_name, 255 - 1);
						diskInfo.label_initialized = true;
						break;
					}
				}
			} else {
				LOG_DEBUG("Stat %s for fail:F %s", cur_disk_label.c_str(), std::strerror(errno));
			}
		}
		closedir(disk_by_label);
	} else {
		LOG_DEBUG("Open %s for reading disk labels fail: %s", label_dir.c_str(), std::strerror(errno));
	}

	return result;
}

/**
 * Try to determine removable devices: as a first guess removable devices doesn't have
 * an entry in /etc/fstab
 *
 * @param diskInfos
 */
static void set_preferred_disks(std::vector<DiskInfo> &diskInfos) {
	FILE *fstabFile = setmntent("/etc/fstab", "r");
	if (fstabFile == nullptr) {
		/*fstab not accessible*/
		return;
	}
	struct mntent *ent;
	while (nullptr != (ent = getmntent(fstabFile))) {
		bool found = false;
		for (auto disk_info : diskInfos) {
			if (strcmp(ent->mnt_fsname, disk_info.device) == 0) {
				disk_info.preferred = true;
				break;
			}
		}
	}
	endmntent(fstabFile);
	return;
}

/**
 * First try to read disk_infos from /dev/disk/by-id folder, if fails try to use
 * blkid cache to see what's in there, then try to exclude removable disks
 * looking at /etc/fstab
 * @param diskInfos_out vector used to output the disk informations
 * @return
 */
FUNCTION_RETURN getDiskInfos(std::vector<DiskInfo> &disk_infos) {
	FUNCTION_RETURN result = getDiskInfos_dev(disk_infos);
	if (result != FUNCTION_RETURN::FUNC_RET_OK) {
		result = getDiskInfos_blkid(disk_infos);
	}
	if (result == FUNCTION_RETURN::FUNC_RET_OK) {
		set_preferred_disks(disk_infos);
	}
	return result;
}

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	static struct utsname u;

	if (uname(&u) < 0) {
		return FUNC_RET_ERROR;
	}
	memcpy(identifier, u.nodename, 6);
	return FUNC_RET_OK;
}

FUNCTION_RETURN getOsSpecificIdentifier(unsigned char identifier[6]) {
#if USE_DBUS
	char *dbus_id = dbus_get_local_machine_id();
	if (dbus_id == NULL) {
		return FUNC_RET_ERROR;
	}
	memcpy(identifier, dbus_id, 6);
	dbus_free(dbus_id);
	return FUNC_RET_OK;
#else
	return FUNC_RET_NOT_AVAIL;
#endif
}

FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]) {
	FUNCTION_RETURN result;
	char path[MAX_PATH] = {0};
	char proc_path[MAX_PATH], pidStr[64];
	pid_t pid = getpid();
	sprintf(pidStr, "%d", pid);
	strcpy(proc_path, "/proc/");
	strcat(proc_path, pidStr);
	strcat(proc_path, "/exe");

	int ch = readlink(proc_path, path, MAX_PATH - 1);
	if (ch != -1) {
		path[ch] = '\0';
		strncpy(buffer, path, ch);
		result = FUNC_RET_OK;
	} else {
		result = FUNC_RET_ERROR;
	}
	return result;
}
