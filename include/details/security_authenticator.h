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

#ifndef fastcgi3_SECURITY_AUTHENTICATOR_H_
#define fastcgi3_SECURITY_AUTHENTICATOR_H_

#include <string>
#include <memory>

#include "fastcgi3/request.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/security_subject.h"
#include "fastcgi3/security_realm.h"
#include "details/request_filter.h"

namespace fastcgi
{

namespace security
{

class SecurityConstraint : public fastcgi::UrlFilter {
public:
	SecurityConstraint(const std::string &regex, const std::string& url_prefix, const std::string& roleName);
    ~SecurityConstraint();

    const std::string& getRole() const;

private:
    std::string roleName_;
};

class Authenticator: virtual public fastcgi::Filter {
public:
	Authenticator();
	virtual ~Authenticator();

	virtual void doFilter(fastcgi::Request *request, fastcgi::HandlerContext *context, std::function<void(fastcgi::Request *request, fastcgi::HandlerContext *context)> next) override;

	virtual bool authenticate(fastcgi::Request* request) const = 0;

protected:
	virtual bool isAuthenticationRequired(fastcgi::Request* request) const;
	bool isAuthenticated(fastcgi::Request* request) const;
	bool isAuthorized(fastcgi::Request* request) const;
	bool doLogin(fastcgi::Request* request, const std::string& username, const std::string& credentials) const;
	void setSubject(fastcgi::Request* request, std::shared_ptr<Subject> subject) const;

protected:
	bool changeSessionIdOnAuthentication_;
	std::shared_ptr<Realm> realm_;

	std::vector<std::shared_ptr<SecurityConstraint>> constraints_;
};

} // namespace security

} // namespace fastcgi



#endif /* fastcgi3_SECURITY_AUTHENTICATOR_H_ */
