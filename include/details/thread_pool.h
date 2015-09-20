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

#ifndef _FASTCGI_DETAILS_THREAD_POOL_H_
#define _FASTCGI_DETAILS_THREAD_POOL_H_

#include <algorithm>
#include <queue>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace fastcgi {

struct ThreadPoolInfo
{
	bool started;
	uint64_t threadsNumber;
	uint64_t queueLength;
	uint64_t busyThreadsCounter;
	uint64_t currentQueue;
	uint64_t goodTasksCounter;
	uint64_t badTasksCounter;
};

template<typename T>
class ThreadPool {
public:
	using TaskType = T;
	using InitFuncType = std::function<void()>;

public:
	ThreadPool(const unsigned threadsNumber, const unsigned queueLength) {
		info_.started = false;
		info_.threadsNumber = threadsNumber;
		info_.queueLength = queueLength;
		info_.busyThreadsCounter = 0;
		info_.currentQueue = 0;
		info_.goodTasksCounter = 0;
		info_.badTasksCounter = 0;
	}

	virtual ~ThreadPool() {
		stop();
		join();
	}

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	void start(InitFuncType func) {
		std::unique_lock<std::mutex> lock(mutex_);
		if (info_.started) {
			return;
		}
//		if (threads_.size() != 0) {
//			throw std::runtime_error("Invalid thread pool state.");
//		}

		std::function<void()> f = std::bind(&ThreadPool<T>::workMethod, this, func);
		for (unsigned i = 0; i < info_.threadsNumber; ++i) {
			threads_.push_back(std::unique_ptr<std::thread>(new std::thread(f)));
		}

		info_.started = true;
	}

	void stop() {
		std::unique_lock<std::mutex> lock(mutex_);
		info_.started = false;
		condition_.notify_all();
	}

	void join() {
		// join_all
		std::for_each(threads_.begin(), threads_.end(), [](std::unique_ptr<std::thread>& t){t->join();});
	}

	void addTask(T task) {
		try {
			std::unique_lock<std::mutex> lock(mutex_);

			if (!info_.started) {
				throw std::runtime_error("Thread pool is not started yet");
			}

			if (tasksQueue_.size() >= info_.queueLength) {
				throw std::runtime_error("Pool::handle: the queue has already reached its maximum size of " + std::to_string(info_.queueLength) + " elements");
			}
			tasksQueue_.push(task);
		} catch (...) {
			condition_.notify_one();
			throw;
		}
		condition_.notify_one();
	}

	ThreadPoolInfo getInfo() const {
		std::unique_lock<std::mutex> lock(mutex_);
		info_.currentQueue = tasksQueue_.size();
		return info_;
	}

protected:
	virtual void handleTask(T) = 0;

private:
	void workMethod(InitFuncType func) {
		const int none = 0;
		const int good = 1;
		const int bad = 2;
		int state = none;

		try {
			func();
		}
		catch (...) {
		}

		while (true) {
			try {
				T task;
				{
					std::unique_lock<std::mutex> lock(mutex_);
					switch (state) {
					case none:
						break;
					case good:
						++info_.goodTasksCounter;
						--info_.busyThreadsCounter;
						break;
					case bad:
						++info_.badTasksCounter;
						--info_.busyThreadsCounter;
						break;
					}
					state = none;
					while (true) {
						if (!info_.started) {
							return;
						} else if (!tasksQueue_.empty()) {
							break;
						}
						condition_.wait(lock);
					}
					task = tasksQueue_.front();
					tasksQueue_.pop();
					++info_.busyThreadsCounter;
				}

				try {
					handleTask(task);
					state = good;
				} catch (...) {
					state = bad;
				}
			}
			catch (...)
			{
			}
		}
	}

private:
	mutable std::mutex mutex_;
	std::condition_variable condition_;
	std::vector<std::unique_ptr<std::thread>> threads_;
	std::queue<T> tasksQueue_;
	mutable ThreadPoolInfo info_;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_THREAD_POOL_H_
