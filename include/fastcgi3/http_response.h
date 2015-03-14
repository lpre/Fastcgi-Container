// Fastcgi Container - framework for development of high performance FastCGI applications in C++
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

#ifndef INCLUDE_FASTCGI3_HTTP_RESPONSE_H_
#define INCLUDE_FASTCGI3_HTTP_RESPONSE_H_

#include <memory>

#include "fastcgi3/component.h"
#include "fastcgi3/stream.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/attributes_holder.h"
#include "fastcgi3/session.h"
#include "fastcgi3/cookie.h"

namespace fastcgi
{

class HttpResponse;

class HttpResponseStream : virtual public RequestStream {
public:
	HttpResponseStream(std::shared_ptr<HttpResponse> resp);
	virtual ~HttpResponseStream();

	HttpResponseStream(const HttpResponseStream&) = delete;
	HttpResponseStream& operator=(const HttpResponseStream&) = delete;
};

class HttpResponse {
private:
	friend class HttpResponseStream;

public:
	HttpResponse(fastcgi::Request *req);
	virtual ~HttpResponse() {}

	HttpResponse(const HttpResponse&) = delete;
	HttpResponse& operator=(const HttpResponse&) = delete;

    void setCookie(const Cookie &cookie);
    void setStatus(unsigned short status);
    void sendError(unsigned short status);
    void setHeader(const std::string &name, const std::string &value);

    void write(std::streambuf *buf);
    std::streamsize write(const char *buf, std::streamsize size);
    std::string outputHeader(const std::string &name) const;

    void redirectBack();
    void redirectToPath(const std::string &path);
    void forwardToPath(const std::string &path);

    void setContentType(const std::string &type);
    void setContentEncoding(const std::string &encoding);

private:
	Request *req_;
	HandlerContext *handlerContext_;
};


}

#endif /* INCLUDE_FASTCGI3_HTTP_RESPONSE_H_ */
