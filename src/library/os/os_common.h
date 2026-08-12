

#ifndef OS_COMMON_HPP_
#define OS_COMMON_HPP_

#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdlib.h>
#include <vector>
#include <string>
#ifdef __unix__
#include <unistd.h>
#include <stdbool.h>
#endif

#include <licensecc/datatypes.h>
#include "../base/base.h"

/**
 * Render a 16-byte identifier as a canonical GUID string, e.g. "3F2504E0-4F89-41D3-9A0C-0305E82C3301". Byte order for
 * the first three fields is little-endian, matching how the Windows GetSystemIdForPublisher API exposes its 16-byte
 * identifier.
 */
std::string guidToString(const unsigned char (&bytes)[16]);

#endif /* OS_COMMON_HPP_ */
