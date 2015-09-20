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

#ifndef _FASTCGI_DETAILS_PARSER_H_
#define _FASTCGI_DETAILS_PARSER_H_

#include <map>
#include <string>
#include <vector>
#include <iosfwd>

// // // #include "settings.h"
#include "fastcgi3/util.h"
#include "fastcgi3/range.h"
#include "fastcgi3/functors.h"

namespace fastcgi
{

class File;
class Logger;
class Request;

class Parser {
public:
	Parser(const Parser&) = delete;
	Parser& operator=(const Parser&) = delete;

	static const char* statusToString(short status);
	static std::string getBoundary(const Range &range);
	
	static void addCookie(Request *req, const Range &range);
	static void addHeader(Request *req, const Range &key, const Range &value);

	static void parse(Request *req, char *env[], std::shared_ptr<Logger> logger);
	static void parseCookies(Request *req, const Range &range);
	
	static void parsePart(Request *req, DataBuffer part);
	static void parseLine(DataBuffer line, DataBuffer &name, DataBuffer &filename, DataBuffer &type);
	static void parseMultipart(Request *req, DataBuffer data, const std::string &boundary);

	template<typename Map> static bool has(const Map &m, const std::string &key);
	template<typename Map> static void keys(const Map &m, std::vector<std::string> &v);
	template<typename Map> static const std::string& get(const Map &m, const std::string &key);

	static std::string normalizeInputHeaderName(const Range &range);
	static std::string normalizeOutputHeaderName(const std::string &name);

	static const Range RETURN_N_RANGE;
	static const Range RETURN_RN_RANGE;
	
	static const Range EMPTY_LINE_NN_RANGE;
	static const Range EMPTY_LINE_RNRN_RANGE;
	
	static const Range NAME_RANGE;
	static const Range FILENAME_RANGE;
	
	static const Range HEADER_RANGE;
	static const Range COOKIE_RANGE;
	static const Range CONTENT_TYPE_RANGE;
	static const Range CONTENT_LENGTH_RANGE;
};

template<typename Map> inline bool
Parser::has(const Map &m, const std::string &key) {
	return m.find(key) != m.end();
}

template<typename Map> inline void
Parser::keys(const Map &m, std::vector<std::string> &v) {
	
	std::vector<std::string> tmp;
	tmp.reserve(m.size());
	
	for (typename Map::const_iterator i = m.begin(), end = m.end(); i != end; ++i) {
		tmp.push_back(i->first);
	}
	v.swap(tmp);
}

template<typename Map> inline const std::string&
Parser::get(const Map &m, const std::string &key) {
	typename Map::const_iterator i = m.find(key);
	return (m.end() == i) ? StringUtils::EMPTY_STRING : i->second;
}

} // namespace fastcgi

#endif //_FASTCGI_DETAILS_PARSER_H_
