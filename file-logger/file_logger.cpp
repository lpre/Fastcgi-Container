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

#include "settings.h"

#include "file_logger.h"

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <functional>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>


#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi {

const size_t BUF_SIZE = 512;

FileLogger::FileLogger(std::shared_ptr<ComponentContext> context) : Component(context),
        open_mode_(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH),
        print_level_(true), print_time_(true),
        fd_(-1), stopping_(false),
        writing_thread_(std::bind(&FileLogger::writingThread, this))
{
    const Config *config = context->getConfig();
    const std::string componentXPath = context->getComponentXPath();

    filename_ = config->asString(componentXPath + "/file");
    setLevel(stringToLevel(config->asString(componentXPath + "/level")));

    print_level_ = (0 == strcasecmp(config->asString(componentXPath + "/print-level", "yes").c_str(), "yes"));

    print_time_ = (0 == strcasecmp(config->asString(componentXPath + "/print-time", "yes").c_str(), "yes"));

    time_format_ = config->asString(componentXPath + "/time-format", "[%Y/%m/%d %T]");

    std::string read = config->asString(componentXPath + "/read", "");
    if (!read.empty()) {
        if (read == "all") {
            open_mode_ = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        }
        else if (read == "group") {
            open_mode_ = S_IRUSR | S_IWUSR | S_IRGRP;
        }
        else if (read == "user") {
            open_mode_ = S_IRUSR | S_IWUSR;
        }
    }

    std::string::size_type pos = 0;
    while (true) {
        pos = filename_.find('/', pos + 1);
        if (std::string::npos == pos) {
            break;
        }
        std::string name = filename_.substr(0, pos);
        int res = mkdir(name.c_str(), open_mode_ | S_IXUSR | S_IXGRP | S_IXOTH);
        if (-1 == res && EEXIST != errno) {
            std::cerr << "failed to create dir: " << name << ". Errno: " << errno << std::endl;
        }
    }
    
    openFile();
}

FileLogger::~FileLogger() {
    stopping_ = true;
    queue_condition_.notify_one();
    writing_thread_.join();

    if (-1 != fd_) {
        close(fd_);
    }
}

void
FileLogger::onLoad() {
}

void
FileLogger::onUnload() {
}

void FileLogger::openFile() {
	std::lock_guard<std::mutex> fdLock(fd_mutex_);
    if (-1 != fd_) {
        close(fd_);
    }
    fd_ = open(filename_.c_str(), O_WRONLY | O_CREAT | O_APPEND, open_mode_);
    if (-1 == fd_) {
        std::cerr << "File logger cannot open file for writing: " << filename_ << std::endl;
    }
}

void
FileLogger::rollOver() {
    openFile();
}

void
FileLogger::log(const Logger::Level level, const char* format, va_list args) {
    if (level < getLevel()) {
        return;
    }

    // Check without lock!
    if (fd_ == -1) {
        return;
    }

    char fmt[BUF_SIZE];
    prepareFormat(fmt, sizeof(fmt), level, format);

    va_list tmpargs;
    va_copy(tmpargs, args);
    size_t size = vsnprintf(nullptr, 0, fmt, tmpargs);
    va_end(tmpargs);

    if (size > 0) {
        std::vector<char> data(size + 1);
        vsnprintf(&data[0], size + 1, fmt, args);
        std::unique_lock<std::mutex> lock(queue_mutex_);
        queue_.push_back(std::string(data.begin(), data.begin() + size));
        queue_condition_.notify_one();
    }
}

void
FileLogger::prepareFormat(char * buf, size_t size, const Logger::Level level, const char* format) {
    char timestr[64];
    if (print_time_) {
        struct tm tm;
        time_t t;
        time(&t);
        localtime_r(&t, &tm);
        strftime(timestr, sizeof(timestr) - 1, time_format_.c_str(), &tm);
    }

    std::string level_str;
    if (print_level_) {
        level_str = levelToString(level);
    }


    if (print_time_ && print_level_) {
        snprintf(buf, size - 1, "%s %s: %s\n", timestr, level_str.c_str(), format);
    }
    else if (print_time_) {
        snprintf(buf, size - 1, "%s %s\n", timestr, format);
    }
    else if (print_level_) {
        snprintf(buf, size - 1, "%s: %s\n", level_str.c_str(), format);
    }
    else {
        snprintf(buf, size - 1, "%s\n", format);
    }
    buf[size - 1] = '\0';
}

void
FileLogger::writingThread() {
    while (!stopping_) {
        std::vector<std::string> queueCopy;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_condition_.wait(lock);
            std::swap(queueCopy, queue_);
        }

        std::lock_guard<std::mutex> fdlock(fd_mutex_);
        if (-1 != fd_) {
            for (auto& i : queueCopy) {
                size_t wrote = 0;
                while (wrote < i.length()) {
                	int res = ::write(fd_, i.c_str() + wrote, i.length() - wrote);
                	if (res < 0) {
                		std::cerr << "Failed to write to log " << filename_ << " : " << strerror(errno) << std::endl;
                		break;
                	} else {
                		wrote += res;
                	}
                }

            }
        }
    }
}

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("logger", fastcgi::FileLogger)
FCGIDAEMON_REGISTER_FACTORIES_END()

} // namespace xscript
