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
#include "fastcgi3/stream.h"
#include "fastcgi3/request.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

RequestStream::RequestStream(Request *req)
: request_(req), stream_(req->getResponseStream()) {
}

RequestStream::~RequestStream() {
}

RequestStream&
RequestStream::operator << (std::ostream& (*f)(std::ostream &os)) {
	stream_->operator << (f);
	return *this;
}


void
RequestStream::reset() {
	stream_->str(std::string());
}

void
RequestStream::flush() {
    if (stream_->rdbuf()->in_avail()) {
        request_->write(stream_->rdbuf());
    }
}

} // namespace fastcgi
