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

#ifndef INCLUDE_DETAILS_SESSION_MANAGER_H_
#define INCLUDE_DETAILS_SESSION_MANAGER_H_

#include <string>
#include <regex>
#include <memory>
#include <unordered_map>
#include <mutex>

#include "fastcgi3/request.h"
#include "fastcgi3/session.h"

namespace fastcgi
{

class Request;

class SessionManager {
public:
	static const std::string SESSION_COOKIE_NAME;

public:
	SessionManager();
	virtual ~SessionManager();

    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

	virtual void stop();

	std::shared_ptr<Session> create(Request* request);
	std::shared_ptr<Session> get(Request* request);
	void changeId(Request* request);

	virtual void invalidateAll();
	virtual void invalidate(Session* session);
	virtual std::string getNewId(Request* request);

	virtual bool isBot(const Request* request) const = 0;

protected:
	virtual std::shared_ptr<Session> createInternal(Request* request) = 0;
	virtual std::shared_ptr<Session> newSession(const std::string &id);

protected:
	mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
    bool sessionCookieHttpOnly_;
};


} // namespace fastcgi

#endif /* INCLUDE_DETAILS_SESSION_MANAGER_H_ */
