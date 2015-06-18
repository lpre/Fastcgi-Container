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

#include <stdexcept>

#include "fastcgi3/range.h"
#include "details/string_buffer.h"

#include "fastcgi3/util.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

StringBuffer::StringBuffer(const char *data, std::uint64_t size)
: data_(new std::vector<char>(data, data + size)) {
}

StringBuffer::~StringBuffer() {
}

std::uint64_t
StringBuffer::read(std::uint64_t pos, char *data, std::uint64_t len) {
	memcpy(data, &((*data_)[pos]), len);
	return len;
}

std::uint64_t
StringBuffer::write(std::uint64_t pos, const char *data, std::uint64_t len) {
	memcpy(&((*data_)[pos]), data, len);
	return len;
}

char
StringBuffer::at(std::uint64_t pos) {
	return data_->at(pos);
}

std::uint64_t
StringBuffer::find(std::uint64_t begin, std::uint64_t end, const char* buf, std::uint64_t len) {
	if (len > end - begin) {
		return end;
	}
	char* first = &((*data_)[0]);
	Range base(first + begin, first + end);
	Range substr(buf, buf + len);
	return base.find(substr) - first;
}

std::pair<std::uint64_t, std::uint64_t>
StringBuffer::trim(std::uint64_t begin, std::uint64_t end) const {
	char* first = &((*data_)[0]);
	Range base(first + begin, first + end);
	Range trimmed = base.trim();
	return std::pair<std::uint64_t, std::uint64_t>(trimmed.begin() - first, trimmed.end() - first);
}

std::pair<char*, std::uint64_t>
StringBuffer::chunk(std::uint64_t pos) const {
	return std::pair<char*, std::uint64_t>(&((*data_)[0]) + pos, data_->size() - pos);
}

std::pair<std::uint64_t, std::uint64_t>
StringBuffer::segment(std::uint64_t pos) const {
	return std::pair<std::uint64_t, std::uint64_t>(pos, size());
}

std::uint64_t
StringBuffer::size() const {
	return data_->size();
}

void
StringBuffer::resize(std::uint64_t size) {
	data_->resize(size);
}

const std::string&
StringBuffer::filename() const {
	return StringUtils::EMPTY_STRING;
}

DataBufferImpl*
StringBuffer::getCopy() const {
	return new StringBuffer(*this);
}

} // namespace fastcgi
