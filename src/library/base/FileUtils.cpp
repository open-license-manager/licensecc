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

#include "FileUtils.hpp"

namespace license {
using namespace std;

vector<string> filter_existing_files(const vector<string> &fileList) {
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

string get_file_contents(const char *filename, size_t max_size) {
	ifstream in(filename, std::ios::binary);
	if (in) {
		string contents;
		size_t index = in.seekg(0, ios::end).tellg();
		size_t limited_size = min(index, max_size);
		contents.resize(limited_size);
		in.seekg(0, ios::beg);
		in.read(&contents[0], limited_size);
		return contents;
	}
	throw(errno);
}

}
