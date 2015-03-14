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

#ifndef _FASTCGI_LOG4CXX_LOGGER_H_
#define _FASTCGI_LOG4CXX_LOGGER_H_

#include <log4cxx/logger.h>
#include <log4cxx/rollingfileappender.h>

#include "fastcgi3/logger.h"
#include "fastcgi3/component.h"
#include "fastcgi3/handler.h"

namespace fastcgi
{

class DefaultLogger : virtual public Logger, virtual public Component, virtual public Handler
{
public:
	DefaultLogger(ComponentContext *context);
	virtual ~DefaultLogger();

	virtual void onLoad();
	virtual void onUnload();

	virtual void handleRequest(Request *request, HandlerContext *handlerContext);
	
protected:
	virtual void log(const Level level, const char *format, va_list args);
	virtual void setLevelInternal(const Level level);
	virtual void rollOver();

private:
	static log4cxx::LevelPtr toLog4cxxLevel(const Level level);

private:
	log4cxx::LoggerPtr logger_;
	log4cxx::helpers::ObjectPtrT<log4cxx::RollingFileAppender> appender_;
};

} // namespace fastcgi

#endif // _FASTCGI_LOG4CXX_LOGGER_H_
