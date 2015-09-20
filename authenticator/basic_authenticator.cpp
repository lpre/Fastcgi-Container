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
#include "basic_authenticator.h"

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

const std::string BasicAuthenticator::COMPONENT_NAME {"basic-authenticator"};

BasicAuthenticator::BasicAuthenticator(std::shared_ptr<fastcgi::ComponentContext> context)
: fastcgi::Component(context), AbstractAuthenticator(context) {
}

BasicAuthenticator::~BasicAuthenticator() {
}

bool
BasicAuthenticator::authenticate(fastcgi::Request* request) const {

	if (request->hasHeader("Authorization")) {
		const std::string &auth_header = request->getHeader("Authorization");

		std::vector<std::string> h;
		StringUtils::split(auth_header, ' ', h);
		if (h.size()==2) {
			std::vector<std::string> credentials;
			StringUtils::split(HashUtils::base64_decode(h.at(1)), ':', credentials);
			if (credentials.size()==2) {
				return doLogin(request, credentials.at(0), credentials.at(1));
			}
		}

	}

	// Send authentication request to client
	request->setHeader("WWW-Authenticate", "Basic realm=\""+realm_->getName()+"\"");
	request->setStatus(401);
	request->markAsProcessed();
	return false;
}


}

}

