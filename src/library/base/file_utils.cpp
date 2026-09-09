/*
 * FileUtils.cpp
 *
 *  Created on: Oct 8, 2019
 *      Author: devel
 */

#include <fstream>
#include <string>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <cstring>
#include "file_utils.hpp"
#include "logger.h"

namespace license {
using namespace std;

vector<string> filter_existing_files(const vector<string>& fileList) {
	vector<string> existingFiles;
	for (auto it = fileList.begin(); it != fileList.end(); it++) {
		ifstream f(it->c_str());
		if (f.good()) {
			existingFiles.push_back(*it);
		}
		f.close();
	}
	return existingFiles;
}

FUNCTION_RETURN get_file_contents(const char* filename, size_t max_size, std::string& contents_out) {
	contents_out.clear();
	ifstream in(filename, std::ios::binary);
	if (!in) {
		LOG_ERROR("Error reading file [%s]: %s", filename, std::strerror(errno));
		return FUNC_RET_NOT_AVAIL;
	}
	const size_t index = static_cast<size_t>(in.seekg(0, ios::end).tellg());
	const size_t limited_size = min(index, max_size);
	contents_out.resize(limited_size);
	in.seekg(0, ios::beg);
	in.read(&contents_out[0], limited_size);
	in.close();
	return FUNC_RET_OK;
}

string remove_extension(const string& path) {
	if (path == "." || path == "..") {
		return path;
	}
	size_t dotpos = path.find_last_of(".");
	// no dot
	if (dotpos == string::npos) {
		return path;
	}
	// find the last path separator
	size_t pathsep_pos = path.find_last_of("\\/");
	if (pathsep_pos == string::npos) {
		return (dotpos == 0 ? path : path.substr(0, dotpos));
	} else if (pathsep_pos >= dotpos + 1) {
		return path;
	}
	return path.substr(0, dotpos);
}

}  // namespace license
