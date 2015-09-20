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

#include "basic_authenticator.h"
#include "delegated_authenticator.h"
#include "form_authenticator.h"

#include "fastcgi3/component_factory.h"

namespace fastcgi
{

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY(fastcgi::security::BasicAuthenticator::COMPONENT_NAME, fastcgi::security::BasicAuthenticator)
FCGIDAEMON_ADD_DEFAULT_FACTORY(fastcgi::security::DelegatedAuthenticator::COMPONENT_NAME, fastcgi::security::DelegatedAuthenticator)
FCGIDAEMON_ADD_DEFAULT_FACTORY(fastcgi::security::FormAuthenticator::COMPONENT_NAME, fastcgi::security::FormAuthenticator)
FCGIDAEMON_REGISTER_FACTORIES_END()

}

