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

#ifndef _FASTCGI_DETAILS_GLOBALS_H_
#define _FASTCGI_DETAILS_GLOBALS_H_

#include <map>
#include <string>
#include <memory>

namespace fastcgi
{

class ComponentSet;
class Config;
class HandlerSet;
class Loader;
class Logger;
class RequestsThreadPool;

class Globals {
public:
	Globals(const Config *config);
	virtual ~Globals();

	Globals(const Globals&) = delete;
	Globals& operator=(const Globals&) = delete;

	const Config* config() const;

	using ThreadPoolMap = std::map<std::string, std::shared_ptr<RequestsThreadPool>>;

	ComponentSet* components() const;
	HandlerSet* handlers() const;
	const ThreadPoolMap& pools() const;
	Loader* loader() const;
	std::shared_ptr<Logger> logger() const;

	void stopThreadPools();
	void joinThreadPools();

private:
	void initPools();
	void initLogger();
	void startThreadPools();

private:
	ThreadPoolMap pools_;
	const Config* config_;
	std::unique_ptr<Loader> loader_;
	std::unique_ptr<HandlerSet> handlerSet_;
	std::unique_ptr<ComponentSet> componentSet_;
	std::shared_ptr<Logger> logger_;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_GLOBALS_H_
