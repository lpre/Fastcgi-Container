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

#include <sstream>
#include <sys/time.h>

#include "fastcgi3/component_factory.h"
#include "fastcgi3/request.h"

#include "response_time_handler.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

CounterData::CounterData() : min_(std::numeric_limits<std::uint64_t>::max()),
	max_(0), total_(0), hits_(0)
{}

void
CounterData::add(std::uint64_t time) {
	total_ += time;
	++hits_;
	min_ = std::min(min_, time);
	max_ = std::max(max_, time);
}

std::uint64_t
CounterData::min() const {
	return min_;
}

std::uint64_t
CounterData::max() const {
	return max_;
}

std::uint64_t
CounterData::avg() const {
	return hits_ ? total_/hits_ : 0;
}

std::uint64_t
CounterData::hits() const {
	return hits_;
}

ResponseTimeHandler::ResponseTimeHandler(std::shared_ptr<ComponentContext> context) : Component(context)
{}

ResponseTimeHandler::~ResponseTimeHandler()
{}

void
ResponseTimeHandler::onLoad() {
}

void
ResponseTimeHandler::onUnload() {
}

void
ResponseTimeHandler::handleRequest(Request *req, HandlerContext *handlerContext) {
	(void)handlerContext;
	std::stringstream str;
	str.precision(3);
	str << std::showpoint << std::fixed;
	str << "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	str << "<response-time>";
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& iter : data_) {
			str << "<handler id=\"" << iter.first << "\">";
			for (auto& it : iter.second) {
				str << "<data";
				str << " status=\"" << it.first << "\"";
				str << " avg=\"" << 0.001*it.second->avg() << "\"";
				str << " min=\"" << 0.001*it.second->min() << "\"";
				str << " max=\"" << 0.001*it.second->max() << "\"";
				str << " hits=\"" << it.second->hits() << "\"";
				str << "/>";
			}
			str << "</handler>";
		}
	}
    str << "</response-time>";
	req->setStatus(200);
	req->write(str.rdbuf());
}

void
ResponseTimeHandler::add(const std::string &handler, unsigned short status, std::uint64_t time) {
	std::lock_guard<std::mutex> lock(mutex_);
	CounterMapType& handle = data_[handler];
	auto it = handle.find(status);
	if (handle.end() == it) {
		std::shared_ptr<CounterData> counter(new CounterData);
		counter->add(time);
		handle.insert(std::make_pair(status, counter));
	} else {
		it->second->add(time);
	}
}

} //namespace fastcgi

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("statistics", fastcgi::ResponseTimeHandler)
FCGIDAEMON_REGISTER_FACTORIES_END()
