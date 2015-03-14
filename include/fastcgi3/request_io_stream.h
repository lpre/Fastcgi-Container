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

#ifndef _FASTCGI_REQUEST_IO_STREAM_H_
#define _FASTCGI_REQUEST_IO_STREAM_H_

namespace fastcgi
{

class RequestIOStream {
public:
	virtual int read(char *buf, int size) = 0;
	virtual int write(const char *buf, int size) = 0;
	virtual void write(std::streambuf *buf) = 0;
};

} // namespace fastcgi

#endif // _FASTCGI_REQUEST_IO_STREAM_H_
