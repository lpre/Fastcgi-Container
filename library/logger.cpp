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

#include <ctime>
#include <cstdarg>
#include <stdexcept>
#include <strings.h>
#include <string>
#include <algorithm>

#include "fastcgi3/util.h"
#include "fastcgi3/config.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/request.h"
#include "fastcgi3/stream.h"

#include <syslog.h>

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

Logger::Logger() {
	level_ = Level::DEBUG;
}

Logger::~Logger() {
}

void
Logger::exiting(const char *function) {
	debug("exiting %s\n", function);
}

void
Logger::entering(const char *function) {
	debug("entering %s\n", function);
}

void
Logger::info(const char *format, ...) {
	va_list args;
	va_start(args, format);
	log(Level::INFO, format, args);
	va_end(args);
}

void
Logger::debug(const char *format, ...) {
	va_list args;
	va_start(args, format);
	log(Level::DEBUG, format, args);
	va_end(args);
}

void
Logger::error(const char *format, ...) {
	va_list args;
	va_start(args, format);
	log(Level::ERROR, format, args);
	va_end(args);
}

void
Logger::emerg(const char *format, ...) {
	va_list args;
	va_start(args, format);
	log(Level::EMERGENCY, format, args);
	va_end(args);
}

Logger::Level Logger::getLevel() const {
	return level_;
}

void Logger::setLevel(const Logger::Level level) {
	setLevelInternal(level);
	level_ = level;
}

Logger::Level Logger::stringToLevel(const std::string &name) {
	std::string _name = name;
	std::transform(_name.begin(), _name.end(), _name.begin(), ::tolower);

    if ("info"==_name) {
        return Level::INFO;
    } else if ("debug"==_name) {
        return Level::DEBUG;
    } else if ("error"==_name) {
        return Level::ERROR;
    } else if ("emerg"==_name) {
        return Level::EMERGENCY;
    } else {
    	fprintf(stderr, "Bad string to log level cast: %s\n", name.c_str());
	    throw std::runtime_error("bad string to log level cast");
	}	
}

std::string Logger::levelToString(const Level level) {
	switch (level) {
	case Level::INFO:
		return "INFO";
	case Level::DEBUG:
		return "DEBUG";
	case Level::ERROR:
		return "ERROR";
	case Level::EMERGENCY:
		return "EMERG";
	default:
    	fprintf(stderr, "Bad log level to string cast: %d\n", level);
		throw std::runtime_error("bad log level to string cast");
	} 
}

void Logger::setLevelInternal(const Level level) {
}

void Logger::rollOver() {
}

LoggerRequestId::LoggerRequestId() {
}

LoggerRequestId::~LoggerRequestId() {
}

BulkLogger::BulkLogger() {
}

void BulkLogger::log(Level level, const char *format, va_list args) {
}

} // namespace fastcgi
