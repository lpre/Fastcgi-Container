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

#ifndef _FASTCGI_DETAILS_REQUEST_THREAD_POOL_H_
#define _FASTCGI_DETAILS_REQUEST_THREAD_POOL_H_

#include <chrono>
#include <functional>

#include "fastcgi3/request.h"
#include "fastcgi3/request_io_stream.h"

#include "details/response_time_statistics.h"
#include "details/thread_pool.h"

namespace fastcgi {

class Filter;
class Handler;
class Logger;

struct RequestTask {
	std::shared_ptr<Request> request;
	std::vector<std::shared_ptr<Filter>> filters;
	std::vector<std::shared_ptr<Handler>> handlers;
	std::function<std::vector<std::shared_ptr<Handler>>(RequestTask)> futureHandlers;
	std::function<void(RequestTask)> dispatch;
	std::shared_ptr<RequestIOStream> request_stream;
	std::chrono::steady_clock::time_point start;
};

class RequestsThreadPool : public ThreadPool<RequestTask> {
public:
	RequestsThreadPool(const unsigned threadsNumber, const unsigned queueLength, std::shared_ptr<fastcgi::Logger> logger);
	RequestsThreadPool(const unsigned threadsNumber, const unsigned queueLength, std::chrono::milliseconds delay, std::shared_ptr<fastcgi::Logger> logger);
	virtual ~RequestsThreadPool();
	virtual void handleTask(RequestTask task);
	std::chrono::milliseconds delay() const;
private:
	std::shared_ptr<fastcgi::Logger> logger_;
	std::chrono::milliseconds delay_;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_REQUEST_THREAD_POOL_H_
