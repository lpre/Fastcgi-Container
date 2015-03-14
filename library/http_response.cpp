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

#include <functional>

#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/util.h"
#include "fastcgi3/http_response.h"
#include "details/component_context.h"
#include "details/server.h"


namespace fastcgi
{

HttpResponse::HttpResponse(fastcgi::Request *req)
: req_(req) {
}

void
HttpResponse::setCookie(const Cookie &cookie) {
	req_->setCookie(cookie);
}

void
HttpResponse::setStatus(unsigned short status) {
	req_->setStatus(status);
}

void
HttpResponse::sendError(unsigned short status) {
	req_->sendError(status);
}

void
HttpResponse::setHeader(const std::string &name, const std::string &value) {
	req_->setHeader(name, value);
}

void
HttpResponse::write(std::streambuf *buf) {
	req_->write(buf);
}

std::streamsize
HttpResponse::write(const char *buf, std::streamsize size) {
	return req_->write(buf, size);
}

std::string
HttpResponse::outputHeader(const std::string &name) const {
	return req_->outputHeader(name);
}

void
HttpResponse::redirectBack() {
	req_->redirectBack();
}

void
HttpResponse::redirectToPath(const std::string &path) {
	req_->redirectToPath(path);
}

void
HttpResponse::forwardToPath(const std::string &path) {
	req_->forwardToPath(path);
}

void
HttpResponse::setContentType(const std::string &type) {
	req_->setContentType(type);
}

void
HttpResponse::setContentEncoding(const std::string &encoding) {
	req_->setContentEncoding(encoding);
}


HttpResponseStream::HttpResponseStream(std::shared_ptr<HttpResponse> resp)
: RequestStream(resp->req_) {
}

HttpResponseStream::~HttpResponseStream() {
}


} //namespace fastcgi



