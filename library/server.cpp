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

// #include "settings.h"

#include <chrono>

#include "details/server.h"
#include "details/globals.h"
#include "details/handlerset.h"

#include "fastcgi3/logger.h"
#include "fastcgi3/except.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{
	
Server::Server() {
}

Server::~Server() {
}

void
Server::handleRequest(RequestTask task) {

	task.futureHandlers = [this](RequestTask task) {
		const HandlerSet::HandlerDescription* handler = getHandler(task);
		if (nullptr == handler || handler->handlers.empty()) {
			throw NotFound();
		}
		return handler->handlers;
	};

	task.dispatch = [this](RequestTask task) {
		std::vector<std::shared_ptr<Filter>> filters;
		getFilters(task, filters);

		const HandlerSet::HandlerDescription* handler = getHandler(task);
		if ((nullptr == handler || handler->handlers.empty()) && !filters.empty()) {
			// Handler not found - let the filter(s) to be executed
			// and then try to find the handler again
			// Example: "athenticator"-filter may redirect the
			// request to undefined path (like "j_security_check") and
			// after the login redirect back to the original path
			handleRequestInternal(filters, task);
		} else {
			handleRequestInternal(filters, handler, task);
		}
	};

	task.dispatch(task);
}

void
Server::handleRequestInternal(std::vector<std::shared_ptr<Filter>> &filters, const HandlerSet::HandlerDescription* handler, RequestTask task) {
	if (nullptr == handler || handler->handlers.empty()) {
		task.request->sendError(404);
		return;
	}

	try {
		task.filters = filters;
		task.handlers = handler->handlers;

		RequestsThreadPool* pool = globals()->pools().find(handler->poolName)->second.get();
		task.start = std::chrono::steady_clock::now() + pool->delay();
		pool->addTask(task);
	}
	catch (const std::exception &e) {
		task.request->sendError(503);
		logger()->error("cannot add request to pool: %s", e.what());
	}
}

void
Server::handleRequestInternal(std::vector<std::shared_ptr<Filter>> &filters, RequestTask task) {
	try {
		task.filters = filters;
		task.handlers.clear();

		RequestsThreadPool* pool = globals()->pools().find(globals()->handlers()->getDefaultPool())->second.get();
		task.start = std::chrono::steady_clock::now() + pool->delay();
		pool->addTask(task);
	}
	catch (const std::exception &e) {
		task.request->sendError(503);
		logger()->error("cannot add request to pool: %s", e.what());
	}
}


void
Server::getFilters(RequestTask task, std::vector<std::shared_ptr<Filter>> &v) const {
	globals()->handlers()->findURIFilters(task.request.get(), v);
}

const HandlerSet::HandlerDescription*
Server::getHandler(RequestTask task) const {
	return globals()->handlers()->findURIHandler(task.request.get());
}

} // namespace fastcgi
