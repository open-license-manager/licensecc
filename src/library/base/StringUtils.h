/*
 * StringUtils.h
 *
 *  Created on: Apr 8, 2014
 *      
 */

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_
#include <string>
#include <vector>

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

time_t seconds_from_epoch(const char* s);

/**
 * Split a string on a given character
 */
const vector<string> split_string(const string& licensePositions, const char splitchar);
}

/* namespace license */

#endif /* STRINGUTILS_H_ */
