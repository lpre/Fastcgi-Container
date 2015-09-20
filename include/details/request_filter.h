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

#ifndef _FASTCGI_DETAILS_REQUEST_FILTER_H_
#define _FASTCGI_DETAILS_REQUEST_FILTER_H_

#include <string>
#include <regex>
#include <unordered_map>
#include <mutex>

namespace fastcgi
{

class Request;

class RequestFilter {
public:
    virtual bool check(const Request *request) = 0;
};

class RegexFilter : public RequestFilter {
public:
    RegexFilter(const std::string &regex);
    virtual ~RegexFilter();

    bool check(const std::string &value);
protected:
    std::regex regex_;
//	std::unordered_map<std::string, bool> cache_;
//	mutable std::mutex mutex_;
};

class UrlFilter : public RegexFilter {
public:
    UrlFilter(const std::string &regex, const std::string &url_prefix);
    ~UrlFilter();

    virtual bool check(const Request *request) override;
private:
    std::string url_prefix_;
    std::size_t url_prefix_len_;
};

class HostFilter : public RegexFilter {
public:
    HostFilter(const std::string &regex);
    ~HostFilter();

    virtual bool check(const Request *request) override;
};

class PortFilter : public RegexFilter {
public:
    PortFilter(const std::string &regex);
    ~PortFilter();

    virtual bool check(const Request *request) override;
};

class AddressFilter : public RegexFilter {
public:
    AddressFilter(const std::string &regex);
    ~AddressFilter();

    virtual bool check(const Request *request) override;
};

class RefererFilter : public RegexFilter {
public:
    RefererFilter(const std::string &regex);
    ~RefererFilter();

    virtual bool check(const Request *request) override;
};

class ParamFilter : public RegexFilter {
public:
    ParamFilter(const std::string &name, const std::string &regex);
    ~ParamFilter();

    virtual bool check(const Request *request) override;
private:
    std::string name_;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_REQUEST_FILTER_H_
