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

#ifndef _FASTCGI_HANDLER_H_
#define _FASTCGI_HANDLER_H_

#include <functional>

#include "core/any.hpp"

#include <string>
#include <vector>
#include <map>

namespace fastcgi
{

class Request;

class HandlerContext {
public:
	virtual ~HandlerContext();

	virtual core::any getParam(const std::string &name) const = 0;
	virtual void setParam(const std::string &name, const core::any &value) = 0;
};

/*!
 *  HTTP request handler.
 *
 *  A handler object represents a component that will be invoked
 *  to generate the response for the specific URL.
 *
 *  For each URL, several handlers may be configured in configuration file,
 *  invoked sequentially in the same order as defined in configuration.
 *  Each handler in this sequence is receiving the pointer to Request with
 *  response prepared by previous handler. The current handler may either
 *  add its own response to the common response or rewrite it.
 *
 *  The pointer to the object HandlerContext can be used to store the request-specific data,
 *  and to share it between different filters and handlers.
 */
class Handler {
public:
	Handler();
	virtual ~Handler();

	Handler(const Handler&) = delete;
	Handler& operator=(const Handler&) = delete;

	virtual void onThreadStart();
	virtual void handleRequest(Request *req, HandlerContext *context) = 0;
};

/*!
 *  HTTP request filter.
 *
 *  A filter object represents a component that will be inserted into the request processing pipeline.
 *  Example of sequence diagram for 3 filters F1, F2, F3 and N handlers H1, H2, ..., HN:
 *
 *  F1    F2    F3    Handlers
 *  |---->|
 *  |     |---->|
 *  |     |     |---->|
 *  |     |     |     |---
 *  |     |     |     |   | H1
 *  |     |     |     |<--
 *  |     |     |     |---
 *  |     |     |     |   | H2
 *  |     |     |     |<--
 *  |     |     |     |    ...
 *  |     |     |     |---
 *  |     |     |     |   | HN
 *  |     |     |     |<--
 *  |     |     |<----|
 *  |     |<----|
 *  |<----|
 *  |
 *
 *  To pass the processing to the next filter and finally to the handler(s), the filter has to invoke
 *  the next function passed as a parameter "next" into the method "doFilter".
 *
 *  To break the processing sequence, the filter may return from the method "doFilter" without invoking the function "next".
 *
 *  The pointer to the object HandlerContext can be used to store the request-specific data,
 *  and to share it between different filters and handlers.
 */
class Filter {
public:
	Filter();
	virtual ~Filter();

	Filter(const Filter&) = delete;
	Filter& operator=(const Filter&) = delete;

	virtual void onThreadStart();
	virtual void doFilter(Request *req, HandlerContext *context, std::function<void(Request *req, HandlerContext *context)> next) = 0;
};


} // namespace fastcgi

#endif // _FASTCGI_HANDLER_H_
