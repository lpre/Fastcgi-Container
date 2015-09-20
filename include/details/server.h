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

#ifndef _FASTCGI_DETAILS_SERVER_H_
#define _FASTCGI_DETAILS_SERVER_H_

#include <functional>

#include "details/handlerset.h"
#include "details/request_thread_pool.h"

namespace fastcgi
{

class Globals;

class Server {
public:
	Server();
	virtual ~Server();

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

protected:
	virtual void handleRequest(RequestTask task);
	virtual const Globals* globals() const = 0;
	virtual std::shared_ptr<Logger> logger() const = 0;

	void handleRequestInternal(std::vector<std::shared_ptr<Filter>> &filters, RequestTask task);
	void handleRequestInternal(std::vector<std::shared_ptr<Filter>> &filters, const HandlerSet::HandlerDescription* handler, RequestTask task);

	void getFilters(RequestTask task, std::vector<std::shared_ptr<Filter>> &v) const;
	const HandlerSet::HandlerDescription* getHandler(RequestTask task) const;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_SERVER_H_
