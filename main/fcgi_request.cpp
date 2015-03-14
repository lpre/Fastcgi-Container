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

#include <cstring>
#include <functional>

#include "endpoint.h"
#include "fcgi_request.h"

#include "fastcgi3/logger.h"
#include "fastcgi3/request.h"

#include "fastcgi3/session_manager.h"

#include "details/response_time_statistics.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif


/*
 * A vector of pointers representing the parameters received
 * by a FastCGI application server, with the vector's length
 * and last valid element so adding new parameters is efficient.
 */
typedef struct Params {
    FCGX_ParamArray vec;    /* vector of strings */
    int length;		    /* number of string vec can hold */
    char **cur;		    /* current item in vec; *cur == nullptr */
} Params;
using ParamsPtr = Params*;

namespace fastcgi
{

static const std::string DAEMON_STRING = "fastcgi-daemon";

FastcgiRequest::FastcgiRequest(std::shared_ptr<Request> request,
		std::shared_ptr<Endpoint> endpoint,
		std::shared_ptr<Logger> logger,
		std::shared_ptr<ResponseTimeStatistics> statistics,
		const bool logTimes) :
    request_(request), logger_(logger), endpoint_(endpoint),
    statistics_(statistics), logTimes_(logTimes), handler_(nullptr)
{
    if (0 != FCGX_InitRequest(&fcgiRequest_, endpoint_->socket(), 0)) {
        throw std::runtime_error("can not init fastcgi request");
    }
}

FastcgiRequest::~FastcgiRequest() {
    std::uint64_t microsec = 0;
    if (logTimes_ || statistics_) {
        gettimeofday(&finish_time_, nullptr);

        microsec = (finish_time_.tv_sec - accept_time_.tv_sec) *
            1000000 + (finish_time_.tv_usec - accept_time_.tv_usec);
    }

    if (logTimes_) {
        double res = static_cast<double>(microsec) / 1000000.0;
        logger_->info("handling %s taken %08f seconds", url_.c_str(), res);
    }

    if (statistics_) {
        try {
            statistics_->add(handler_ ? handler_->id : DAEMON_STRING, request_->status(), microsec);
        }
        catch (const std::exception &e) {
            logger_->error("Exception caught while update statistics: %s", e.what());
        }
        catch (...) {
            logger_->error("Unknown exception caught while update statistics");
        }
    }

    FCGX_Finish_r(&fcgiRequest_);
}

void
FastcgiRequest::attach() {

    char **envp = fcgiRequest_.envp;
    for (std::size_t i = 0; envp[i]; ++i) {
        if (0 == strncasecmp(envp[i], "REQUEST_URI=", sizeof("REQUEST_URI=") - 1)) {
            url_.assign(envp[i] + sizeof("REQUEST_URI=") - 1);
        }
        if (0 == strncasecmp(envp[i], "REQUEST_ID=", sizeof("REQUEST_ID=") - 1)) {
            request_id_.assign(envp[i] + sizeof("REQUEST_ID=") - 1);
        }
    }

    std::shared_ptr<LoggerRequestId> logger_req_id = std::dynamic_pointer_cast<LoggerRequestId>(logger_);
    if (logger_req_id) {
		logger_req_id->setRequestId(request_id_);
    }

    if (0==fcgiRequest_.keepConnection) {
    	request_->setHeader("Connection", "close");
    }

    request_->attach(this, fcgiRequest_.envp);
}

int
FastcgiRequest::accept() {
    int status = FCGX_Accept_r(&fcgiRequest_);

    if (status >= 0) {
    	// Apache mod_proxy_fcgi does not keep connection
        fcgiRequest_.keepConnection = endpoint_->getKeepConnection();

        if (logTimes_ || statistics_) {
            gettimeofday(&accept_time_, nullptr);
        }
    }

    return status;
}

void
FastcgiRequest::finish() {
    FCGX_Finish_r(&fcgiRequest_);
}

int
FastcgiRequest::read(char *buf, int size) {
    return FCGX_GetStr(buf, size, fcgiRequest_.in);
}

static void
generateRequestInfo(const Request *request, std::stringstream &str) {
    str << "Url: " << request->getUrl();
    if ("POST" != request->getRequestMethod() && "PUT" != request->getRequestMethod()) {
        return;
    }
    std::string result;
    result.reserve(2048);
    std::vector<std::string> names;
    request->argNames(names);
    for (auto& name : names) {
        std::vector<std::string> args;
        request->getArg(name, args);

        for (auto& e : args) {
            if (!result.empty()) {
                result.push_back('&');
            }
            result.append(name);
            result.push_back('=');
            if (result.size() + e.size() > 1024) {
                str << ". Args: " << result << "...";
                return;
            }
            result.append(e);
        }

    }
    str << ". Args: " << result;
}

int
FastcgiRequest::write(const char *buf, int size) {
    int num = FCGX_PutStr(buf, size, fcgiRequest_.out);
    if (-1 == num) {
        std::stringstream str;
        int error = FCGX_GetError(fcgiRequest_.out);
        if (error > 0) {
            char buffer[256];
            str << "Cannot write data to fastcgi socket: " << strerror_r(error, buffer, sizeof(buffer)) << ". ";
        } else {
            str << "FastCGI error. ";
        }
        generateRequestInfo(request_.get(), str);
        throw std::runtime_error(str.str());
    }
    return num;
}

void
FastcgiRequest::write(std::streambuf *buf) {
    std::vector<char> outv(4096);
    fcgi_streambuf outbuf(fcgiRequest_.out, &outv[0], outv.size());
    std::ostream os(&outbuf);
    os << buf;
}

void
FastcgiRequest::setHandlerDesc(const HandlerSet::HandlerDescription *handler) {
    handler_ = handler;
}

} // namespace fastcgi
