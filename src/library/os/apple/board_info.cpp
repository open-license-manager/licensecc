/*
 * board_info.cpp - macOS implementation
 *
 * Reads board/firmware information from the IORegistry (IOKit C API).
 * On Apple Silicon there is no SMBIOS/DMI; the platform expert device and
 * the hw.model sysctl carry the equivalent information.
 */

#include <string>
#include <cstring>
#include <sys/sysctl.h>

#include <AvailabilityMacros.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

// kIOMasterPortDefault is deprecated since macOS 12 in favor of kIOMainPortDefault
#if defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 120000
#define LCC_IO_MAIN_PORT kIOMainPortDefault
#else
#define LCC_IO_MAIN_PORT kIOMasterPortDefault
#endif

#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../board_info.hpp"

namespace license {
namespace os {
using namespace std;

/**
 * Read a CFString property of the IOPlatformExpertDevice.
 */
static string get_platform_property(const char* property_name) {
	string result;
	io_service_t platform = IOServiceGetMatchingService(LCC_IO_MAIN_PORT, IOServiceMatching("IOPlatformExpertDevice"));
	if (platform == IO_OBJECT_NULL) {
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
static string get_sysctl_string(const char* name) {
	string result;
	size_t size = 0;
	if (sysctlbyname(name, nullptr, &size, nullptr, 0) != 0 || size == 0) {
		return result;
	}
	result.resize(size);
	if (sysctlbyname(name, &result[0], &size, nullptr, 0) != 0) {
		return "";
	}
	while (!result.empty() && result.back() == '\0') {
		result.pop_back();
	}
	return result;
}

BoardInfo::BoardInfo() {
	// e.g. "Apple Inc."
	string manufacturer = toupper_copy(trim_copy(get_platform_property("manufacturer")));
	m_sys_vendor = manufacturer.empty() ? "APPLE" : manufacturer;
	// no legacy BIOS on macOS, the platform manufacturer is the closest equivalent
	m_bios_vendor = m_sys_vendor;
	// e.g. "MACBOOKPRO18,3" or "MAC14,2"; VMs report e.g. "PARALLELS VIRTUAL PLATFORM"
	string model = toupper_copy(trim_copy(get_sysctl_string("hw.model")));
	if (model.empty()) {
		model = toupper_copy(trim_copy(get_platform_property("model")));
	}
	m_bios_description = model.empty() ? "APPLE" : model;
}

}  // namespace os
}  // namespace license
