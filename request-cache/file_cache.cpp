// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
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

#include "settings.h"

#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <stdexcept>

#include "details/component_context.h"
#include "details/componentset.h"
#include "details/file_buffer.h"

#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"
#include "fastcgi3/data_buffer.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/util.h"

#include "file_cache.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

struct KeyId {
	KeyId() : id(0), last(time(nullptr)) {}
	std::string get() {
		time_t now = time(nullptr);
		if (now == last) {
			++id;
		}
		else {
			last = now;
			id = 0;
		}
		std::string result;
		result.append((char*)&id, sizeof(id));
		result.push_back(':');
		result.append((char*)&last, sizeof(last));
		result.push_back(':');
		pthread_t thread_id = pthread_self();
		result.append((char*)&thread_id, sizeof(thread_id));
		return HashUtils::hexMD5(result.c_str(), result.size());
	}

	int id;
	time_t last;
};

static thread_local std::unique_ptr<KeyId> key_id_holder;

std::string
FileRequestCache::generateUniqueKey() {
	KeyId* key_id = key_id_holder.get();
    if (nullptr == key_id) {
    	key_id_holder.reset(new KeyId());
    	key_id = key_id_holder.get();
    }
    return key_id->get();
}

class RequestCacheStream : public RequestIOStream {
public:
	int read(char *buf, int size) {
		(void)buf;
		return size;
	}
	int write(const char *buf, int size) {
		(void)buf;
		return size;
	}
	void write(std::streambuf *buf) {
		(void)buf;
	}
};

FileRequestCache::FileRequestCache(std::shared_ptr<ComponentContext> context) :
	Component(context), globals_(nullptr), logger_(), sessionManager_(), stopped_(false) {

	std::shared_ptr<ComponentContextImpl> impl = std::dynamic_pointer_cast<ComponentContextImpl>(context);
	if (!impl) {
		throw std::runtime_error("cannot fetch globals in request cache");
	}
	globals_ = impl->globals();

    const Config *config = context->getConfig();
	const std::string componentXPath = context->getComponentXPath();

	cache_dir_ = config->asString(componentXPath + "/cache-dir", StringUtils::EMPTY_STRING);
	if (cache_dir_.empty()) {
		cache_dir_ = "/tmp/fastcgi3-container/cache/request-cache/";
		std::cout << "FileRequestCache: cache directory is not configured; using default directory \"" << cache_dir_ << "\"" << std::endl;
	}
	if (*cache_dir_.rbegin() != '/') {
		cache_dir_.push_back('/');
	}
	try {
		fastcgi::FileSystemUtils::createDirectories(cache_dir_);
		if (!fastcgi::FileSystemUtils::isWritable(cache_dir_)) {
			throw std::runtime_error("Permission denied");
		}
	} catch (const std::exception &e) {
		std::cerr << "FileRequestCache: could not create cache directory \"" << cache_dir_ << "\": " << e.what() << std::endl;
		throw;
	}

	window_ = config->asInt(componentXPath + "/file-window", 1024*1024);
	max_retries_ = config->asInt(componentXPath + "/max-retries", 2);
	min_post_size_ = config->asInt(componentXPath + "/min-post-size", 1024*1024);
}

FileRequestCache::~FileRequestCache() {
}

void
FileRequestCache::onLoad() {
	std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
	logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
	if (!logger_) {
		throw std::runtime_error("cannot get component " + loggerComponentName);
	}

	const std::string managerComponentName = context()->getConfig()->asString(
		context()->getComponentXPath() + "/session[@attach=\"1\" or @attach=\"true\"]@component",
		globals_->config()->asString("/fastcgi[count(session)=1]/session[@attach=\"1\" or @attach=\"true\"]/@component", StringUtils::EMPTY_STRING));
	if (!managerComponentName.empty()) {
		std::shared_ptr<Component> managerComponent = globals()->components()->find(managerComponentName);
		if (!managerComponent) {
			throw std::runtime_error("cannot get component " + managerComponentName);
		}

		sessionManager_ = std::dynamic_pointer_cast<SessionManager>(managerComponent);
		if (!sessionManager_) {
			throw std::runtime_error("Component " + managerComponentName + " does not implement SessionManager interface");
		}
	}

	thread_.reset(new std::thread(&FileRequestCache::handle, this));
}

void
FileRequestCache::onUnload() {
	stop();
}

DataBuffer
FileRequestCache::createFileBuffer(const std::string &key) {
	std::string path = cache_dir_ + key;
	return DataBuffer::create(new FileBuffer(path.c_str(), window_));
}

DataBuffer
FileRequestCache::create() {
	std::string key = generateUniqueKey();
	DataBuffer buffer = createFileBuffer(key);
	if (!buffer.isNil()) {
		std::lock_guard<std::mutex> lock(active_mutex_);
		active_.insert(std::make_pair(key, 0));
	}
	return buffer;
}

std::string
FileRequestCache::getStoredKey(Request *request) {
	DataBuffer request_buffer = request->requestBody();
	FileBuffer* impl = dynamic_cast<FileBuffer*>(request_buffer.impl());
	const std::string& filename = impl ? impl->filename() : StringUtils::EMPTY_STRING;
	if (!filename.empty() &&
		0 == strncmp(filename.c_str(), cache_dir_.c_str(), cache_dir_.size())) {
		return filename.substr(cache_dir_.size());
	}
	return StringUtils::EMPTY_STRING;
}

std::string
FileRequestCache::getKey(Request *request) {
	std::string key = getStoredKey(request);
	return key.empty() ? generateUniqueKey() : key;
}

std::string
FileRequestCache::createHardLink(const std::string &key) {
	std::string path = cache_dir_ + key;
	std::string new_key = generateUniqueKey();
	std::string new_path = cache_dir_ + new_key;
	if (-1 == link(path.c_str(), new_path.c_str())) {
		char buffer[256];
		logger_->error("Cannot link file %s: %s", path.c_str(), strerror_r(errno, buffer, sizeof(buffer)));
		return StringUtils::EMPTY_STRING;
	}
	return new_key;
}

bool
FileRequestCache::saveRequest(Request *request, const std::string &key, std::string &new_key) {
	FileBuffer* impl = dynamic_cast<FileBuffer*>(request->requestBody().impl());
	const std::string& filename = impl ? impl->filename() : StringUtils::EMPTY_STRING;
	DataBuffer buffer;
	if (filename.empty() ||
		0 != strncmp(filename.c_str(), cache_dir_.c_str(), cache_dir_.size())) {
		buffer = createFileBuffer(key);
		if (buffer.isNil()) {
			return false;
		}
		request->serialize(buffer);
	}
	new_key = createHardLink(key);
	return !new_key.empty();
}

void
FileRequestCache::eraseActive(const std::string &key) {
	std::lock_guard<std::mutex> lock(active_mutex_);
	active_.erase(key);
}

void
FileRequestCache::insertWaiting(std::chrono::milliseconds delay, const std::string &key, int retries) {
	std::unique_lock<std::mutex> lock(waiting_mutex_);
	waiting_.insert(std::make_pair(std::chrono::steady_clock::now()+delay, DelayTask(key, retries)));
}

void
FileRequestCache::save(Request *request, std::chrono::milliseconds delay) {

	if (std::chrono::milliseconds(0)==delay || max_retries_<=0) {
		std::string key = getStoredKey(request);
		if (!key.empty()) {
			eraseActive(key);
		}
		return;
	}

	std::string key = getKey(request);
	unsigned int retries = 0;
	bool active_found = false;
	{
		std::lock_guard<std::mutex> lock(active_mutex_);
		auto it = active_.find(key);
		if (active_.end() != it) {
			active_found = true;
			retries = it->second;
		}
	}

	if (!active_found) {
		std::string new_key;
		if (!saveRequest(request, key, new_key)) {
			logger_->error("Cannot save request for %s", request->getScriptName().c_str());
			return;
		}
		insertWaiting(delay, new_key, 1);
		return;
	}

	if (retries >= max_retries_) {
		logger_->info("Max retries reach for %s", request->getScriptName().c_str());
		eraseActive(key);
		return;
	}


	std::string new_key;
	if (!saveRequest(request, key, new_key)) {
		logger_->error("Cannot save request for %s", request->getScriptName().c_str());
		eraseActive(key);
		return;
	}

	eraseActive(key);
	insertWaiting(delay, new_key, retries + 1);
}

std::uint32_t
FileRequestCache::minPostSize() const {
	return min_post_size_;
}

void
FileRequestCache::stop() {
	stopped_ = true;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.notify_all();
	}
	thread_->join();
}

void
FileRequestCache::handle() {

	while (true) {
		try {
            if (stopped_) {
            	return;
            }

			DelayTask delay_task;
			{
				std::unique_lock<std::mutex> lock(waiting_mutex_);
				auto task_it = waiting_.begin();
				if (waiting_.end() == task_it || task_it->first > std::chrono::steady_clock::now()) {
					lock.unlock();
		            std::unique_lock<std::mutex> l(mutex_);
		            condition_.wait_for(l, std::chrono::seconds(1));
					continue;
				}
				delay_task = task_it->second;
				waiting_.erase(task_it);
			}

			RequestTask task;

			task.request = std::make_shared<Request>(logger_, shared_from_this(), sessionManager_);
			task.request_stream = std::make_shared<RequestCacheStream>();

			DataBuffer buffer = createFileBuffer(delay_task.key);
			if (buffer.isNil()) {
				logger_->error("Cannot load file %s", delay_task.key.c_str());
			}
			else {
				std::lock_guard<std::mutex> lock(active_mutex_);
				active_.insert(std::make_pair(delay_task.key, delay_task.retries));
			}
			task.request->parse(buffer);
			handleRequest(task);
		} catch (const std::exception &e) {
			logger_->error("caught exception while handling request: %s", e.what());
		} catch (...) {
			logger_->error("caught unknown exception while handling request");
		}
	}
}

const Globals*
FileRequestCache::globals() const {
	return globals_;
}

std::shared_ptr<Logger>
FileRequestCache::logger() const {
	return logger_;
}

} //namespace fastcgi

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("request-cache", fastcgi::FileRequestCache)
FCGIDAEMON_REGISTER_FACTORIES_END()
