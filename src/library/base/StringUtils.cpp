/*
 * StringUtils.cpp
 *
 *  Created on: Apr 8, 2014
 *      
 */

#include <cctype> //toupper
#include "StringUtils.h"
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <stdexcept>

#ifdef _WIN32
#include <time.h> //mktime under windows
#endif

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
	std::transform(cp.begin(), cp.end(), cp.begin(), (int (*)(int))toupper);
	return cp;
}

time_t seconds_from_epoch(const char* timeString) {
	int year, month, day;
	tm tm;
	if (strlen(timeString) == 8) {
		int nfield = sscanf(timeString, "%4d%2d%2d", &year, &month, &day);
		if (nfield != 3) {
			throw invalid_argument("Date not recognized");
		}
	} else if (strlen(timeString) == 10) {
		int nfield = sscanf(timeString, "%4d-%2d-%2d", &year, &month, &day);
		if (nfield != 3) {
			int nfield = sscanf(timeString, "%4d/%2d/%2d", &year, &month, &day);
			if (nfield != 3) {
				throw invalid_argument("Date not recognized");
			}
		}
	} else{
		throw invalid_argument("Date not recognized");
	}
	tm.tm_isdst = -1;
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	tm.tm_yday = -1;
	tm.tm_wday = -1;
	return mktime(&tm);
}
} /* namespace license */
