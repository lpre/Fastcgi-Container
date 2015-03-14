// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
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

#include "settings.h"

#include <cstdlib>
#include <stdexcept>
#include <limits>

#include <openssl/md5.h>
#include <uuid/uuid.h>
#include <sys/stat.h>
#include <dirent.h>

#include "fastcgi3/util.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/range.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

const std::string StringUtils::EMPTY_STRING;

StringUtils::StringUtils() {
}

StringUtils::~StringUtils() {
}

std::string
StringUtils::urlencode(const Range &range) {
	std::string result;
	result.reserve(3 * range.size());
	
	for (const char* i = range.begin(), *end = range.end(); i != end; ++i) {
		char symbol = (*i);
		if (isalnum(symbol)) {
			result.append(1, symbol);
			continue;
		}
		switch (symbol) {
			case '-': case '_': case '.': case '!': case '~': 
			case '*': case '(': case ')': case '\'': 
				result.append(1, symbol);
				break;
			default:
				result.append(1, '%');
				char bytes[3] = { 0, 0, 0 };
				bytes[0] = (symbol & 0xF0) / 16 ;
				bytes[0] += (bytes[0] > 9) ? 'A' - 10 : '0';
				bytes[1] = symbol & 0x0F;
				bytes[1] += (bytes[1] > 9) ? 'A' - 10 : '0';
				result.append(bytes, sizeof(bytes) - 1);
				break;
		}
	}
	return result;
}

void
StringUtils::urldecode(const Range &range, std::string &result) {
	for (const char *i = range.begin(), *end = range.end(); i != end; ++i) {
		switch (*i) {
			case '+':
				result.append(1, ' ');
				break;
			case '%':
				if (std::distance(i, end) > 2) {
					int digit;
					char f = *(i + 1), s = *(i + 2);
					digit = (f >= 'A' ? ((f & 0xDF) - 'A') + 10 : (f - '0')) * 16;
					digit += (s >= 'A') ? ((s & 0xDF) - 'A') + 10 : (s - '0');
					result.append(1, static_cast<char>(digit));
					i += 2;
				}
				else {
					result.append(1, '%');
				}
				break;
			default:
				result.append(1, (*i));
				break;
		}
	}
}

std::string
StringUtils::urldecode(const Range &range) {
	std::string result;
	result.reserve(range.size());
	urldecode(range, result);
	return result;
}

std::string
StringUtils::urldecode(DataBuffer data) {
	std::string result;
	result.reserve(data.size());
	for (const auto& chunk : data) {
		urldecode(Range(chunk.first, chunk.first + chunk.second), result);
	}
	return result;
}

void
StringUtils::parse(const Range &range, std::vector<NamedValue> &v) {
	Range tmp = range;
	while (!tmp.empty()) {
		Range key, value, head, tail;
		
		tmp.split('&', head, tail);
		head.split('=', key, value);
		if (!key.empty()) {
			try {
				v.push_back(std::pair<std::string, std::string>(urldecode(key), urldecode(value)));
			}
			catch (const std::exception &e) {
				throw;
				// do we really want to swallow this exception here ???
				// log()->error("%s, caught exception: %s", e.what());
			}
		}
		tmp = tail;
	}
}

void
StringUtils::parse(DataBuffer data, std::vector<NamedValue> &v) {
	DataBuffer tmp = data;
	while (!tmp.empty()) {
		DataBuffer key, value, head, tail;
		tmp.split('&', head, tail);
		head.split('=', key, value);
		if (!key.empty()) {
			try {
				v.push_back(std::pair<std::string, std::string>(urldecode(key), urldecode(value)));
			}
			catch (const std::exception &e) {
				throw;
			}
		}
		tmp = tail;
	}
}

void
StringUtils::parse(const std::string &str, std::vector<NamedValue> &v) {
	parse(Range::fromString(str), v);
}

std::string
StringUtils::urlencode(const std::string &str) {
	return urlencode(Range::fromString(str));
}
	
std::string
StringUtils::urldecode(const std::string &str) {
	return urldecode(Range::fromString(str));
}

void
StringUtils::split(const std::string& str, char c, std::vector<std::string> &v) {
	const char *s = str.c_str();
	do {
		if (*s == c) {
			continue;
		}
		const char *begin = s;
		while (*s != c && *s) {
			s++;
		}
		v.push_back(std::string(begin, s));
	} while (0 != *s++);
}

HttpDateUtils::HttpDateUtils() {
}

HttpDateUtils::~HttpDateUtils() {
}

std::string
HttpDateUtils::format(time_t value) {
	struct tm ts;
	memset(&ts, 0, sizeof(struct tm));

	if (nullptr != gmtime_r(&value, &ts)) {
		char buf[255];
		int res = strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", &ts);
		if (0 != res) {
			return std::string(buf, buf + res);
		}
	}
	throw std::runtime_error("failed to format date");
}

time_t
HttpDateUtils::parse(const char *value) {
	struct tm ts;
	memset(&ts, 0, sizeof(struct tm));
	
	const char *formats[] = { "%a, %d %b %Y %T GMT", "%A, %d-%b-%y %T GMT", "%a %b %d %T %Y" };
	for (unsigned int i=0, end=sizeof(formats)/sizeof(const char*); i<end; ++i) {
		if (nullptr != strptime(value, formats[i], &ts)) {
			return mktime(&ts) - timezone;
		}
	}
	return static_cast<time_t>(0);
}

std::string
HashUtils::hexMD5(const char *key, unsigned long len) {
    MD5_CTX md5handler;
    unsigned char md5buffer[16];

    MD5_Init(&md5handler);
    MD5_Update(&md5handler, (unsigned char *)key, len);
    MD5_Final(md5buffer, &md5handler);

    char alpha[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    unsigned char c;
    std::string md5digest;
    md5digest.reserve(32);

    for (int i = 0; i < 16; ++i) {
        c = (md5buffer[i] & 0xf0) >> 4;
        md5digest.push_back(alpha[c]);
        c = (md5buffer[i] & 0xf);
        md5digest.push_back(alpha[c]);
    }

    return md5digest;
}


static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char reverse_table[128] = {
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

std::string
HashUtils::base64_encode(const std::string &bindata) {
	if (bindata.size() > (std::numeric_limits<std::string::size_type>::max() / 4u) * 3u) {
		throw std::length_error("Converting too large a string to base64.");
	}

	const std::size_t binlen = bindata.size();
	// Use = signs so the end is properly padded.
	std::string retval((((binlen + 2) / 3) * 4), '=');
	std::size_t outpos = 0;
	int bits_collected = 0;
	unsigned int accumulator = 0;

	for (auto &c : bindata) {
		accumulator = (accumulator << 8) | (c & 0xffu);
		bits_collected += 8;
		while (bits_collected >= 6) {
			bits_collected -= 6;
			retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
		}
	}
	if (bits_collected > 0) { // Any trailing bits that are missing.
		accumulator <<= 6 - bits_collected;
		retval[outpos++] = b64_table[accumulator & 0x3fu];
	}
	return retval;
}

std::string
HashUtils::base64_decode(const std::string &ascdata) {
	std::string retval;
	const std::string::const_iterator last = ascdata.end();
	int bits_collected = 0;
	unsigned int accumulator = 0;

	for (auto &c : ascdata) {
		if (std::isspace(c) || c == '=') {
			// Skip whitespace and padding. Be liberal in what you accept.
			continue;
		}
		if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
			throw std::invalid_argument("This contains characters not legal in a base64 encoded string.");
		}
		accumulator = (accumulator << 6) | reverse_table[c];
		bits_collected += 6;
		if (bits_collected >= 8) {
			bits_collected -= 8;
			retval += (char)((accumulator >> bits_collected) & 0xffu);
		}
	}
	return retval;
}

std::string
UUIDUtils::getNewId() {
	uuid_t u;
//	uuid_generate_random(u);
	uuid_generate_time_safe(u);
	char uuid_str[37];
	uuid_unparse_lower(u, uuid_str);

	return std::string(uuid_str);
}


void
FileSystemUtils::createDirectories(const std::string& path) {
	std::vector<std::string> dirs;
	fastcgi::StringUtils::split(path, '/', dirs);

	std::string newFolder = "";
	for (auto& dir : dirs) {
		newFolder += "/"+dir;
		mkdir(newFolder.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
	}
}


} // namespace fastcgi
