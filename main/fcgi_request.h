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

#ifndef _FASTCGI_FASTCGI_REQUEST_H_
#define _FASTCGI_FASTCGI_REQUEST_H_

#include <sys/time.h>

#include <fcgiapp.h>
#include <fcgio.h>

#include <memory>
#include <vector>

#include "fastcgi3/request_io_stream.h"
#include "details/handlerset.h"
#include "fastcgi3/session_manager.h"

namespace fastcgi
{

class Endpoint;
class Logger;
class Request;
class ResponseTimeStatistics;

class FastcgiRequest : public RequestIOStream {
public:
    FastcgiRequest(std::shared_ptr<Request> request,
    		std::shared_ptr<Endpoint> endpoint,
			std::shared_ptr<Logger> logger,
			std::shared_ptr<ResponseTimeStatistics> statistics,
			const bool logTimes);
    virtual ~FastcgiRequest();
    void attach();
	int accept();
	void finish();

	int read(char *buf, int size);
	int write(const char *buf, int size);
	void write(std::streambuf *buf);

	void setHandlerDesc(const HandlerSet::HandlerDescription *handler);

private:
	std::shared_ptr<Request> request_;
	std::shared_ptr<Logger> logger_;
    std::string url_;
    std::string request_id_;
    std::shared_ptr<Endpoint> endpoint_;
    FCGX_Request fcgiRequest_;
    std::shared_ptr<ResponseTimeStatistics> statistics_;
	const bool logTimes_;
    timeval accept_time_, finish_time_;
    const HandlerSet::HandlerDescription* handler_;
};

} // namespace fastcgi

#endif // _FASTCGI_FASTCGI_REQUEST_H_
