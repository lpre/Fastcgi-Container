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

#ifndef _FASTCGI_STATISTICS_RESPONSE_TIME_HANDLER_H_
#define _FASTCGI_STATISTICS_RESPONSE_TIME_HANDLER_H_

#include <string>
#include <mutex>

#include "fastcgi3/component.h"
#include "fastcgi3/handler.h"

#include "details/response_time_statistics.h"

namespace fastcgi
{

class CounterData {
public:
	CounterData();
	void add(std::uint64_t time);
	std::uint64_t min() const;
	std::uint64_t max() const;
	std::uint64_t avg() const;
	std::uint64_t hits() const;

private:
	std::uint64_t min_;
	std::uint64_t max_;
	std::uint64_t total_;
	std::uint64_t hits_;
};

class ResponseTimeHandler : virtual public Handler, virtual public Component,
	virtual public ResponseTimeStatistics {
public:
	ResponseTimeHandler(std::shared_ptr<ComponentContext> context);
    virtual ~ResponseTimeHandler();

    virtual void onLoad();
    virtual void onUnload();

    virtual void handleRequest(Request *req, HandlerContext *handlerContext);
	virtual void add(const std::string &handler, unsigned short status, std::uint64_t time);

private:
	std::mutex mutex_;
	using CounterMapType = std::map<unsigned short, std::shared_ptr<CounterData>>;
	std::map<std::string, CounterMapType> data_;
};

} // namespace fastcgi

#endif // _FASTCGI_STATISTICS_RESPONSE_TIME_HANDLER_H_
