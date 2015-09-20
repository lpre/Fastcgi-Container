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

#ifndef INCLUDE_ABSTRACT_AUTHENTICATOR_H_
#define INCLUDE_ABSTRACT_AUTHENTICATOR_H_

#include <string>
#include <memory>

#include "fastcgi3/component.h"
#include "fastcgi3/session_manager.h"
#include "details/security_authenticator.h"
#include "details/globals.h"

namespace fastcgi
{

class Request;

namespace security
{

class AbstractAuthenticator : virtual public Authenticator, virtual public fastcgi::Component {
public:
	AbstractAuthenticator(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~AbstractAuthenticator();

	virtual void onLoad() override;
	virtual void onUnload() override;

protected:
	void initConstraints();

private:
	const Globals *globals_;
	std::shared_ptr<Logger> logger_;
};


} // namespace security


} // namespace fastcgi

#endif /* INCLUDE_ABSTRACT_AUTHENTICATOR_H_ */
