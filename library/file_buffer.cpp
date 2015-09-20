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

// #include "settings.h"

#include <stdexcept>

#include "details/data_buffer_impl.h"
#include "details/file_buffer.h"
#include "fastcgi3/range.h"

#include "fastcgi3/util.h"


#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

FileBuffer::FileBuffer()
: window_(0) {
}

FileBuffer::FileBuffer(const char *name, std::uint64_t window)
: file_(new MMapFile(name, window)), holder_(new FileHolder(name)), window_(file_->window()) {
}

FileBuffer::~FileBuffer() {
}

char
FileBuffer::at(std::uint64_t pos) {
	std::lock_guard<std::mutex> lock(mutex_);
	return file_->at(pos);
}

std::uint64_t
FileBuffer::read(std::uint64_t pos, char *data, std::uint64_t len) {
	if (pos + len > size()) {
		throw std::runtime_error("Data is out of range");
	}
	std::lock_guard<std::mutex> lock(mutex_);
	std::uint64_t read_len = len;
	while (read_len > 0) {
		std::pair<char*, std::uint64_t> cur_chunk = chunk(pos);
		if (nullptr == cur_chunk.first || 0 == cur_chunk.second) {
			throw std::runtime_error("Cannot fetch chunk");
		}
		std::uint64_t cur_len = std::min(cur_chunk.second, read_len);
		memcpy(data, cur_chunk.first, cur_len);
		pos += cur_len;
		data += cur_len;
		read_len -= cur_len;
	}
	return len;
}

std::uint64_t
FileBuffer::write(std::uint64_t pos, const char *data, std::uint64_t len) {
	if (pos + len > size()) {
		throw std::runtime_error("Data is out of range");
	}
	std::lock_guard<std::mutex> lock(mutex_);
	std::uint64_t write_len = len;
	while (write_len > 0) {
		std::pair<char*, std::uint64_t> cur_chunk = chunk(pos);
		if (nullptr == cur_chunk.first || 0 == cur_chunk.second) {
			throw std::runtime_error("Cannot fetch chunk");
		}
		std::uint64_t cur_len = std::min(cur_chunk.second, write_len);
		memcpy(cur_chunk.first, data, cur_len);
		pos += cur_len;
		data += cur_len;
		write_len -= cur_len;
	}
	return len;
}

std::uint64_t
FileBuffer::find(std::uint64_t begin, std::uint64_t end, const char* buf, std::uint64_t len) {
	if (len > end - begin) {
		return end;
	}
	std::uint64_t segment = begin / window_;
	std::uint64_t segment_pos = window_ * segment;
	std::uint64_t offset = begin - segment_pos;
	bool finish = false;
	std::lock_guard<std::mutex> lock(mutex_);
	while (1) {
		std::uint64_t pos = segment_pos + offset;
		std::uint64_t length = window_ - offset + len;
		if (pos + length >= end) {
			length = end - pos;
			finish = true;
		}

		char* range = file_->atRange(pos, length);
		Range base(range, range + length);
		Range substr(buf, buf + len);

		const char* res = base.find(substr);
		if (res != base.end()) {
			return (res - base.begin()) + pos;
		}

		if (finish) {
			break;
		}

		offset = 0;
		segment_pos += window_;
	}
	return end;
}

std::pair<std::uint64_t, std::uint64_t>
FileBuffer::trim(std::uint64_t begin, std::uint64_t end) const {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		while (begin != end && isspace(file_->at(begin))) {
			++begin;
		}
	}
	while (begin != end && isspace(end - 1)) {
		--end;
	}
	return std::make_pair(begin, end);
}

std::pair<char*, std::uint64_t>
FileBuffer::chunk(std::uint64_t pos) const {
	return file_->atSegment(pos);
}

std::pair<std::uint64_t, std::uint64_t>
FileBuffer::segment(std::uint64_t pos) const {
	std::uint64_t beg = window_ * (pos / window_);
	return std::pair<std::uint64_t, std::uint64_t>(pos, beg + window_);
}

std::uint64_t
FileBuffer::size() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return file_->size();
}

void
FileBuffer::resize(std::uint64_t size) {
	std::lock_guard<std::mutex> lock(mutex_);
	file_->resize(size);
}

const std::string&
FileBuffer::filename() const {
	return holder_.get() ? holder_->filename : StringUtils::EMPTY_STRING;
}

DataBufferImpl*
FileBuffer::getCopy() const {
	std::unique_ptr<FileBuffer> buffer(new FileBuffer);
	buffer->file_.reset(file_->clone());
	buffer->holder_ = holder_;
	buffer->window_ = window_;
	return buffer.release();
}

} // namespace fastcgi
