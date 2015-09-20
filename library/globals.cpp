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

// #include "settings.h"

#include <functional>
#include <chrono>

#include "fastcgi3/component.h"
#include "fastcgi3/config.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/logger.h"

#include "details/componentset.h"
#include "details/globals.h"
#include "details/handlerset.h"
#include "details/loader.h"
#include "details/request_thread_pool.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

Globals::Globals(const Config *config)
: config_(config), loader_(new Loader()), handlerSet_(new HandlerSet()), componentSet_(new ComponentSet()), logger_() {
	loader_->init(config);
	componentSet_->init(this);
	handlerSet_->init(config, componentSet_.get());

	initLogger();
	initPools();
	startThreadPools();
}

Globals::~Globals() {
}

ComponentSet*
Globals::components() const {
	return componentSet_.get();
}

HandlerSet*
Globals::handlers() const {
	return handlerSet_.get();
}

const Globals::ThreadPoolMap&
Globals::pools() const {
	return pools_;
}

Loader*
Globals::loader() const {
	return loader_.get();
}

std::shared_ptr<Logger>
Globals::logger() const {
	return logger_;
}

const Config*
Globals::config() const {
	return config_;
}

static void
startUpFunc(const std::set<std::shared_ptr<Handler>> &handlers) {
	for (auto& it : handlers) {
		it->onThreadStart();
	}
}

void
Globals::startThreadPools() {
	for (auto& it : pools_) {
		std::set<std::shared_ptr<Handler>> handlers;
		handlerSet_->findPoolHandlers(it.first, handlers);
		it.second->start(std::bind(&startUpFunc, handlers));
	}
}

void
Globals::stopThreadPools() {
	for (auto& it : pools_) {
		it.second->stop();
	}
}

void
Globals::joinThreadPools() {
	for (auto& it : pools_) {
		it.second->join();
	}
}

void
Globals::initPools() {
	std::set<std::string> poolsNeeded = handlerSet_->getPoolsNeeded();

	std::vector<std::string> poolSubkeys;
	config_->subKeys("/fastcgi/pools/pool", poolSubkeys);
    unsigned maxTasksInProcessCounter = 0;
    for (auto& p : poolSubkeys) {
        const std::string poolName = config_->asString(p + "/@name");
        const int threadsNumber = config_->asInt(p + "/@threads");
        const int queueLength = config_->asInt(p + "/@queue");
        const std::chrono::milliseconds delay = std::chrono::milliseconds(config_->asInt(p + "/@max-delay", 0));

		maxTasksInProcessCounter += (threadsNumber + queueLength);
		if (maxTasksInProcessCounter > 65535) {
			throw std::runtime_error("The sum of all threads and queue attributes must be not more than 65535");
		}

		if (pools_.find(poolName) != pools_.end()) {
            throw std::runtime_error(poolName + ": pool names must be unique");
        }

		if (poolsNeeded.find(poolName) == poolsNeeded.end()) {
			continue;
		}

		pools_.insert(make_pair(poolName, std::shared_ptr<RequestsThreadPool>(
			delay > std::chrono::milliseconds(0) ?
			new RequestsThreadPool(threadsNumber, queueLength, delay, logger_) :
			new RequestsThreadPool(threadsNumber, queueLength, logger_)))
		);
    }

    for (auto& i : poolsNeeded) {
        if (pools_.find(i) == pools_.end()) {
            throw std::runtime_error("cannot find pool " + i);
        }
    }
}

void
Globals::initLogger() {
	const std::string loggerComponentName = config_->asString("/fastcgi/daemon[count(logger)=1]/logger/@component");
	std::shared_ptr<Component> loggerComponent = componentSet_->find(loggerComponentName);
	if (!loggerComponent) {
		throw std::runtime_error("Daemon logger does not exist");
	}
	logger_ = std::dynamic_pointer_cast<Logger>(loggerComponent);
	if (!logger_) {
		throw std::runtime_error("Component " + loggerComponentName + " does not implement Logger interface");
	}
}

} // namespace fastcgi
