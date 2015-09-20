// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
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

#ifndef _FASTCGI_FASTCGI_SERVER_H_
#define _FASTCGI_FASTCGI_SERVER_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#include "details/server.h"
#include "fastcgi3/session_manager.h"

namespace fastcgi
{

class Config;
class Request;
class Logger;
class Loader;
class Endpoint;
class ComponentSet;
class HandlerSet;
class RequestsThreadPool;

class ServerStopper {
public:
	ServerStopper() {
		stopped_.store(false);
	}

	void stopped(bool flag) {
		stopped_.store(flag);
	}

	bool stopped() const {
		return stopped_.load();
	}
private:
	std::atomic<bool> stopped_;
};

class ActiveThreadCounter {
public:
	ActiveThreadCounter() {
		count_.store(0);
	}
	void increment() {
		++count_;
	}
	void decrement() {
		--count_;
	}
	int count() const {
		return count_.load();
	}
private:
	std::atomic<int> count_;
};

class FCGIServer : public Server {
protected:
	enum class Status {NOT_INITED, LOADING, RUNNING};

public:
	FCGIServer(std::shared_ptr<Globals> globals);
	virtual ~FCGIServer();

	static void writePid(const Config& config);
	void start();
	void stop();
	void join();
	
private:
	virtual const Globals* globals() const;
	virtual std::shared_ptr<Logger> logger() const override;
	virtual void handleRequest(RequestTask task) override;
	void handle(std::shared_ptr<Endpoint> endpoint);
	void monitor();

	std::string getServerInfo() const;

	void initMonitorThread();
	void initRequestCache();
	void initSessionManager();
	void initTimeStatistics();
    void initFastCGISubsystem();
	void initPools();
    void createWorkThreads();

	void stopInternal();

	void stopThreadFunction();

	Status status() const;

private:
	std::shared_ptr<Globals> globals_;
	std::shared_ptr<ServerStopper> stopper_;

	using ThreadHolder = char;
	std::shared_ptr<ThreadHolder> active_thread_holder_;

	std::vector<std::shared_ptr<Endpoint>> endpoints_;
	int monitorSocket_;
	
	std::shared_ptr<RequestCache> request_cache_;
	std::shared_ptr<ResponseTimeStatistics> time_statistics_;
	std::shared_ptr<SessionManager> sessionManager_;
	
	std::atomic<Status> status_;

	std::unique_ptr<std::thread> monitorThread_;
	std::unique_ptr<std::thread> stopThread_;
	int stopPipes_[2];

	bool logTimes_;
	std::vector<std::unique_ptr<std::thread>> globalPool_;

};

} // namespace fastcgi

#endif // _FASTCGI_FASTCGI_SERVER_H_
