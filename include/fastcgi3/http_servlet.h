// Fastcgi Container - framework for development of high performance FastCGI applications in C++
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

#ifndef INCLUDE_FASTCGI3_HTTP_SERVLET_H_
#define INCLUDE_FASTCGI3_HTTP_SERVLET_H_

#include <memory>

#include "fastcgi3/component.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/attributes_holder.h"
#include "fastcgi3/session.h"
#include "fastcgi3/http_request.h"
#include "fastcgi3/http_response.h"

namespace fastcgi
{

class Globals;

class Servlet : public fastcgi::Component, public fastcgi::Handler {
public:
	Servlet(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~Servlet();

	Servlet(const Servlet&) = delete;
	Servlet& operator=(const Servlet&) = delete;

	virtual void onLoad() override;
	virtual void onUnload() override;

	const std::string getParam(const std::string& name);
	const std::string getParam(const std::string& name, const std::string& defValue);

	virtual void handleRequest(fastcgi::Request* req, fastcgi::HandlerContext* handlerContext) override;
	virtual void handleRequest(HttpRequest* httpReq, HttpResponse* httpResp);
	virtual void doHead(HttpRequest* httpReq, HttpResponse* httpResp);
	virtual void doGet(HttpRequest* httpReq, HttpResponse* httpResp);
	virtual void doPost(HttpRequest* httpReq, HttpResponse* httpResp);
	virtual void doPut(HttpRequest* httpReq, HttpResponse* httpResp);
	virtual void doDelete(HttpRequest* httpReq, HttpResponse* httpResp);
	virtual void doOptions(HttpRequest* httpReq, HttpResponse* httpResp);
	virtual void doTrace(HttpRequest* httpReq, HttpResponse* httpResp);

protected:
	virtual const Globals* globals() const;
	virtual std::shared_ptr<Logger> logger() const;

private:
	const Globals* globals_;
	std::shared_ptr<fastcgi::Logger> logger_;

};

}

#endif /* INCLUDE_FASTCGI3_HTTP_SERVLET_H_ */
