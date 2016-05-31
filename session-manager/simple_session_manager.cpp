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
#include <chrono>
#include <iostream>
#include <uuid/uuid.h>

#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"
#include "fastcgi3/data_buffer.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/util.h"

#include "details/component_context.h"

#include "simple_session_manager.h"

namespace fastcgi
{

const std::string SimpleSessionManager::COMPONENT_NAME {"simple-session-manager"};

const unsigned int check_session_timeout_sec {30};

const std::string default_crawlers_agents {".*(bot|Crawler|Google|Rambler|Yahoo|Yandex|accoona|ASPSeek|Lycos|Scooter|AltaVista|eStyle|Scrubby|Bench).*"};
const std::string crawler_agent_session_id {"crawler-fixed-sesson-id"};
const unsigned int crawler_session_timeout_min {5};


SimpleSessionManager::SimpleSessionManager(std::shared_ptr<ComponentContext> context) :
	Component(context), stopped_(false), max_inactive_interval_(30), globals_(nullptr), logger_()
{
	std::shared_ptr<ComponentContextImpl> impl = std::dynamic_pointer_cast<ComponentContextImpl>(context);
	if (!impl) {
		std::cerr << "SimpleSessionManager: cannot fetch globals in request cache" << std::endl;
		throw std::runtime_error("Cannot fetch globals in request cache");
	}
	globals_ = impl->globals();

    const Config *config = context->getConfig();
	const std::string componentXPath = context->getComponentXPath();

	const int sessionTimeout = globals_->config()->asInt(
			"/fastcgi[count(session)=1]/session[@attach=\"1\" or @attach=\"true\"]/timeout",	// Global configuration value
			config->asInt(componentXPath + "/timeout",											// Component-specific value
			30));																				// Default value 30 minutes
	setMaxInactiveInterval<std::chrono::minutes>(sessionTimeout);

	// Regular expression that the user agent HTTP request header is matched against to determine
	// if a request is from a web crawler. If not set, the default value is used.
	const std::string crawler_agents = globals_->config()->asString(
			"/fastcgi[count(session)=1]/session[@attach=\"1\" or @attach=\"true\"]/crawlerUserAgents",
			default_crawlers_agents);
	try {
		crawler_agents_regexp_.assign(crawler_agents, std::regex_constants::ECMAScript|std::regex_constants::icase);
	} catch (const std::regex_error& e) {
    	std::cerr << "SimpleSessionManager: invalid crawler agents RegExp: " << crawler_agents << std::endl;
	}

}

SimpleSessionManager::~SimpleSessionManager() {
};

void
SimpleSessionManager::onLoad() {
	std::cout << "onLoad SimpleSessionManager executed" << std::endl;

	initTimeoutThread();

	std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
	logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
	if (!logger_) {
		std::cerr << "SimpleSessionManager: cannot get component " << loggerComponentName << std::endl;
		throw std::runtime_error("Cannot get component " + loggerComponentName);
	}
}

void
SimpleSessionManager::onUnload() {
	stop();
}

bool
SimpleSessionManager::isBot(const Request* request) const {
	const std::string& userAgent = request->getHeader("USER-AGENT");
	if (!userAgent.empty() && std::regex_match(userAgent, crawler_agents_regexp_)) {
		return true;
	}
	return false;
}

std::string
SimpleSessionManager::getNewId(Request* request) {
	if (!isBot(request)) {
		return SessionManager::getNewId(request);
	}
	return crawler_agent_session_id;
}

void
SimpleSessionManager::stop() {
	stopTimeoutThread();
	SessionManager::stop();
}

std::shared_ptr<Session>
SimpleSessionManager::createInternal(Request* request) {
	std::shared_ptr<Session> session;

	std::string id = getNewId(request);

	if (crawler_agent_session_id!=id) {
		// This is a new session for normal user (or unknown crawler)

		session = newSession(id);

		// Do it in the loop to avoid the duplication of the Session ID
		while (!(sessions_.insert({id, session}).second)) {
			id = getNewId(request);
			session = newSession(id);
		}
		// Reset max time of session inactivity for normal user
		session->setMaxInactiveInterval<std::chrono::minutes>(max_inactive_interval_.count());

	} else {
		// Crawler detected: use the same one dedicated sessions for all bots/crawlers

		if (sessions_.end()==sessions_.find(id)) {
			session = newSession(id);
			sessions_.insert({id, session});
			// Reset max time of session inactivity for crawler
			session->setMaxInactiveInterval<std::chrono::minutes>(crawler_session_timeout_min);
		}

		session = sessions_.find(id)->second;
		session->updateLastAccessedTime();
	}

	return session;
}


void
SimpleSessionManager::initTimeoutThread() {
	timeout_thread_ = std::make_unique<std::thread>(&SimpleSessionManager::checkSessionTimeout, this);
}

void
SimpleSessionManager::stopTimeoutThread() {
	stopped_.store(true);
	condition_.notify_all();
	timeout_thread_->join();
}

void SimpleSessionManager::checkSessionTimeout() {
	const auto duration = std::chrono::seconds(check_session_timeout_sec);
	while (true) {
		try {
            if (stopped_) {
            	return;
            }
            std::unique_lock<std::mutex> lock(mutex_);

        	for (auto it=sessions_.cbegin(), end=sessions_.cend(); it != end;) {
        		if (it->second && it->second->isExpired()) {
            		sessions_.erase(it++);
        		} else {
        			++it;
        		}
        	}

            condition_.wait_for(lock, duration);

		} catch (...) {
			if (logger_) {
				logger_->error("Caught exception while checking session timeout");
			}
		}
	}
}


FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY(fastcgi::SimpleSessionManager::COMPONENT_NAME, fastcgi::SimpleSessionManager)
FCGIDAEMON_REGISTER_FACTORIES_END()

}

