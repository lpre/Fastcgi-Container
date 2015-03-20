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

#ifndef _FASTCGI_REQUEST_CACHE_MMAP_FILE_H_
#define _FASTCGI_REQUEST_CACHE_MMAP_FILE_H_

#include <unistd.h>
#include <memory>

namespace fastcgi
{

class FileDescriptor {
public:
	FileDescriptor(int fdes)
	: fdes_(fdes) {
	}
	~FileDescriptor() {
		if (-1 != fdes_) {
			close(fdes_);
		}
	}

	int value() const {
		return fdes_;
	}
private:
	int fdes_;
};

class MMapFile {
public:
	MMapFile(const char *name, std::uint64_t window, bool is_read_only = false);
	virtual ~MMapFile();

	std::uint64_t size() const;
	bool empty() const;
	void resize(std::uint64_t newsize);

	char at(std::uint64_t index);
	char* atRange(std::uint64_t index, std::uint64_t length);
	std::pair<char*, std::uint64_t> atSegment(std::uint64_t index);

	std::uint64_t window() const;

	MMapFile* clone() const;

private:
	MMapFile();
	void checkWindow();
	void unmap();
	char* map_segment(std::uint64_t segment);
	char* map_range(std::uint64_t begin, std::uint64_t end);
	void check_index(std::uint64_t index) const;
	bool mapped(std::uint64_t index) const;
	bool mapped(std::uint64_t begin, std::uint64_t end) const;

private:
	void *pointer_;
	std::uint64_t size_;
	std::shared_ptr<FileDescriptor> fdes_;
	bool is_read_only_;
	std::uint64_t window_;
	std::uint64_t segment_start_, segment_len_;
	int page_size_;
};

} // namespace fastcgi

#endif // _FASTCGI_REQUEST_CACHE_MMAP_FILE_H_
