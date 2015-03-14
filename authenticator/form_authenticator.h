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

#ifndef INCLUDE_FORM_AUTHENTICATOR_H_
#define INCLUDE_FORM_AUTHENTICATOR_H_

#include <string>
#include <memory>
#include <regex>

#include "fastcgi3/component.h"
#include "details/globals.h"

#include "abstract_authenticator.h"

namespace fastcgi
{

class Request;

namespace security
{

class FormAuthenticator : virtual public AbstractAuthenticator {
public:
	static const std::string COMPONENT_NAME;
	static const std::string SECURITY_CHECK_URL;
	static const std::string FORM_ACTION;
	static const std::string FORM_USERNAME;
	static const std::string FORM_PASSWORD;
protected:
	static const std::string PARAM_NAME_STORED_REQUEST;
	static const std::string PARAM_NAME_STORED_REQUEST_URI;

public:
	FormAuthenticator(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~FormAuthenticator();

	virtual void onLoad() override;
	virtual void onUnload() override;

	void doFilter(fastcgi::Request *request, fastcgi::HandlerContext *context, std::function<void(fastcgi::Request *request, fastcgi::HandlerContext *context)> next) override;

	virtual bool authenticate(fastcgi::Request* request) const override;

	std::string getSecurityCheckUrl(fastcgi::Request *request) const;
	const std::string& getFormUsername() const;
	const std::string& getFormPassword() const;

protected:
	virtual bool isAuthenticationRequired(fastcgi::Request* request) const override;

	fastcgi::DataBuffer createFileBuffer(const std::string &key) const;

private:
	std::string urlPrefix_;
	std::string securityCheckUrl_;
	std::string formAction_;

	std::string cache_dir_;
	std::uint64_t window_;

	std::regex securityCheckUrlRegex_;
	bool storeRequest_;
};


} // namespace security


} // namespace fastcgi

#endif /* INCLUDE_FORM_AUTHENTICATOR_H_ */
