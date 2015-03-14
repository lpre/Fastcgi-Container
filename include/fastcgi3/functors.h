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

#ifndef _FASTCGI_DETAILS_FUNCTORS_H_
#define _FASTCGI_DETAILS_FUNCTORS_H_

#include <cctype>
#include <string>
#include <cstring>
#include <algorithm>
#include <functional>

#include "fastcgi3/range.h"

namespace fastcgi
{

struct CharCILess : public std::binary_function<char, char, bool> {
	bool operator () (char c, char target) const {
		return tolower(c) < tolower(target);
	}
};

struct RangeCILess : public std::binary_function<const Range&, const Range&, bool> {
	bool operator() (const Range &range, const Range &target) const {
		return std::lexicographical_compare(range.begin(), range.end(), target.begin(), target.end(), CharCILess());
	}
};

struct StringCILess : public std::binary_function<const std::string&, const std::string&, bool> {
	bool operator () (const std::string& str, const std::string& target) const {
		return std::lexicographical_compare(str.begin(), str.end(), target.begin(), target.end(), CharCILess());
	}
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_FUNCTORS_H_
