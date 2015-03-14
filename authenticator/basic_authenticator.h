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
