#ifndef BASE64_H
#define BASE64_H

#include <string>

#if _WIN32
#include <wtypes.h>
#endif

namespace license {

unsigned char* unbase64(const char* ascii, int len, int* flen);
std::string base64(const void* binaryData, size_t len, int lineLenght = -1);

}  // namespace license

#endif
