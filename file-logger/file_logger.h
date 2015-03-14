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

#ifndef _FASTCGI_FILE_LOGGER_H_
#define _FASTCGI_FILE_LOGGER_H_

#include <vector>
#include <string>
#include <thread>
#include <condition_variable>

#include "fastcgi3/component.h"
#include "fastcgi3/logger.h"

namespace fastcgi
{

class FileLogger : virtual public Logger, virtual public Component {
public:
    FileLogger(std::shared_ptr<ComponentContext> context);
    ~FileLogger();

	virtual void onLoad();
	virtual void onUnload();

	virtual void log(const Logger::Level level, const char *format, va_list args);
private:
	virtual void rollOver();

private:
    // File name
    std::string filename_;

    // Open mode
    mode_t open_mode_;

    // Time format specification
    std::string time_format_;

    bool print_level_;
    bool print_time_;

    // File descriptor
    int fd_;

    // Lock of file descriptor to avoid logrotate race-condition
    std::mutex fd_mutex_;


    // Writing queue.
    // All writes happens in separate thread. All someInternal methods just
    // push string into queue and signal conditional variable.

    // Logger is stopping.
    volatile bool stopping_;

    // Writing queue.
    std::vector<std::string> queue_;

    // Condition and mutex for signalling.
    std::condition_variable queue_condition_;
    std::mutex queue_mutex_;

    // Writing thread.
    std::thread writing_thread_;


    void openFile();
    void prepareFormat(char * buf, size_t size, const Logger::Level level, const char* format);

    void writingThread();
};

}

#endif // _FASTCGI_FILE_LOGGER_H_
