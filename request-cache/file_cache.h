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

#ifndef _FASTCGI_REQUEST_CACHE_FILE_CACHE_H_
#define _FASTCGI_REQUEST_CACHE_FILE_CACHE_H_

#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>

#include "fastcgi3/component.h"
#include "details/request_cache.h"
#include "details/server.h"
#include "details/globals.h"

namespace fastcgi
{

class Logger;

struct DelayTask {
	DelayTask() {}
	DelayTask(const std::string &key_str, int num_retries) :
		key(key_str), retries(num_retries)
	{}
	std::string key;
	int retries;
};

class FileRequestCache : virtual public RequestCache, virtual public Component, public Server, public std::enable_shared_from_this<RequestCache> {
public:
	FileRequestCache(std::shared_ptr<ComponentContext> context);
	virtual ~FileRequestCache();

	virtual void onLoad();
	virtual void onUnload();

	virtual DataBuffer create();
	virtual void save(Request *request, std::chrono::milliseconds delay);
	virtual std::uint32_t minPostSize() const;

protected:
	virtual const Globals* globals() const;
	virtual std::shared_ptr<Logger> logger() const;

private:
	void handle();
	bool saveRequest(Request *request, const std::string &key, std::string &new_key);
	std::string getKey(Request *request);
	std::string getStoredKey(Request *request);
	std::string generateUniqueKey();
	void eraseActive(const std::string &key);
	void insertWaiting(std::chrono::milliseconds delay, const std::string &key, int retries);
	DataBuffer createFileBuffer(const std::string &key);
	std::string createHardLink(const std::string &key);
	void stop();

private:
	const Globals *globals_;
	std::shared_ptr<Logger> logger_;
	std::shared_ptr<SessionManager> sessionManager_;

	std::string cache_dir_;
	std::uint64_t window_;
	std::uint32_t max_retries_;
	std::uint32_t min_post_size_;
	std::multimap<std::chrono::steady_clock::time_point, DelayTask> waiting_;
	std::map<std::string, int> active_;
	std::mutex active_mutex_, waiting_mutex_;
	std::unique_ptr<std::thread> thread_;

	bool stopped_;
	std::condition_variable condition_;
	std::mutex mutex_;
};

} // namespace fastcgi

#endif // _FASTCGI_REQUEST_CACHE_FILE_CACHE_H_
