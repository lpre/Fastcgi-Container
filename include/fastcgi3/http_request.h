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

#ifndef INCLUDE_FASTCGI3_HTTP_REQUEST_H_
#define INCLUDE_FASTCGI3_HTTP_REQUEST_H_

#include <memory>

#include "core/any.hpp"

#include "fastcgi3/component.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/request.h"
#include "fastcgi3/attributes_holder.h"
#include "fastcgi3/session.h"
#include "fastcgi3/cookie.h"

namespace fastcgi
{

class HttpRequest {
public:
	HttpRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext);
	virtual ~HttpRequest();

	HttpRequest(const HttpRequest&) = delete;
	HttpRequest& operator=(const HttpRequest&) = delete;

	const std::string& getEnvVariable(const std::string &name) const;

    unsigned short getServerPort() const;
    const std::string& getHost() const;
    const std::string& getServerAddr() const;

    const std::string& getPathInfo() const;
    const std::string& getPathTranslated() const;

    const std::string& getScriptName() const;
    const std::string& getScriptFilename() const;

    const std::string& getDocumentRoot() const;

    const std::string& getRemoteUser() const;
    const std::string& getRemoteAddr() const;
    const std::string& getQueryString() const;
    const std::string& getRequestMethod() const;
    const std::string& getRequestId() const;

    std::streamsize getContentLength() const;
    const std::string& getContentType() const;

    std::string getURI() const;
    std::string getUrl() const;

    unsigned int countArgs() const;
    bool hasArg(const std::string &name) const;
    const std::string& getArg(const std::string &name) const;
    void getArg(const std::string &name, std::vector<std::string> &v) const;
    void argNames(std::vector<std::string> &v) const;

    unsigned int countHeaders() const;
    bool hasHeader(const std::string &name) const;
    const std::string& getHeader(const std::string &name) const;
    void headerNames(std::vector<std::string> &v) const;

    unsigned int countCookie() const;
    bool hasCookie(const std::string &name) const;
    const std::string& getCookie(const std::string &name) const;
    void cookieNames(std::vector<std::string> &v) const;

	unsigned int countRemoteFiles() const;
	bool hasRemoteFile(const std::string &name) const;
    bool hasFile(const std::string &name) const;
    void remoteFiles(std::vector<std::string> &v) const;

    const std::string& remoteFileName(const std::string &name) const;
    const std::string& remoteFileType(const std::string &name) const;
    DataBuffer remoteFile(const std::string &name) const;

    bool isSecure() const;
    DataBuffer requestBody() const;

    std::shared_ptr<Session> createSession();
    std::shared_ptr<Session> getSession();

	std::shared_ptr<security::Subject> getSubject() const;
	bool isUserInRole(const std::string& roleName) const;

	template<class T> bool
	isUserInRole(const std::string &roleName) {
		return req_->isUserInRole<T>(roleName);
	}

	template<class T> void
	setAttribute(const std::string &name, T attr) {
		T _value = attr;
		setAttribute(name, (const core::any&)std::move(_value));
	}

	template<class T> T
	getAttribute(const std::string &name) {
		core::any attr = getAttribute(name);
		if (!attr.empty()) {
			return core::any_cast<T>(attr);
		}
		throw std::runtime_error("Attribute not found");
	}

	void setAttribute(const std::string &name, const core::any &value);
	core::any getAttribute(const std::string &name) const;
	bool hasAttribute(const std::string &name);
	void removeAttribute(const std::string& name);
	void removeAllAttributes();

private:
	Request *req_;
	HandlerContext *handlerContext_;
};

}

#endif /* INCLUDE_FASTCGI3_HTTP_REQUEST_H_ */
