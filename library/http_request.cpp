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

#include <functional>

#include "core/any.hpp"

#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/util.h"
#include "fastcgi3/http_request.h"
#include "details/component_context.h"


namespace fastcgi
{

HttpRequest::HttpRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext)
: req_(req), handlerContext_(handlerContext) {
}

HttpRequest::~HttpRequest() {
}

unsigned short
HttpRequest::getServerPort() const {
	return req_->getServerPort();
}

const std::string&
HttpRequest::getHost() const {
	return req_->getHost();
}

const std::string&
HttpRequest::getServerAddr() const {
	return req_->getServerAddr();
}

const std::string&
HttpRequest::getPathInfo() const {
	return req_->getPathInfo();
}

const std::string&
HttpRequest::getPathTranslated() const {
	return req_->getPathTranslated();
}

const std::string&
HttpRequest::getScriptName() const {
	return req_->getScriptName();
}

const std::string&
HttpRequest::getScriptFilename() const {
	return req_->getScriptFilename();
}

const std::string&
HttpRequest::getDocumentRoot() const {
	return req_->getDocumentRoot();
}

const std::string&
HttpRequest::getRemoteUser() const {
	return req_->getRemoteUser();
}

const std::string&
HttpRequest::getRemoteAddr() const {
	return req_->getRemoteAddr();
}

const std::string&
HttpRequest::getQueryString() const {
	return req_->getQueryString();
}

const std::string&
HttpRequest::getRequestMethod() const {
	return req_->getRequestMethod();
}

const std::string&
HttpRequest::getRequestId() const {
	return req_->getRequestId();
}

std::streamsize
HttpRequest::getContentLength() const {
	return req_->getContentLength();
}

const std::string&
HttpRequest::getContentType() const {
	return req_->getContentType();
}

std::string
HttpRequest::getURI() const {
	return req_->getURI();
}

std::string
HttpRequest::getUrl() const {
	return req_->getUrl();
}

unsigned int
HttpRequest::countArgs() const {
	return req_->countArgs();
}

bool
HttpRequest::hasArg(const std::string &name) const {
	return req_->hasArg(name);
}

const std::string&
HttpRequest::getArg(const std::string &name) const {
	return req_->getArg(name);
}

void
HttpRequest::getArg(const std::string &name, std::vector<std::string> &v) const {
	return req_->getArg(name, v);
}

void
HttpRequest::argNames(std::vector<std::string> &v) const {
	return req_->argNames(v);
}

unsigned int
HttpRequest::countHeaders() const {
	return req_->countHeaders();
}

bool
HttpRequest::hasHeader(const std::string &name) const {
	return req_->hasHeader(name);
}

const std::string&
HttpRequest::getHeader(const std::string &name) const {
	return req_->getHeader(name);
}

void
HttpRequest::headerNames(std::vector<std::string> &v) const {
	return req_->headerNames(v);
}

unsigned int
HttpRequest::countCookie() const {
	return req_->countCookie();
}

bool
HttpRequest::hasCookie(const std::string &name) const {
	return req_->hasCookie(name);
}

const std::string&
HttpRequest::getCookie(const std::string &name) const {
	return req_->getCookie(name);
}

void
HttpRequest::cookieNames(std::vector<std::string> &v) const {
	return req_->cookieNames(v);
}

unsigned int
HttpRequest::countRemoteFiles() const {
	return req_->countRemoteFiles();
}

bool
HttpRequest::hasRemoteFile(const std::string &name) const {
	return req_->hasRemoteFile(name);
}

bool
HttpRequest::hasFile(const std::string &name) const {
	return req_->hasFile(name);
}

void
HttpRequest::remoteFiles(std::vector<std::string> &v) const {
	return req_->remoteFiles(v);
}

const std::string&
HttpRequest::remoteFileName(const std::string &name) const {
	return req_->remoteFileName(name);
}

const std::string&
HttpRequest::remoteFileType(const std::string &name) const {
	return req_->remoteFileType(name);
}

DataBuffer
HttpRequest::remoteFile(const std::string &name) const {
	return req_->remoteFile(name);
}

bool
HttpRequest::isSecure() const {
	return req_->isSecure();
}

DataBuffer
HttpRequest::requestBody() const {
	return req_->requestBody();
}

std::shared_ptr<Session>
HttpRequest::createSession() {
	return std::move(req_->createSession());
}

std::shared_ptr<Session>
HttpRequest::getSession() {
	return std::move(req_->getSession());
}

std::shared_ptr<security::Subject>
HttpRequest::getSubject() const {
	return std::move(req_->getSubject());
}

bool
HttpRequest::isUserInRole(const std::string& roleName) const {
	return req_->isUserInRole(roleName);
}

void
HttpRequest::setAttribute(const std::string &name, const core::any &value) {
	dynamic_cast<AttributesHolder*>(handlerContext_)->setAttribute(name, value);
}

core::any
HttpRequest::getAttribute(const std::string &name) const {
	return dynamic_cast<AttributesHolder*>(handlerContext_)->getAttribute(name);
}

bool
HttpRequest::hasAttribute(const std::string &name) {
	return dynamic_cast<AttributesHolder*>(handlerContext_)->hasAttribute(name);
}

void
HttpRequest::removeAttribute(const std::string& name) {
	dynamic_cast<AttributesHolder*>(handlerContext_)->removeAttribute(name);
}

void HttpRequest::removeAllAttributes() {
	dynamic_cast<AttributesHolder*>(handlerContext_)->removeAllAttributes();
}

} //namespace fastcgi



