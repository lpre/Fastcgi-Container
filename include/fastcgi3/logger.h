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

#ifndef _FASTCGI_LOGGER_H_
#define _FASTCGI_LOGGER_H_

#include <cstdarg>

namespace fastcgi
{

class Logger {
public:
	enum class Level {
		DEBUG, INFO, ERROR, EMERGENCY
	};
	
public:
	Logger();
	virtual ~Logger();

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	void exiting(const char *function);
	void entering(const char *function);

	Level getLevel() const;
	void setLevel(const Level level);

	static Level stringToLevel(const std::string &);
	static std::string levelToString(const Level);

	virtual void info(const char *format, ...);
	virtual void debug(const char *format, ...);
	virtual void error(const char *format, ...);
	virtual void emerg(const char *format, ...);

	virtual void log(const Level level, const char *format, va_list args) = 0;

protected:
	virtual void setLevelInternal(const Level level);
	virtual void rollOver();

private:
	Level level_;
};

class LoggerRequestId
{
public:
	LoggerRequestId();
	virtual ~LoggerRequestId();

	virtual void setRequestId(const std::string &id) = 0;
	virtual std::string getRequestId() = 0;
};


class BulkLogger : public Logger {
public:
	BulkLogger();

protected:
	virtual void log(const Level level, const char *format, va_list args);
};

} // namespace fastcgi

#endif // _FASTCGI_LOGGER_H_
