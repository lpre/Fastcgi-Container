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

#ifndef INCLUDE_BASIC_AUTHENTICATOR_H_
#define INCLUDE_BASIC_AUTHENTICATOR_H_

#include <string>

#include "fastcgi3/component.h"

#include "abstract_authenticator.h"

namespace fastcgi
{

class Request;

namespace security
{

/**
 * HTTP Basic authentication is the simplest technique for enforcing access controls to web resources because it doesn't require cookies,
 * session identifier and login pages. Rather, HTTP Basic authentication uses static, standard fields in the HTTP header which means that
 * no handshakes have to be done in anticipation.
 *
 * When the server wants the user agent to authenticate itself towards the server, it must respond appropriately to unauthenticated requests.
 * Unauthenticated requests should return a response whose header contains a HTTP status "401 Not Authorized" and a HTTP header
 * field "WWW-Authenticate", which is constructed as following:
 *
 * 		WWW-Authenticate: Basic realm="WallyWorld"
 *
 * where "WallyWorld" is a realm name - the string assigned by the server to identify the protection space of the Request-URI.
 *
 * To receive authorization, the client sends the credentials within the HTTP header field "Authorization", which is constructed as follows:
 * 	1. Username and password are combined into a string "username:password" with a single colon (":") character as a separator.
 * 	2. The resulting string is then encoded using the RFC2045-MIME variant of Base64, except not limited to 76 char/line.
 * 	3. The authorization method and a space i.e. "Basic " is then put before the encoded string.
 *
 * For example, if the user agent uses "Aladdin" as the username and "open sesame" as the password then the field is formed as follows:
 *
 * 		Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ
 *
 */
class BasicAuthenticator : virtual public AbstractAuthenticator {
public:
	static const std::string COMPONENT_NAME;

public:
	BasicAuthenticator(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~BasicAuthenticator();

	virtual bool authenticate(fastcgi::Request* request) const override;
};


} // namespace security


} // namespace fastcgi

#endif /* INCLUDE_BASIC_AUTHENTICATOR_H_ */
