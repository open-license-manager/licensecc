/*
 * StringUtils.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: devel
 */

#include <cctype> //toupper
#include "StringUtils.h"
#include <iostream>
#include <string>
#include <algorithm>

namespace license {
using namespace std;

string trim_copy(const string& string_to_trim) {
	std::string::const_iterator it = string_to_trim.begin();
	while (it != string_to_trim.end() && isspace(*it))
		it++;

	std::string::const_reverse_iterator rit = string_to_trim.rbegin();
	while (rit.base() != it && isspace(*rit))
		rit++;

	return std::string(it, rit.base());
}

string toupper_copy(const string& lowercase) {
	string cp(lowercase);
	std::transform(cp.begin(), cp.end(), cp.begin(), (int(*)(int))toupper);
	return cp;
}

} /* namespace license */
