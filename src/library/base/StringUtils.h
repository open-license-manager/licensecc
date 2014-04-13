/*
 * StringUtils.h
 *
 *  Created on: Apr 8, 2014
 *      Author: devel
 */

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_
#include <string>

namespace license {
using namespace std;

/**
 * Eliminate whitespace from both sides of a string and returns a copy
 * of the string itself.
 * @param string_to_trim
 * @return the trimmed string
 */
string trim_copy(const string& string_to_trim);

string toupper_copy(const string& lowercase);

} /* namespace license */

#endif /* STRINGUTILS_H_ */
