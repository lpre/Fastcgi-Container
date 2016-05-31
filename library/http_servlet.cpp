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

#include <memory>
#include <functional>

#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"
#include "fastcgi3/except.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/util.h"
#include "fastcgi3/http_request.h"
#include "fastcgi3/http_response.h"
#include "fastcgi3/http_servlet.h"
#include "details/component_context.h"
#include "details/server.h"
#include "details/globals.h"

namespace fastcgi
{

static const unsigned int HTTP_METHOD_HEAD 		{1};
static const unsigned int HTTP_METHOD_GET 		{2};
static const unsigned int HTTP_METHOD_POST 		{3};
static const unsigned int HTTP_METHOD_PUT 		{4};
static const unsigned int HTTP_METHOD_DELETE 	{5};
static const unsigned int HTTP_METHOD_OPTIONS 	{6};
static const unsigned int HTTP_METHOD_TRACE 	{7};
static const unsigned int HTTP_METHOD_PATCH 	{8};

static const std::map<std::string, unsigned int> http_methods_map {
	{"HEAD", 	HTTP_METHOD_HEAD},
	{"GET", 	HTTP_METHOD_GET},
	{"POST", 	HTTP_METHOD_POST},
	{"PUT", 	HTTP_METHOD_PUT},
	{"DELETE", 	HTTP_METHOD_DELETE},
	{"OPTIONS", HTTP_METHOD_OPTIONS},
	{"TRACE", 	HTTP_METHOD_TRACE},
	{"PATCH", 	HTTP_METHOD_PATCH}
};

Servlet::Servlet(std::shared_ptr<fastcgi::ComponentContext> context)
: Component(context), globals_(nullptr), logger_() {

	std::shared_ptr<ComponentContextImpl> impl = std::dynamic_pointer_cast<ComponentContextImpl>(context);
	if (!impl) {
		throw std::runtime_error("cannot fetch globals in request cache");
	}
	globals_ = impl->globals();

}

Servlet::~Servlet() {
}

void
Servlet::onLoad() {
	std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
	logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
	if (!logger_) {
		throw std::runtime_error("cannot get component " + loggerComponentName);
	}
}

void
Servlet::onUnload() {
}

const std::string
Servlet::getParam(const std::string& name) {
	return context()->getConfig()->asString(context()->getComponentXPath() + name);
}

const std::string
Servlet::getParam(const std::string& name, const std::string& defValue) {
	return context()->getConfig()->asString(context()->getComponentXPath() + name, defValue);
}

const Globals*
Servlet::globals() const {
	return globals_;
}

std::shared_ptr<Logger>
Servlet::logger() const {
	return logger_;
}

void
Servlet::handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext) {

	// Function to obtain list of handlers/servlets by URL
	// Used in function includePath within HttpResponse
	auto handlers = [this](const std::string &uri) {
		const HandlerSet::HandlerDescription* handler = globals_->handlers()->findURIHandler(uri);
		if (nullptr == handler || handler->handlers.empty()) {
			throw NotFound();
		}
		return handler->handlers;
	};

	// Function to obtain handler/servlet by component name
	// Used in function includeComponent within HttpResponse
	auto component = [this](const std::string &name) {
		std::shared_ptr<fastcgi::Component> component = context()->findComponent<fastcgi::Component>(name);
        if (!component) {
            throw std::runtime_error("Cannot find component: " + name);
        }
        std::shared_ptr<Handler> handler = std::dynamic_pointer_cast<Handler>(component);
        if (!handler) {
            throw std::runtime_error("Component " + name + " does not implement interface Handler");
        }
        return handler;
	};

	std::unique_ptr<HttpRequest> request =  std::make_unique<HttpRequest>(req, handlerContext);
	std::unique_ptr<HttpResponse> response = std::make_unique<HttpResponse>(req, handlerContext, handlers, component);
	handleRequest(request.get(), response.get());
}

void
Servlet::handleRequest(HttpRequest *httpReq, HttpResponse *httpResp) {
	const std::string &method = httpReq->getRequestMethod();

	auto it = http_methods_map.find(method);
	if (it!=http_methods_map.end()) {
		switch(it->second) {
		case HTTP_METHOD_GET:
			doGet(httpReq, httpResp);
			break;
		case HTTP_METHOD_POST:
			doPost(httpReq, httpResp);
			break;
		case HTTP_METHOD_HEAD:
			doHead(httpReq, httpResp);
			break;
		case HTTP_METHOD_PUT:
			doPut(httpReq, httpResp);
			break;
		case HTTP_METHOD_DELETE:
			doDelete(httpReq, httpResp);
			break;
		case HTTP_METHOD_OPTIONS:
			doOptions(httpReq, httpResp);
			break;
		case HTTP_METHOD_PATCH:
			doPatch(httpReq, httpResp);
			break;
		case HTTP_METHOD_TRACE:
			doTrace(httpReq, httpResp);
			break;
		default:
			httpResp->sendError(405);
			break;
		}
	} else {
		httpResp->sendError(405);
	}
}

void
Servlet::doHead(HttpRequest *httpReq, HttpResponse *httpResp) {
	httpResp->sendError(405);
}

void
Servlet::doGet(HttpRequest *httpReq, HttpResponse *httpResp) {
	httpResp->sendError(405);
}

void
Servlet::doPost(HttpRequest *httpReq, HttpResponse *httpResp) {
	httpResp->sendError(405);
}

void
Servlet::doPut(HttpRequest *httpReq, HttpResponse *httpResp) {
	httpResp->sendError(405);
}

void
Servlet::doDelete(HttpRequest *httpReq, HttpResponse *httpResp) {
	httpResp->sendError(405);
}

void
Servlet::doOptions(HttpRequest *httpReq, HttpResponse *httpResp) {
	httpResp->sendError(405);
	// Example of implementation:
	// httpResp->setHeader("Allow", "GET,POST,PUT,DELETE,OPTIONS");
}

void
Servlet::doTrace(HttpRequest *httpReq, HttpResponse *httpResp) {
	httpResp->sendError(405);
}

void
Servlet::doPatch(HttpRequest *httpReq, HttpResponse *httpResp) {
	httpResp->sendError(405);
}

} //namespace fastcgi



