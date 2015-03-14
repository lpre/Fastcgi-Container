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

#ifndef _FASTCGI_EXCEPT_H_
#define _FASTCGI_EXCEPT_H_

#include <string>
#include <exception>

#include "fastcgi3/data_buffer.h"

namespace fastcgi
{

class HttpException : public std::exception {
public:
	HttpException(unsigned short status);
	virtual ~HttpException() noexcept;
	
	unsigned short status() const;
	virtual const char* what() const noexcept;

private:
	unsigned short status_;
};

class BadMethod : public HttpException {
public:
	BadMethod(const char *reason);
	virtual const char* reason() const;
private:
	char reason_[256];
};

class InternalError : public HttpException {
public:
	InternalError();
};

class NotImplemented : public HttpException {
public:
	NotImplemented();
};

class NotFound : public HttpException {
public:
	NotFound();
};

class Forbidden : public HttpException {
public:
	Forbidden();
};

class Unauthorized : public HttpException {
public:
	Unauthorized();
};

class DispatchException : public std::exception {
public:
	enum class DispatchType {
		APPEND,
		FORWARD
	};
public:
	DispatchException(DispatchType type);
	DispatchException(DispatchType type, const std::string &url);
	DispatchException(DispatchType type, DataBuffer buffer);

	virtual ~DispatchException() noexcept;

	DispatchType type() const;
	const std::string& url() const noexcept;
	DataBuffer buffer() const noexcept;

private:
	DispatchType type_;
	std::string url_;
	DataBuffer buffer_;
};


} // namespace fastcgi

#endif // _FASTCGI_EXCEPT_H_
