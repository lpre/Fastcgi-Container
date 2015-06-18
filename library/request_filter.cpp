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

#include <unordered_map>

// #include "settings.h"

#include "details/request_filter.h"

#include "fastcgi3/request.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

RegexFilter::RegexFilter(const std::string &regex)
: regex_(regex.empty()?".*":regex) {
}

RegexFilter::~RegexFilter() {
}

bool
RegexFilter::check(const std::string &value) {
	return std::regex_match(value, regex_);

	// TODO: regex in C++11 is not very fast. Try to cache the results.
	// TODO: implement the limit for number of stored results

//	std::lock_guard<std::mutex> lock(mutex_);
//
//	auto it = cache_.find(value);
//	if (cache_.end()!=it) {
//		return it->second;
//	} else {
//		bool ret = std::regex_match(value, regex_);
//		cache_.insert({std::string(value), ret});
//		return ret;
//	}
}

UrlFilter::UrlFilter(const std::string &regex, const std::string &url_prefix)
: RegexFilter(regex), url_prefix_(url_prefix), url_prefix_len_(url_prefix.length()) {
}

UrlFilter::~UrlFilter() {
}

bool
UrlFilter::check(const Request *request) {
	const std::string &script_name = request->getScriptName();
	if (url_prefix_len_>0) {
		std::size_t n = script_name.find(url_prefix_);
		if (0==n) {
			return RegexFilter::check(script_name.substr(url_prefix_len_));
		}
	}
    return RegexFilter::check(script_name);
}

HostFilter::HostFilter(const std::string &regex)
: RegexFilter(regex) {
}

HostFilter::~HostFilter() {
}

bool
HostFilter::check(const Request *request) {
    return RegexFilter::check(request->getHost());
}


PortFilter::PortFilter(const std::string &regex)
: RegexFilter(regex) {
}

PortFilter::~PortFilter() {
}

bool
PortFilter::check(const Request *request) {
    std::string port = std::to_string(request->getServerPort());
    return RegexFilter::check(port);
}


AddressFilter::AddressFilter(const std::string &regex)
: RegexFilter(regex) {
}

AddressFilter::~AddressFilter() {
}

bool
AddressFilter::check(const Request *request) {
    return RegexFilter::check(request->getServerAddr());
}

RefererFilter::RefererFilter(const std::string &regex)
: RegexFilter(regex) {
}

RefererFilter::~RefererFilter() {
}

bool
RefererFilter::check(const Request *request) {
	if (!request->hasHeader("Referer")) {
		return false;
	}
    return RegexFilter::check(request->getHeader("Referer"));
}

ParamFilter::ParamFilter(const std::string &name, const std::string &regex)
: RegexFilter(regex), name_(name) {
}

ParamFilter::~ParamFilter() {
}

bool
ParamFilter::check(const Request *request) {
    if (!request->hasArg(name_)) {
        return false;
    }
    return RegexFilter::check(request->getArg(name_));
}


} // namespace fastcgi
