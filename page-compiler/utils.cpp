// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2015 Alexander Ponomarenko <contact@propulsion-analysis.com>

// This file is part of Fastcgi Container.
//
// Fastcgi Container is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fastcgi Container is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Fastcgi Container. If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include <sys/stat.h>
#include <dirent.h>
#include <system_error>

#include "utils.h"

struct MatchPathSeparator {
    bool operator()( char ch ) const {
        return ch == '\\' || ch == '/';
    }
};

std::string
basename(const std::string& path) {
    return std::string(std::find_if(path.rbegin(), path.rend(), MatchPathSeparator()).base(), path.end());
}

std::string
removeExtension(const std::string& filename) {
    auto pivot = std::find(filename.rbegin(), filename.rend(), '.');
    return pivot == filename.rend()
        ? filename
        : std::string( filename.begin(), pivot.base() - 1 );
}

std::string
pathToFile(const std::string& path) {
	std::string name = basename(path);
	return path.substr(0, path.length()-name.length());
}

void
split(const std::string& str, char c, std::vector<std::string> &result) {
	const char *s = str.c_str();
	do {
		if (*s == c) {
			continue;
		}
		const char *begin = s;
		while(*s != c && *s) {
			s++;
		}
		result.push_back(std::string(begin, s));
	} while (0 != *s++);
}

std::string
replaceAll(std::string str, const std::string& toReplace, const std::string& replaceWith) {
	size_t start_pos = 0;
	while((start_pos = str.find(toReplace, start_pos)) != std::string::npos) {
		 str.replace(start_pos, toReplace.length(), replaceWith);
		 start_pos += replaceWith.length();
	}
	return std::move(str);
}

std::string
error(int error) {
	char buffer[256];
	strerror_r(error, buffer, sizeof(buffer));
	std::string result(buffer);
	return result;
}

void
createDirectories(const std::string& path) {
	std::vector<std::string> dirs;
	split(path, '/', dirs);

	std::string newFolder = "";
	for (auto& dir : dirs) {
		newFolder += "/"+dir;
		if (0!=mkdir(newFolder.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) && EEXIST!=errno) {
			std::cerr << "Could not create directory \"" << path << "\": " << error(errno) << std::endl;
			throw std::system_error(errno, std::system_category());
		}
	}
}

bool
hasExt(const std::string& s, const std::string& ext) {
    return (s.size() >= ext.size()) && equal(ext.rbegin(), ext.rend(), s.rbegin());
}

void
listFiles(const std::string& path, const std::string& ext, std::vector<std::string> &files) {
	DIR* dirFile = opendir(path.c_str());
	if (dirFile) {
		dirent* hFile;
		errno = 0;
		while ((hFile = readdir(dirFile)) != NULL) {
			if (!strcmp(hFile->d_name, "." )) {
				continue;
			}
			if (!strcmp( hFile->d_name, "..")) {
				continue;
			}

			// in linux hidden files all start with '.'
			if (hFile->d_name[0] == '.') {
				continue;
			}

			// dirFile.name is the name of the file. Do whatever string comparison
			// you want here. Something like:
			if (hasExt(hFile->d_name, ext)) {
				files.push_back(hFile->d_name);
			}
		}
		closedir(dirFile);
	}
}
