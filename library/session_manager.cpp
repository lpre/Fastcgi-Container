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

#include <string>
#include <regex>
#include <functional>

#include "fastcgi3/cookie.h"
#include "fastcgi3/session_manager.h"
#include "fastcgi3/util.h"

namespace fastcgi
{

const std::string SessionManager::SESSION_COOKIE_NAME {"JSESSIONID"};

SessionManager::SessionManager()
: sessionCookieHttpOnly_(true) {
}

SessionManager::~SessionManager() {
	stop();
}

void SessionManager::stop() {
	invalidateAll();
}

std::shared_ptr<Session>
SessionManager::create(Request* request) {
	std::lock_guard<std::mutex> lock(mutex_);

	if (request->hasCookie(SESSION_COOKIE_NAME)) {
		auto it = sessions_.find(request->getCookie(SESSION_COOKIE_NAME));
		if (sessions_.end()!=it && it->second) {
			it->second->invalidate();
		}
	}

	std::shared_ptr<Session> session = createInternal(request);

	Cookie sessionCookie(SESSION_COOKIE_NAME, session->getId());
	sessionCookie.httpOnly(sessionCookieHttpOnly_);
	sessionCookie.secure(request->isSecure());
	request->setCookie(sessionCookie);

    return session;
}

std::shared_ptr<Session>
SessionManager::get(Request* request) {
	std::lock_guard<std::mutex> lock(mutex_);

	if (request->hasCookie(SESSION_COOKIE_NAME)) {
		auto it = sessions_.find(request->getCookie(SESSION_COOKIE_NAME));
		if (sessions_.end()!=it && it->second) {
			it->second->updateLastAccessedTime();
			return it->second;
		}
	}

	std::shared_ptr<Session> session = createInternal(request);

	Cookie sessionCookie(SESSION_COOKIE_NAME, session->getId());
	sessionCookie.httpOnly(sessionCookieHttpOnly_);
	sessionCookie.secure(request->isSecure());
	request->setCookie(sessionCookie);

    return session;
}

void
SessionManager::changeId(Request* request) {
	std::lock_guard<std::mutex> lock(mutex_);

	if (request->hasCookie(SESSION_COOKIE_NAME)) {
		auto it = sessions_.find(request->getCookie(SESSION_COOKIE_NAME));
		if (sessions_.end()!=it && it->second) {
			std::string id = getNewId(request);
			it->second->changeId(id);
			sessions_.insert({id, std::move(it->second)});
			sessions_.erase(it);

			Cookie sessionCookie(SESSION_COOKIE_NAME, id);
			sessionCookie.httpOnly(sessionCookieHttpOnly_);
			sessionCookie.secure(request->isSecure());
			request->setCookie(sessionCookie);
		}
	}
}

void
SessionManager::invalidateAll() {
	std::lock_guard<std::mutex> lock(mutex_);

	for (auto it=sessions_.cbegin(), end=sessions_.cend(); it != end;) {
		sessions_.erase(it++);
	}
}

void
SessionManager::invalidate(Session* session) {
	if (nullptr!=session) {
		std::string id = session->getId();

	    auto it = sessions_.find(id);
	    if (sessions_.end() != it) {
			sessions_.erase(it);
	    }

		delete session;
	}
}


std::string
SessionManager::getNewId(Request* request) {
	return UUIDUtils::getNewId();
}

std::shared_ptr<Session>
SessionManager::newSession(const std::string &id) {
	Session* s = nullptr;
	try {
		s = new Session(id);
	    return std::shared_ptr<Session>(s, std::bind(&SessionManager::invalidate, this, std::placeholders::_1));
	} catch (...) {
		if (nullptr!=s) {
			delete s;
		}
	}
	return std::shared_ptr<Session>();
}

}

