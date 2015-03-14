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

#include <functional>

#include "fastcgi3/component.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/config.h"
#include "fastcgi3/security_principal.h"
#include "fastcgi3/security_subject.h"
#include "fastcgi3/security_realm.h"

namespace fastcgi
{

namespace security
{

Realm::Realm(std::shared_ptr<fastcgi::ComponentContext> context)
: fastcgi::Component(context), logger_() {

    const fastcgi::Config *config = context->getConfig();
	const std::string componentXPath = context->getComponentXPath();

	name_ = config->asString(componentXPath+"/name", "ExampleRealm");
}

Realm::~Realm() {
}

void
Realm::onLoad() {
	const std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
	if (loggerComponentName.empty()) {
		throw std::runtime_error("logger is not configured");
	}
	logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
	if (!logger_) {
		throw std::runtime_error("cannot get component " + loggerComponentName);
	}
}

void
Realm::onUnload() {
}

const std::string&
Realm::getName() const {
	return name_;
}

std::shared_ptr<Subject>
Realm::authenticate(const std::string& username, const std::string& credentials) {
	return std::shared_ptr<fastcgi::security::Subject>();
}

std::shared_ptr<Subject>
Realm::getSubject(const std::string& username) {
	return std::shared_ptr<fastcgi::security::Subject>();
}

}

}

