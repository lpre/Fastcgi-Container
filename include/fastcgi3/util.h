// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
// Copyright (C) 2015 Alexander Ponomarenko <contact@propulsion-analysis.com>

// This file is part of Fastcgi Container.
//
// Fastcgi Container is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License (LGPL) as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fastcgi Container is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License (LGPL) for more details.
//
// You should have received a copy of the GNU Lesser General Public License (LGPL)
// along with Fastcgi Container. If not, see <http://www.gnu.org/licenses/>.

#ifndef _FASTCGI_UTIL_H_
#define _FASTCGI_UTIL_H_

#include <string>
#include <vector>
#include <memory>

#include <sys/stat.h>

#include "fastcgi3/data_buffer.h"

#if __cplusplus <= 201103L
// C++14 defines std::make_unique<T> in <memory>
// But current C++ compiler is not C++14
namespace std
{
	template<typename T, typename ...Args>
	unique_ptr<T> make_unique( Args&& ...args ) {
		T* p = nullptr;
		try {
			p = new T(forward<Args>(args)...);
			return unique_ptr<T>(p);
		} catch (...) {
			if (nullptr!=p) {
				delete p;
			}
		}
		return unique_ptr<T>();
	}
}
#endif

namespace fastcgi
{

class Range;

class StringUtils {
public:
	StringUtils(const StringUtils&) = delete;
	StringUtils& operator=(const StringUtils&) = delete;

	static std::string urlencode(const Range &val);
	static std::string urlencode(const std::string &val);
	
	static std::string urldecode(const Range &val);
	static std::string urldecode(DataBuffer data);
	static std::string urldecode(const std::string &val);
	
	static std::string escapeXml(const std::string &data);

	using NamedValue = std::pair<std::string, std::string>;
	
	static void parse(const Range &range, std::vector<NamedValue> &v);
	static void parse(const std::string &str, std::vector<NamedValue> &v);
	static void parse(DataBuffer data, std::vector<NamedValue> &v);

	static inline bool beginsWith(const std::string &value, const std::string &prefix) {
	    if (prefix.size() > value.size()) {
	    	return false;
	    }
	    return std::equal(prefix.begin(), prefix.end(), value.begin());
	}
	
	static inline bool endsWith(const std::string &value, const std::string &suffix) {
	    if (suffix.size() > value.size()) {
	    	return false;
	    }
	    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
	}

	static void split(const std::string& str, char c, std::vector<std::string> &v);

	static std::string error(int error);

	static const std::string EMPTY_STRING;

private:
	static void urldecode(const Range &range, std::string &result);

	StringUtils();
	virtual ~StringUtils();
};

class HttpDateUtils {
public:
	static time_t parse(const char *value);
	static std::string format(time_t value);

	HttpDateUtils(const HttpDateUtils&) = delete;
	HttpDateUtils& operator=(const HttpDateUtils&) = delete;

private:
	HttpDateUtils();
	virtual ~HttpDateUtils();
};

class HashUtils {
public:
	static std::string hexMD5(const char *key, unsigned long len);
	static std::string base64_encode(const std::string &bindata);
	static std::string base64_decode(const std::string &ascdata);
};

class UUIDUtils {
public:
	static std::string getNewId();
};

class FileSystemUtils {
public:
	static void createDirectories(const std::string& path, mode_t open_mode=S_IRUSR|S_IWUSR|S_IXUSR);
	static bool isWritable(const std::string& path);
	static std::string basename(const std::string& path);
	static std::string removeExtension(const std::string& filename);
	static std::string pathToFile(const std::string& path);
};



} // namespace fastcgi

#endif // _FASTCGI_UTIL_H_
