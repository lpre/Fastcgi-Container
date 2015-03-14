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

#ifndef _FASTCGI_DETAILS_HANDLERSET_H_
#define _FASTCGI_DETAILS_HANDLERSET_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

namespace fastcgi
{

class Config;
class ComponentSet;
class Filter;
class Handler;
class Request;
class RequestFilter;

class HandlerSet {
public:
	using SelectorArray = std::vector<std::pair<std::string, std::shared_ptr<RequestFilter>>>;

	struct HandlerDescription {
		SelectorArray selectors;
		std::vector<std::shared_ptr<Handler>> handlers;
		std::string poolName;
		std::string id;
	};
	using HandlerArray = std::vector<HandlerDescription>;

	struct FilterDescription {
		SelectorArray selectors;
		std::vector<std::shared_ptr<Filter>> handlers;
		std::string id;
	};
	using FilterArray = std::vector<FilterDescription>;

public:
	HandlerSet();
	virtual ~HandlerSet();

	HandlerSet(const HandlerSet&) = delete;
	HandlerSet& operator=(const HandlerSet&) = delete;

	void init(const Config *config, const ComponentSet *componentSet);

	const HandlerSet::HandlerDescription* findURIHandler(const Request *request) const;
	void findPoolHandlers(const std::string &poolName, std::set<std::shared_ptr<Handler>> &handlers) const;
	std::set<std::string> getPoolsNeeded() const;

	void findURIFilters(const Request *request, std::vector<std::shared_ptr<Filter>> &v) const;
	
	const std::string& getDefaultPool() const;

private:
	void initInternal(const Config *config, const ComponentSet *componentSet, const std::string &url_prefix, std::vector<std::string> &v);

private:
	FilterArray filters_;
	HandlerArray handlers_;
	std::string defaultPoolName_;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_HANDLERSET_H_
