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

#include <ctime>
#include <cstdio>
#include <cstdarg>
#include <stdexcept>

#include <log4cxx/patternlayout.h>

#include "fastcgi3/config.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/component_factory.h"
#include "fastcgi3/request.h"
#include "fastcgi3/stream.h"
#include "logger.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#if defined(HAVE_VA_COPY)
#define VA_COPY(a,b) va_copy((a), (b))
#elif defined(HAVE_NONSTANDARD_VA_COPY)
#define VA_COPY(a,b) __va_copy((a), (b))
#endif

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("logger", fastcgi::DefaultLogger)
FCGIDAEMON_REGISTER_FACTORIES_END()

namespace fastcgi
{

DefaultLogger::DefaultLogger(ComponentContext *context) 
	: Component(context), logger_(log4cxx::Logger::getRootLogger())
{
	const Config *config = context->getConfig();
	const std::string componentXPath = context->getComponentXPath();

	std::string layoutPattern = config->asString(componentXPath + "/pattern", "DEFAULT");

    log4cxx::LayoutPtr layout(new log4cxx::PatternLayout(
				"DEFAULT" == layoutPattern ? log4cxx::PatternLayout::TTCC_CONVERSION_PATTERN : layoutPattern));

    logger_ = log4cxx::Logger::getLogger(config->asString(componentXPath + "/ident"));

	const std::string logFileName = config->asString(componentXPath + "/file");
    appender_ = log4cxx::helpers::ObjectPtrT<log4cxx::RollingFileAppender>(new log4cxx::RollingFileAppender(layout, logFileName));

    appender_->setMaxFileSize("2000MB");

    logger_->addAppender(appender_);

    setLevel(stringToLevel(config->asString(componentXPath + "/level")));	
}

DefaultLogger::~DefaultLogger() {
}

void 
DefaultLogger::onLoad() {
}

void
DefaultLogger::onUnload() {
}

void
DefaultLogger::handleRequest(Request *request, HandlerContext *handlerContext) {
	request->setContentType("text/plain");
    const std::string &action = request->getArg("action");
    if ("setlevel" == action) {
        const std::string &l = request->getArg("level");
        setLevel(stringToLevel(l));
        RequestStream(request) << "level " << l << "successfully set" << std::endl;
    }
    else if ("rollover" == action) {
        rollOver();
        RequestStream(request) << "rollover successful" << std::endl;
    }
    else {
        RequestStream(request) << "bad action" << std::endl;
    }
}

void
DefaultLogger::rollOver() {
	appender_->rollOver();
}

void
DefaultLogger::setLevelInternal(const Level level) {
	logger_->setLevel(toLog4cxxLevel(level));
}

void
DefaultLogger::log(const Level level, const char *format, va_list args) {
	log4cxx::LevelPtr log4cxxLevel = toLog4cxxLevel(level);
	if (logger_->isEnabledFor(log4cxxLevel)) {
		va_list tmpargs;
		VA_COPY(tmpargs, args);
		size_t size = vsnprintf(nullptr, 0, format, tmpargs);
		va_end(tmpargs);
		if (size > 0) {
			std::vector<char> data(size + 1);
			vsnprintf(&data[0], size + 1, format, args);
			logger_->log(log4cxxLevel, std::string(data.begin(), data.begin() + size));
		}
	}
}

log4cxx::LevelPtr
DefaultLogger::toLog4cxxLevel(const Level level) {
	switch (level) {
		case INFO:
			return log4cxx::Level::INFO;
		case DEBUG:
			return log4cxx::Level::DEBUG;
		case ERROR:
			return log4cxx::Level::ERROR;
		case EMERGENCY:
			return log4cxx::Level::FATAL;
		default:
			fprintf(stderr, "toLog4cxxLevel: unknown log level %d\n", level);
			throw std::logic_error("toLog4cxxLevel: unknown log level");
	}
}

} // namespace fastcgi
