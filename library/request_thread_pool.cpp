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

#include "details/request_thread_pool.h"

#include <vector>

#include <sys/time.h>

#include "fastcgi3/except.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/logger.h"

#include "fastcgi3/stream.h"

#include "../main/fcgi_request.h"


#include "details/handler_context.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

RequestsThreadPool::RequestsThreadPool(const unsigned threadsNumber, const unsigned queueLength, std::shared_ptr<fastcgi::Logger> logger)
: ThreadPool<RequestTask>(threadsNumber, queueLength), logger_(logger), delay_(0) {
}

RequestsThreadPool::RequestsThreadPool(const unsigned threadsNumber, const unsigned queueLength, std::chrono::milliseconds delay, std::shared_ptr<fastcgi::Logger> logger)
: ThreadPool<RequestTask>(threadsNumber, queueLength), logger_(logger), delay_(delay) {
}

RequestsThreadPool::~RequestsThreadPool() {
}

std::chrono::milliseconds
RequestsThreadPool::delay() const {
	return delay_;
}

void
RequestsThreadPool::handleTask(RequestTask task) {
    try {
   		if (std::chrono::steady_clock::now() - task.start < delay_) {
			logger_->error("thread pool task is timed out");
			task.request->sendError(503);
			return;
    	}
        try {
            std::shared_ptr<LoggerRequestId> logger_req_id = std::dynamic_pointer_cast<LoggerRequestId>(logger_);
            if (logger_req_id) {
                logger_req_id->setRequestId(task.request->getRequestId());
            }

            // Function to execute all handlers
            auto handlers = [&task](Request *r, HandlerContext *c) {
            	if (task.handlers.empty() && task.futureHandlers) {
            		task.handlers = task.futureHandlers(task);
            	}
				for (auto& i : task.handlers) {
					if (r->isProcessed()) {
						break;
					}
					i->handleRequest(r, c);
				}
            };

            // Recursive execution of the nested filters
            std::vector<std::function<void(Request *req, HandlerContext *context)>> filters;
            unsigned int next = 0;
            for (auto& i : task.filters) {
				++next;
            	filters.push_back([&filters, &i, next, &handlers](Request *r, HandlerContext *c) {
					if (next<filters.size()) {
						// Execute current filter, passing the reference to the functor for the next filter
						i->doFilter(r, c, filters[next]);
					} else {
						// No more filters - execute handlers
						// Execute current filter, passing the reference to the functor for the handlers
						i->doFilter(r, c, handlers);
					}
            	});
            }

            // All filters and handlers are using the same underlaying instance
            // of the std::stringstream hosted by class RequestImpl.
            // That is, if any filter or handler instantiates the class RequestStream
            // using the same request pointer, it will contain the pointer to
            // the same std::stringstream.
            fastcgi::RequestStream stream(task.request.get());
            stream.reset();

            std::unique_ptr<HandlerContext> context(new HandlerContextImpl);
            if (filters.size()>0) {
            	filters[0](task.request.get(), context.get());
            } else {
            	// No filter is defined - execute handlers
            	handlers(task.request.get(), context.get());
            }

            stream.flush();
            stream.reset();

            task.request->sendHeaders();
        }
        catch (const DispatchException &e) {
        	// TODO: usage of exception to dispatch the request is not really clean solution
        	if (task.dispatch) {
				if (!task.request->headersSent()) {
					if (DispatchException::DispatchType::FORWARD==e.type()) {
						// Clear request output stream
						task.request->getResponseStream()->str(std::string());
					}

					DataBuffer buffer = e.buffer();
					if (!buffer.isNil()) {
						task.request->restore(buffer);
					}

					const std::string &url = e.url();
					if (!url.empty()) {
						auto it = task.request->vars_.find("SCRIPT_NAME");
						if (it == task.request->vars_.end()) {
							task.request->vars_.insert({"SCRIPT_NAME", url});
						} else {
							it->second = url;
						}
					}

					task.dispatch(task);
				} else {
					throw std::runtime_error("Error while dispatching request "+task.request->getURI()+": headers already sent");
				}
        	} else {
				throw std::runtime_error("Error while dispatching request "+task.request->getURI()+": dispatcher is not assigned");
        	}
        }
        catch (const HttpException &e) {
            bool headersAlreadySent = false;
            try {
                task.request->setStatus(500);
            }
            catch (...) { // this means that headers already send and we cannot change status/headers and so on
                headersAlreadySent = true;
            }
            if (headersAlreadySent) {
                throw;
            }
            else {
                task.request->sendError(e.status());
            }
        }
        catch (const std::exception &e) {
            bool headersAlreadySent = false;
            try {
                task.request->setStatus(500);
            }
            catch (...) { // this means that headers already send and we cannot change status/headers and so on
                headersAlreadySent = true;
            }
            if (headersAlreadySent) {
                throw;
            }
            else {
                task.request->sendError(500);
            }
        }
        catch (...) {
            bool headersAlreadySent = false;
            try {
                task.request->setStatus(500);
            }
            catch (...) { // this means that headers already send and we cannot change status/headers and so on
                headersAlreadySent = true;
            }
            if (headersAlreadySent) {
                throw;
            }
            else {
                task.request->sendError(500);
            }
        }
    }
    catch (const std::exception &e) {
        logger_->error("%s", e.what());
        throw;
    }
    catch (...) {
        logger_->error("RequestsThreadPool::handleTask: got unknown exception");
        throw;
    }
}

} // namespace fastcgi
