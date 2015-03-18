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

#include <iostream>

#include "abstract_authenticator.h"

#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"
#include "fastcgi3/logger.h"

#include "details/component_context.h"

namespace fastcgi
{

namespace security
{

AbstractAuthenticator::AbstractAuthenticator(std::shared_ptr<fastcgi::ComponentContext> context)
: fastcgi::Component(context), globals_(nullptr), logger_()
{
	std::shared_ptr<fastcgi::ComponentContextImpl> impl = std::dynamic_pointer_cast<fastcgi::ComponentContextImpl>(context);
	if (!impl) {
		std::cerr << "AbstractAuthenticator: cannot fetch globals in request cache" << std::endl;
		throw std::runtime_error("cannot fetch globals in request cache");
	}
	globals_ = impl->globals();

    const fastcgi::Config *config = context->getConfig();
	const std::string componentXPath = context->getComponentXPath();

	changeSessionIdOnAuthentication_ = "true"==config->asString(componentXPath+"/changeSessionIdOnAuthentication", "true");

	initConstraints();
}

AbstractAuthenticator::~AbstractAuthenticator() {
}

void
AbstractAuthenticator::onLoad() {
	std::cout << "onLoad AbstractAuthenticator executed" << std::endl;

	std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
	logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
	if (!logger_) {
		std::cerr << "AbstractAuthenticator: cannot get component " << loggerComponentName << std::endl;
		throw std::runtime_error("cannot get component " + loggerComponentName);
	}

	const std::string realmComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/realm");
	realm_ = context()->findComponent<fastcgi::security::Realm>(realmComponentName);
	if (!realm_) {
		logger_->error("AbstractAuthenticator: cannot get realm %s\n", realmComponentName.c_str());
		throw std::runtime_error("cannot get component " + realmComponentName);
	}
}

void
AbstractAuthenticator::onUnload() {
	realm_.reset();
	std::cout << "onUnload AbstractAuthenticator executed" << std::endl;
}

void
AbstractAuthenticator::initConstraints() {
	const Config *config = context()->getConfig();

    const std::string url_prefix = config->asString("/fastcgi/handlers/@urlPrefix", StringUtils::EMPTY_STRING);

    // Constraints with available attribute "url"
    std::vector<std::string> v;
    config->subKeys("/fastcgi/security-constraints/constraint", v);
    for (auto& k : v) {
        std::string url_filter = config->asString(k + "/@url", "/");
        std::string role_name = config->asString(k + "/@role", "");
        if (!url_filter.empty() && !role_name.empty()) {
        	constraints_.push_back(std::make_shared<SecurityConstraint>(url_filter, url_prefix, role_name));
        }
    }

}

}

}

