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

#ifndef INCLUDE_SIMPLE_SESSION_MANAGER_H_
#define INCLUDE_SIMPLE_SESSION_MANAGER_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>

#include "fastcgi3/component.h"
#include "fastcgi3/session_manager.h"
#include "details/globals.h"


namespace fastcgi
{

class Request;

class SimpleSessionManager : virtual public SessionManager, virtual public Component {
public:
	static const std::string COMPONENT_NAME;

public:
	SimpleSessionManager(std::shared_ptr<ComponentContext> context);
	~SimpleSessionManager();

	virtual void onLoad() override;
	virtual void onUnload() override;

    /**
     * Web crawlers can trigger the creation of many thousands of sessions as they crawl a site
     * which may result in significant memory consumption. This SessionManager ensures that crawlers
     * are associated with a single session - just like normal users - regardless of whether or
     * not they provide a session token with their requests.
     */
    virtual void setCrawlerAgentsRegExp(std::string crawler_agents);
	virtual void stop() override;

	template<class T>
	void setMaxInactiveInterval(unsigned int d) {
		max_inactive_interval_ = std::chrono::duration_cast<std::chrono::minutes>(T(d));
	}

	template<class T>
	std::chrono::duration<T> getMaxInactiveInterval() const {
		return std::chrono::duration_cast<T>(max_inactive_interval_);
	}

	virtual std::string getNewId(Request* request) override;

	virtual bool isBot(const Request* request) const override;

protected:
	virtual std::shared_ptr<Session> createInternal(Request* request);

	void initTimeoutThread();
	void stopTimeoutThread();
	void checkSessionTimeout();

private:
    std::atomic<bool> stopped_;
    std::condition_variable condition_;
    std::unique_ptr<std::thread> timeout_thread_;
    std::chrono::minutes max_inactive_interval_;
    std::regex crawler_agents_regexp_;

	const Globals *globals_;
	std::shared_ptr<Logger> logger_;
};


} // namespace fastcgi

#endif /* INCLUDE_SIMPLE_SESSION_MANAGER_H_ */
