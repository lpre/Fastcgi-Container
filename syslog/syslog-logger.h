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

#ifndef _FASTCGI_SYSLOG_LOGGER_H_
#define _FASTCGI_SYSLOG_LOGGER_H_

#include "fastcgi3/logger.h"
#include "fastcgi3/component.h"
#include "fastcgi3/handler.h"

#include <memory>
#include <string>

namespace fastcgi
{

class SyslogLogger : virtual public Logger, virtual public Component, virtual public Handler, virtual public LoggerRequestId
{
public:
	SyslogLogger(std::shared_ptr<ComponentContext> context);
	virtual ~SyslogLogger();

	virtual void onLoad() override;
	virtual void onUnload() override;

	virtual void handleRequest(Request *request, HandlerContext *handlerContext) override;
	
	virtual void setRequestId(const std::string &id);
	virtual std::string getRequestId();

protected:
	virtual void log(const Level level, const char *format, va_list args);
	virtual void setLevelInternal(const Level level);
	virtual void rollOver();

private:
	static int toSyslogPriority(const Level level);

private:
	std::string ident_;
	int priority_;
	bool requestSpecificIdent_;
	thread_local static std::unique_ptr<std::string> threadIdent_;
};

} // namespace fastcgi

#endif // _FASTCGI_SYSLOG_LOGGER_H_
