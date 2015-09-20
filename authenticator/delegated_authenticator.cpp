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

#include "abstract_authenticator.h"
#include "delegated_authenticator.h"

#include <functional>
#include <iostream>
#include <algorithm>
#include <regex>

#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/util.h"
#include "fastcgi3/data_buffer.h"
#include "fastcgi3/stream.h"
#include "fastcgi3/except.h"

#include "details/component_context.h"
#include "details/file_buffer.h"

namespace fastcgi
{

namespace security
{

const std::string DelegatedAuthenticator::COMPONENT_NAME {"delegated-authenticator"};

DelegatedAuthenticator::DelegatedAuthenticator(std::shared_ptr<fastcgi::ComponentContext> context)
: fastcgi::Component(context), AbstractAuthenticator(context) {
}

DelegatedAuthenticator::~DelegatedAuthenticator() {
}

bool
DelegatedAuthenticator::authenticate(fastcgi::Request* request) const {
	// Obtain the name of authenticated user from the HTTP server
	const std::string& username = request->getRemoteUser();

	if (!username.empty() && realm_) {
		const std::string& password = request->getRemotePassword();

		if (password.empty()) {
			std::shared_ptr<Subject> subject = realm_->getSubject(username);
			if (subject) {
				setSubject(request, subject);
				return !(subject->isAnonymous());
			}
		} else {
			return doLogin(request, username, password);
		}
	}
	return false;
}

}

}

