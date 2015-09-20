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

#include "fastcgi3/request.h"
#include "fastcgi3/except.h"
#include "fastcgi3/security_subject.h"
#include "fastcgi3/security_realm.h"
#include "details/security_authenticator.h"
#include "details/request_filter.h"

namespace fastcgi
{

namespace security
{

SecurityConstraint::SecurityConstraint(const std::string &regex, const std::string& url_prefix, const std::string& roleName)
: fastcgi::UrlFilter(regex, url_prefix), roleName_(roleName) {
}

SecurityConstraint::~SecurityConstraint() {
}

const std::string&
SecurityConstraint::getRole() const {
	return roleName_;
}

Authenticator::Authenticator()
: changeSessionIdOnAuthentication_(true), realm_() {
}

Authenticator::~Authenticator() {
	constraints_.clear();
}

void
Authenticator::doFilter(fastcgi::Request *request, fastcgi::HandlerContext *context, std::function<void(fastcgi::Request *request, fastcgi::HandlerContext *context)> next) {
	if (!isAuthenticationRequired(request)) {
		// Authentication is not required: process the request
		next(request, context);
		return;
	}

	if (isAuthenticated(request) || authenticate(request)) {
		// Authenticated
		if (isAuthorized(request)) {
			// Authorized
			next(request, context);
		} else {
			// Unauthorized
			request->setSubject(Subject::getAnonymousSubject());
			throw fastcgi::Unauthorized();
			// request->sendError(401);
		}
		request->setSubject(Subject::getAnonymousSubject());

	} else if (!request->isProcessed()) {
		// Unauthenticated
		throw fastcgi::Forbidden();
		// request->sendError(403); // Forbidden
	}
}

bool
Authenticator::isAuthenticated(fastcgi::Request* request) const {
	if (request->getSubject()->isAnonymous()) {
		std::shared_ptr<Session> session = request->getSession();
		if (session) {
			std::shared_ptr<Subject> subject = session->getSubject();
			if (subject && !subject->isAnonymous()) {
				// Copy the cached authentication information from session to request
				request->setSubject(subject);
				return !(subject->isAnonymous());
			}
		}
		return !(request->getSubject()->isAnonymous());
	} else {
		return true;
	}
}

bool
Authenticator::isAuthenticationRequired(fastcgi::Request* request) const {
    for (auto& c : constraints_) {
    	if (c->check(request)) {
    		return true;
    	}
    }
	return false;
}

bool Authenticator::isAuthorized(fastcgi::Request* request) const {
	std::shared_ptr<Subject> subject = request->getSubject();
	bool allowed = true;
    for (auto& c : constraints_) {
    	if (c->check(request) && !subject->hasPrincipal(c->getRole())) {
   			allowed = false;
   			break;
    	}
    }
	return allowed;
}

bool
Authenticator::doLogin(fastcgi::Request* request, const std::string& username, const std::string& credentials) const {
	if (realm_) {
		std::shared_ptr<Subject> subject = realm_->authenticate(username, credentials);
		if (subject) {
			setSubject(request, subject);
			return !(subject->isAnonymous());
		}
	}
	return false;
}

void
Authenticator::setSubject(fastcgi::Request* request, std::shared_ptr<Subject> subject) const {
	// Cache the authentication information in our session, if any
	std::shared_ptr<Session> session = request->getSession();
	if (session) {
		session->setSubject(subject);
		if (!subject->isAnonymous() && changeSessionIdOnAuthentication_) {
			request->changeSessionId();
		}
	}

	// Cache the authentication information in our request
	request->setSubject(subject);
}


}

}

