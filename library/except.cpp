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

#include <cstdio>

#include "fastcgi3/except.h"
#include "details/parser.h"

#ifdef HAVE_LDMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

HttpException::HttpException(unsigned short status)
: status_(status) {
}

HttpException::~HttpException() noexcept {
}

unsigned short
HttpException::status() const {
	return status_;
}

const char*
HttpException::what() const noexcept {
	return Parser::statusToString(status_);
}

BadMethod::BadMethod(const char *reason)
: HttpException(400) {
	snprintf(reason_, sizeof(reason_), "%s", reason);
}

const char*
BadMethod::reason() const {
	return reason_;
}

InternalError::InternalError()
: HttpException(500) {
}

NotImplemented::NotImplemented()
: HttpException(501) {
}

NotFound::NotFound()
: HttpException(404) {
}

Forbidden::Forbidden()
: HttpException(403) {
}

Unauthorized::Unauthorized()
: HttpException(401) {
}

DispatchException::DispatchException(DispatchException::DispatchType type)
: type_(type), url_() {
}

DispatchException::DispatchException(DispatchException::DispatchType type, const std::string &url)
: type_(type), url_(url) {
}

DispatchException::DispatchException(DispatchType type, DataBuffer buffer)
: type_(type), url_(), buffer_(buffer) {
}

DispatchException::~DispatchException() noexcept {
}

DispatchException::DispatchType
DispatchException::type() const {
	return type_;
}

const std::string&
DispatchException::url() const noexcept {
	return url_;
}

DataBuffer
DispatchException::buffer() const noexcept {
	return buffer_;
}

} // namespace fastcgi
