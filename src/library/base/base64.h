#ifndef BASE64_H
#define BASE64_H

#include <cstdint>
#include <vector>
#include <string>

#if _WIN32
#include <wtypes.h>
#endif

namespace license {
/*
	bool SafeString: if true push a null terminator at the end of the binary data. This is useful when the binary data
   is a string.
*/
std::vector<uint8_t> unbase64(const std::string& base64_data, bool safeString = false);
std::string base64(const void* binaryData, size_t len, int lineLenght = -1);

}  // namespace license

#endif
