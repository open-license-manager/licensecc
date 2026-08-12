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
#include "../../base/string_utils.h"

#include <sys/ioctl.h>
#include <linux/nvme_ioctl.h>
#include <scsi/sg.h>
#include <fcntl.h>
#include <algorithm>

#include <mntent.h>
#include <dirent.h>
#include <sys/utsname.h>
#ifndef NDEBUG
#include <valgrind/memcheck.h>
#endif

// #ifdef USE_DISK_MODEL
/// #define PARSE_ID_FUNC parse_disk_id
// #define ID_FOLDER "/dev/disk/by-id"
// #else
#define PARSE_ID_FUNC parseUUID
#define ID_FOLDER "/dev/disk/by-uuid"
#define SERIAL_FOLDER "/run/udev/data"
// #endif
#ifdef USE_DBUS
#include <dbus-1.0/dbus/dbus.h>
#endif

using namespace license;

/**
 *Usually uuid are hex number separated by "-". this method read up to 8 hex
 *numbers skipping - characters.
 *@param uuid uuid as read in /dev/disk/by-uuid
 *@param buffer_out: unsigned char buffer[8] output buffer for result
 */
static void parseUUID(const char* uuid, unsigned char* buffer_out, unsigned int out_size) {
	unsigned int i, j;
	char* hexuuid;
	unsigned char cur_character;
	// remove characters not in hex set
	size_t len = strlen(uuid);
	hexuuid = (char*)malloc(sizeof(char) * len);
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
/**
 * Encode a string of arbitrary length into a fixed-size buffer by XORing
 * the string bytes into the buffer, wrapping around when the buffer end is reached.

 *
 * @param input the input string to encode
 * @param buffer_out output buffer to fill
 * @param out_size size of the output buffer (in bytes)
 */
static void encode_string_to_buffer(const std::string& input, unsigned char* buffer_out, size_t out_size) {
	memset(buffer_out, 0, out_size);
	for (size_t i = 0; i < input.size(); i++) {
		unsigned char current_byte = static_cast<unsigned char>(input[i]);
		if (i % 2 == 1) {
			current_byte = (current_byte << 4) | (current_byte >> 4);
		}
		buffer_out[i % out_size] ^= current_byte;
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

static std::string getAttribute(const std::string& source, const std::string& attrName) {
	std::string attr_namefull = attrName + "=\"";
	std::size_t startpos = source.find(attr_namefull) + attr_namefull.size();
	std::size_t endpos = source.find("\"", startpos);
	return source.substr(startpos, endpos - startpos);
}

FUNCTION_RETURN parse_blkid(const std::string& blkid_file_content, std::vector<DiskInfo>& diskInfos_out,
							std::unordered_map<std::string, int>& disk_by_uuid) {
	DiskInfo diskInfo = {};
	int diskNum = 0;
	for (std::size_t oldpos = 0, pos = 0; (pos = blkid_file_content.find("</device>", oldpos)) != std::string::npos;
		 oldpos = pos + 1) {
		std::string cur_dev = blkid_file_content.substr(oldpos, pos);
		diskInfo.id = diskNum++;
		std::string device = cur_dev.substr(cur_dev.find_last_of(">") + 1);
		mstrlcpy(diskInfo.device, device.c_str(), MAX_PATH);
		std::string label = getAttribute(cur_dev, "PARTLABEL");
		mstrlcpy(diskInfo.label, label.c_str(), 255);
		std::string disk_sn = getAttribute(cur_dev, "UUID");
		parseUUID(disk_sn.c_str(), diskInfo.disk_sn, sizeof(diskInfo.disk_sn));
		std::string disk_type = getAttribute(cur_dev, "TYPE");
		disk_by_uuid.insert(std::pair<std::string, int>(disk_sn, diskInfo.id));
		diskInfo.label_initialized = true;
		diskInfo.sn_initialized = true;
		// unlikely that somebody put the swap on a removable disk.
		// this is a first rough guess on what can be a preferred disk for blkid devices
		// just in case /etc/fstab can't be accessed or it is not up to date.
		diskInfo.preferred = (disk_type == "swap");
		diskInfos_out.push_back(diskInfo);
	}
	return FUNCTION_RETURN::FUNC_RET_OK;
}

#define BLKID_LOCATIONS {"/run/blkid/blkid.tab", "/etc/blkid.tab"};

static FUNCTION_RETURN getDiskInfos_blkid(std::vector<DiskInfo>& diskInfos,
										  std::unordered_map<std::string, int>& disk_by_uuid) {
	const char* strs[] = BLKID_LOCATIONS;
	bool can_read = false;
	std::stringstream buffer;
	for (int i = 0; i < sizeof(strs) / sizeof(const char*); i++) {
		const char* location = strs[i];
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

	return parse_blkid(buffer.str(), diskInfos, disk_by_uuid);
}

#define MAX_UNITS 40

static void read_disk_labels(std::vector<DiskInfo>& disk_infos) {
	struct stat sym_stat;
	struct dirent* dir;

	std::string label_dir("/dev/disk/by-label");
	DIR* disk_by_label = opendir(label_dir.c_str());
	if (disk_by_label == nullptr) {
		label_dir = "/dev/disk/by-partlabel";
		disk_by_label = opendir(label_dir.c_str());
	}
	if (disk_by_label != nullptr) {
		while ((dir = readdir(disk_by_label)) != nullptr) {
			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
				continue;
			}
			std::string cur_disk_label = label_dir + "/" + dir->d_name;
			if (stat(cur_disk_label.c_str(), &sym_stat) == 0) {
				bool found = false;
				for (auto& diskInfo : disk_infos) {
					if (((int)(sym_stat.st_ino)) == diskInfo.id) {
						mstrlcpy(diskInfo.label, dir->d_name, 255);
						diskInfo.label_initialized = true;
						LOG_DEBUG("Label for disk ino %d device %s, set to %s", sym_stat.st_ino, diskInfo.device,
								  diskInfo.label);
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
}

inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch) && 0 != ch; }));
}
inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch) && 0 != ch; }).base(),
			s.end());
}
inline void trim(std::string& s) {
	rtrim(s);
	ltrim(s);
}

/**
 * return the disk physical serial number
 * for nvme a Plain ASCII, null-padded to 20 bytes (eg S5H5NX0T100123)
 */
FUNCTION_RETURN getDiskSerial(const std::string& devname, std::string& out_serial) {
	std::string filename("/dev/");
	filename.append(devname);
	if (devname.compare(0, 4, "nvme") == 0) {
		int fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd < 0) {
			LOG_DEBUG("Can't open %s: %s", devname.c_str(), std::strerror(errno));
			return FUNC_RET_ERROR;
		}

		char buf[4096] = {0};
		struct nvme_admin_cmd mib = {0};
		mib.opcode = 0x06;
		mib.nsid = 0;
		mib.addr = (__u64)buf;
		mib.data_len = sizeof(buf);
		mib.cdw10 = 1;

		int ret = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &mib);
		close(fd);
		if (ret) {
			LOG_DEBUG("Ioctl for %s failed: %s", devname.c_str(), std::strerror(errno));
			return FUNC_RET_ERROR;
		}

		std::string serial(&buf[4], 20);
		trim(serial);
		out_serial = serial;
		LOG_DEBUG("Disk: %s; Serial: %s", devname.c_str(), serial.c_str());
	} else if (devname.compare(0, 2, "sd") == 0) {
		int fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd < 0) {
			LOG_DEBUG("Can't open %s: %s", devname.c_str(), std::strerror(errno));
			return FUNC_RET_ERROR;
		}
		unsigned char cmd[] = {0x12, 0x01, 0x80, 0, 0, 0};
		unsigned int data_size = 0x00ff;
		char data[data_size];
		unsigned int sense_len = 32;
		unsigned char sense[sense_len];
		int res, pl, i;

		cmd[3] = (data_size >> 8) & 0xff;
		cmd[4] = data_size & 0xff;
		struct sg_io_hdr io_hdr = {0};
		io_hdr.interface_id = 'S';
		io_hdr.cmdp = cmd;
		io_hdr.cmd_len = sizeof(cmd);
		io_hdr.sbp = sense;
		io_hdr.mx_sb_len = sense_len;
		io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
		io_hdr.dxferp = data;
		io_hdr.dxfer_len = data_size;
		io_hdr.timeout = 1000; /* SCSI timeout in ms */

		int ret = ioctl(fd, SG_IO, &io_hdr);
		close(fd);
		if (ret < 0) {
			LOG_DEBUG("Ioctl for %s failed: %s", devname.c_str(), std::strerror(errno));
			return FUNC_RET_ERROR;
		}
		if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
			return FUNC_RET_ERROR;
		}

		std::string serial(&data[4], (uint8_t)data[3] - 2);
		// serial = serial.c_str();
		trim(serial);
		out_serial = serial;
		LOG_DEBUG("Disk: %s; Serial: %s", devname.c_str(), serial.c_str());
	} else if (devname.compare(0, 2, "hd") == 0) {
		LOG_DEBUG("%s disk not supported", devname.c_str());
		return FUNC_RET_ERROR;
	} else {
		LOG_DEBUG("%s disk not supported", devname.c_str());
		return FUNC_RET_ERROR;
	}

	return FUNCTION_RETURN::FUNC_RET_OK;
}

FUNCTION_RETURN getDiskInfos_dev(std::vector<DiskInfo>& disk_infos,
								 std::unordered_map<std::string, int>& disk_by_uuid) {
	struct dirent* dir = NULL;
	struct stat sym_stat;
	FUNCTION_RETURN result;
	char device_name[MAX_PATH];

	DIR* disk_by_uuid_dir = opendir(ID_FOLDER);
	if (disk_by_uuid_dir == nullptr) {
		LOG_DEBUG("Open " ID_FOLDER " fail: %s", std::strerror(errno));
	} else {
		const std::string base_dir(ID_FOLDER "/");
		while ((dir = readdir(disk_by_uuid_dir)) != nullptr && disk_infos.size() < MAX_UNITS) {
			if (::strcmp(dir->d_name, ".") == 0 || ::strcmp(dir->d_name, "..") == 0 ||
				::strncmp(dir->d_name, "usb", 3) == 0) {
				continue;
			}

			std::string cur_dir = base_dir + dir->d_name;
			if (stat(cur_dir.c_str(), &sym_stat) == 0) {
				DiskInfo tmpDiskInfo = {};
				tmpDiskInfo.id = sym_stat.st_ino;
				ssize_t len = ::readlink(cur_dir.c_str(), device_name, MAX_PATH - 1);
				if (len != -1) {
					device_name[len] = '\0';
					std::string device_name_s(device_name, len);
					auto pos = device_name_s.find_last_of("/");
					if (pos != std::string::npos) {
						device_name_s = device_name_s.substr(pos + 1);
					}
					mstrlcpy(tmpDiskInfo.device, device_name_s.c_str(), sizeof(tmpDiskInfo.device));
					PARSE_ID_FUNC(dir->d_name, tmpDiskInfo.disk_sn, sizeof(tmpDiskInfo.disk_sn));
					std::string serial;
					if (getDiskSerial(device_name_s, serial) == FUNC_RET_OK) {
						encode_string_to_buffer(serial, tmpDiskInfo.physical_serial,
												sizeof(tmpDiskInfo.physical_serial));
						tmpDiskInfo.physical_serial_initialized = true;
					}
					tmpDiskInfo.sn_initialized = true;
					tmpDiskInfo.label_initialized = false;
					tmpDiskInfo.preferred = false;
					bool found = false;
					for (auto diskInfo : disk_infos) {
						if (tmpDiskInfo.id == diskInfo.id) {
							found = true;
							break;
						}
					}
					disk_by_uuid.insert(std::pair<std::string, int>(std::string(dir->d_name), tmpDiskInfo.id));
					if (!found) {
						LOG_DEBUG("Found disk inode %d device %s, sn %s", sym_stat.st_ino, tmpDiskInfo.device,
								  dir->d_name);
						disk_infos.push_back(tmpDiskInfo);
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

	result = disk_infos.size() > 0 ? FUNCTION_RETURN::FUNC_RET_OK : FUNCTION_RETURN::FUNC_RET_NOT_AVAIL;
	read_disk_labels(disk_infos);
	return result;
}

/**
 * Try to determine removable devices: as a first guess removable devices doesn't have
 * an entry in /etc/fstab
 *
 * @param diskInfos
 */
static void set_preferred_disks(std::vector<DiskInfo>& diskInfos, std::unordered_map<std::string, int>& disk_by_uuid) {
	FILE* fstabFile = setmntent("/etc/fstab", "r");
	if (fstabFile == nullptr) {
		LOG_DEBUG("/etc/fstab not accessible");
		return;
	}
	struct mntent* ent;
	while (nullptr != (ent = getmntent(fstabFile))) {
		bool found = false;
		std::string device_name_s(ent->mnt_fsname);
		LOG_DEBUG("found fstab entry %s ", ent->mnt_fsname);
		if (strncmp("UUID=", ent->mnt_fsname, 5) == 0) {
			// fstab entry is uuid
			device_name_s = device_name_s.substr(5);
			auto it = disk_by_uuid.find(device_name_s);
			if (it != disk_by_uuid.end()) {
				for (auto& disk_info : diskInfos) {
					if (it->second == disk_info.id) {
						disk_info.preferred = true;
						LOG_DEBUG("Disk %d device %s set as preferred", disk_info.id, disk_info.device);
						break;
					}
				}
			} else {
				LOG_DEBUG("fstab device %s found, but no corresponding diskInfo", ent->mnt_fsname);
			}
		} else if (strncmp("LABEL=", ent->mnt_fsname, 6) == 0) {
			// fstab entry is uuid
			device_name_s = device_name_s.substr(6);
			for (auto& disk_info : diskInfos) {
				if (device_name_s == disk_info.label) {
					disk_info.preferred = true;
					LOG_DEBUG("Disk %d device %s set as preferred", disk_info.id, disk_info.device);
					break;
				}
			}
		} else {
			// fstab entry is a device
			auto pos = device_name_s.find_last_of("/");
			if (pos != std::string::npos) {
				device_name_s = device_name_s.substr(pos + 1);
			}

			for (auto disk_info : diskInfos) {
				if (device_name_s == disk_info.device) {
					disk_info.preferred = true;
					LOG_DEBUG("Disk %d device %s set as preferred", disk_info.id, disk_info.device);
					break;
				}
			}
		}
	}
	endmntent(fstabFile);
	return;
}

/**
 * First try to read disk_infos from /dev/disk/by-uuid folder, if fails try to use
 * blkid cache to see what's in there, then try to exclude removable disks
 * looking at /etc/fstab
 * @param diskInfos_out vector used to output the disk informations
 * @return
 */
FUNCTION_RETURN getDiskInfos(std::vector<DiskInfo>& disk_infos) {
	std::unordered_map<std::string, int> disk_by_uuid;

	FUNCTION_RETURN result = getDiskInfos_dev(disk_infos, disk_by_uuid);

	if (result != FUNCTION_RETURN::FUNC_RET_OK) {
		result = getDiskInfos_blkid(disk_infos, disk_by_uuid);
	}
	if (result == FUNCTION_RETURN::FUNC_RET_OK) {
		set_preferred_disks(disk_infos, disk_by_uuid);
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

FUNCTION_RETURN getOsSpecificIdentifier(unsigned char identifier[HW_IDENTIFIER_PROPRIETARY_DATA]) {
#if USE_DBUS
	char* dbus_id = dbus_get_local_machine_id();
	if (dbus_id == NULL) {
		return FUNC_RET_ERROR;
	}
	memcpy(identifier, dbus_id, HW_IDENTIFIER_PROPRIETARY_DATA);
	dbus_free(dbus_id);
	return FUNC_RET_OK;
#else
	std::ifstream machine_id_file("/etc/machine-id");
	if (!machine_id_file.is_open()) {
		return FUNC_RET_NOT_AVAIL;
	}
	std::string machine_id((std::istreambuf_iterator<char>(machine_id_file)), std::istreambuf_iterator<char>());
	encode_string_to_buffer(machine_id, identifier, HW_IDENTIFIER_PROPRIETARY_DATA);
	return FUNC_RET_OK;
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
	if (ch > MAX_PATH || ch < 0) {
		result = FUNC_RET_ERROR;
	} else {
		mstrlcpy(buffer, path, ch + 1);
		result = FUNC_RET_OK;
	}
	return result;
}
