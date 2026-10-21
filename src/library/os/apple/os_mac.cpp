/**
 * @file os_mac.cpp
 * @brief macOS implementation of the OS-dependent functions (os.h).
 *
 * Uses only native macOS APIs (IOKit, sysctl, getfsstat, uname,
 * _NSGetExecutablePath): no subprocesses are spawned and no command
 * output is parsed.
 */

#include <string>
#include <cstring>
#include <cerrno>
#include <vector>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <sys/attr.h>
#include <unistd.h>
#include <uuid/uuid.h>

#include <AvailabilityMacros.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <mach-o/dyld.h>

// kIOMasterPortDefault is deprecated since macOS 12 in favor of kIOMainPortDefault
#if defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 120000
#define LCC_IO_MAIN_PORT kIOMainPortDefault
#else
#define LCC_IO_MAIN_PORT kIOMasterPortDefault
#endif

#include "../os.h"
#include "../os_common.h"
#include "../../base/logger.h"
#include "../../base/string_utils.h"

using namespace license;

/**
 * Read a CFString property of the IOPlatformExpertDevice (the "platform
 * expert" node, present on every Mac).
 */
static std::string get_platform_property(const char* property_name) {
	std::string result;
	io_service_t platform = IOServiceGetMatchingService(LCC_IO_MAIN_PORT, IOServiceMatching("IOPlatformExpertDevice"));
	if (platform == IO_OBJECT_NULL) {
		LOG_DEBUG("IOPlatformExpertDevice not found");
		return result;
	}
	CFStringRef key = CFStringCreateWithCString(kCFAllocatorDefault, property_name, kCFStringEncodingUTF8);
	CFTypeRef prop = IORegistryEntryCreateCFProperty(platform, key, kCFAllocatorDefault, 0);
	CFRelease(key);
	IOObjectRelease(platform);
	if (prop != nullptr) {
		if (CFGetTypeID(prop) == CFStringGetTypeID()) {
			char buffer[256] = {0};
			if (CFStringGetCString((CFStringRef)prop, buffer, sizeof(buffer), kCFStringEncodingUTF8)) {
				result = buffer;
			}
		}
		CFRelease(prop);
	}
	return result;
}

/**
 * Read a sysctl string parameter, e.g. "hw.model".
 */
static std::string get_sysctl_string(const char* name) {
	std::string result;
	size_t size = 0;
	if (sysctlbyname(name, nullptr, &size, nullptr, 0) != 0 || size == 0) {
		return result;
	}
	result.resize(size);
	if (sysctlbyname(name, &result[0], &size, nullptr, 0) != 0) {
		return "";
	}
	// strip trailing '\0' copied from the kernel buffer
	while (!result.empty() && result.back() == '\0') {
		result.pop_back();
	}
	return result;
}

/**
 * Serial number of the physical storage device behind a BSD device name
 * (e.g. "disk0s2"). Looks up the IOMedia node with the matching "BSD Name"
 * and searches the service plane upwards for the "Serial Number" property,
 * set by the IOBlockStorageDriver of the physical disk.
 */
static std::string get_disk_serial(const std::string& bsd_name) {
	std::string serial;
	CFMutableDictionaryRef match = IOServiceMatching("IOMedia");
	if (match == nullptr) {
		return serial;
	}
	CFStringRef bsd = CFStringCreateWithCString(kCFAllocatorDefault, bsd_name.c_str(), kCFStringEncodingUTF8);
	CFDictionarySetValue(match, CFSTR("BSD Name"), bsd);
	io_service_t media = IOServiceGetMatchingService(LCC_IO_MAIN_PORT, match);
	CFRelease(bsd);
	if (media == IO_OBJECT_NULL) {
		return serial;
	}
	CFTypeRef prop =
		IORegistryEntrySearchCFProperty(media, kIOServicePlane, CFSTR("Serial Number"), kCFAllocatorDefault,
										kIORegistryIterateRecursively | kIORegistryIterateParents);
	IOObjectRelease(media);
	if (prop != nullptr) {
		if (CFGetTypeID(prop) == CFStringGetTypeID()) {
			char buffer[256] = {0};
			if (CFStringGetCString((CFStringRef)prop, buffer, sizeof(buffer), kCFStringEncodingUTF8)) {
				serial = buffer;
			}
		} else if (CFGetTypeID(prop) == CFDataGetTypeID()) {
			CFDataRef data = (CFDataRef)prop;
			CFIndex len = CFDataGetLength(data);
			if (len > 0) {
				const UInt8* bytes = CFDataGetBytePtr(data);
				serial.assign(reinterpret_cast<const char*>(bytes), len);
				// strip trailing garbage
				serial.erase(std::find(serial.begin(), serial.end(), '\0'), serial.end());
			}
		}
		CFRelease(prop);
	}
	return serial;
}

/**
 * Volume UUID of the filesystem mounted at mountpoint, via getattrlist(2).
 */
static bool get_volume_uuid(const char* mountpoint, std::string& uuid_out) {
	struct {
		u_int32_t size;
		uuid_t uuid;
	} __attribute__((packed)) attr_buf;

	struct attrlist attrs;
	memset(&attrs, 0, sizeof(attrs));
	attrs.bitmapcount = ATTR_BIT_MAP_COUNT;
	attrs.volattr = ATTR_VOL_INFO | ATTR_VOL_UUID;

	if (getattrlist(mountpoint, &attrs, &attr_buf, sizeof(attr_buf), 0) != 0) {
		LOG_DEBUG("getattrlist %s failed: %s", mountpoint, std::strerror(errno));
		return false;
	}
	if (attr_buf.size != sizeof(attr_buf)) {
		return false;
	}
	char uuid_str[37];
	uuid_unparse_lower(attr_buf.uuid, uuid_str);
	uuid_out = uuid_str;
	return true;
}

/**
 * Volume label (name set by the user, e.g. "Macintosh HD") of the
 * filesystem mounted at mountpoint, via getattrlist(2).
 */
static bool get_volume_label(const char* mountpoint, std::string& label_out) {
	// buffer must leave room for the string referenced by the attrreference
	struct {
		u_int32_t size;
		struct attrreference name;
		char data[256];
	} __attribute__((packed)) attr_buf;

	struct attrlist attrs;
	memset(&attrs, 0, sizeof(attrs));
	attrs.bitmapcount = ATTR_BIT_MAP_COUNT;
	attrs.volattr = ATTR_VOL_INFO | ATTR_VOL_NAME;

	if (getattrlist(mountpoint, &attrs, &attr_buf, sizeof(attr_buf), 0) != 0) {
		LOG_DEBUG("getattrlist %s failed: %s", mountpoint, std::strerror(errno));
		return false;
	}
	// attr_dataoffset is relative to the beginning of the attrreference structure
	if (attr_buf.name.attr_dataoffset + attr_buf.name.attr_length > sizeof(attr_buf)) {
		LOG_DEBUG("unexpected attrreference layout for %s", mountpoint);
		return false;
	}
	const char* name = reinterpret_cast<char*>(&attr_buf.name) + attr_buf.name.attr_dataoffset;
	label_out.assign(name, strnlen(name, attr_buf.name.attr_length));
	return !label_out.empty();
}

FUNCTION_RETURN getDiskInfos(std::vector<DiskInfo>& disk_infos) {
	int num_mounts = getfsstat(nullptr, 0, MNT_NOWAIT);
	if (num_mounts <= 0) {
		LOG_DEBUG("getfsstat failed");
		return FUNC_RET_NOT_AVAIL;
	}
	std::vector<struct statfs> mounts(num_mounts);
	num_mounts = getfsstat(mounts.data(), num_mounts * sizeof(struct statfs), MNT_NOWAIT);
	if (num_mounts <= 0) {
		return FUNC_RET_NOT_AVAIL;
	}

	int next_id = 0;
	for (int i = 0; i < num_mounts; i++) {
		const struct statfs& m = mounts[i];
		const std::string from_name(m.f_mntfromname);
		// only local disk-backed filesystems (skips devfs, tmpfs, network mounts...)
		if ((m.f_flags & MNT_LOCAL) == 0 || from_name.compare(0, 9, "/dev/disk") != 0) {
			continue;
		}
		const std::string bsd_name = from_name.substr(from_name.find_last_of('/') + 1);

		// merge multiple mounts of the same volume (e.g. snapshots, firmlinks)
		auto existing = std::find_if(disk_infos.begin(), disk_infos.end(),
									 [&bsd_name](const DiskInfo& d) { return bsd_name == d.device; });
		if (existing != disk_infos.end()) {
			if (strcmp(m.f_mntonname, "/") == 0) {
				existing->preferred = true;
			}
			continue;
		}

		DiskInfo disk_info = {};
		disk_info.id = next_id++;
		mstrlcpy(disk_info.device, bsd_name.c_str(), MAX_PATH);
		disk_info.preferred = (strcmp(m.f_mntonname, "/") == 0);

		std::string uuid;
		if (get_volume_uuid(m.f_mntonname, uuid)) {
			disk_info.disk_uuid = uuid;
			disk_info.uuid_initialized = true;
		}
		std::string serial = get_disk_serial(bsd_name);
		if (!serial.empty()) {
			disk_info.disk_phys_id = serial;
			disk_info.disk_phys_id_initialized = true;
		}
		std::string label;
		if (get_volume_label(m.f_mntonname, label)) {
			mstrlcpy(disk_info.label, label.c_str(), 255);
			disk_info.label_initialized = true;
		}
		disk_infos.push_back(disk_info);
	}

	return disk_infos.size() > 0 ? FUNC_RET_OK : FUNC_RET_NOT_AVAIL;
}

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	static struct utsname u;

	if (uname(&u) < 0) {
		return FUNC_RET_ERROR;
	}
	memcpy(identifier, u.nodename, 6);
	return FUNC_RET_OK;
}

FUNCTION_RETURN getOsSpecificIdentifier(std::string& identifier) {
	// The platform UUID is stable across OS reinstalls and updates,
	// making it the most reliable machine identifier on macOS.
	identifier = get_platform_property("IOPlatformUUID");
	if (identifier.empty()) {
		return FUNC_RET_NOT_AVAIL;
	}
	return FUNC_RET_OK;
}

FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]) {
	uint32_t size = MAX_PATH;
	int result = _NSGetExecutablePath(buffer, &size);
	if (result != 0) {
		return FUNC_RET_ERROR;
	}
	return FUNC_RET_OK;
}
