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

#ifndef _FASTCGI_COOKIE_H_
#define _FASTCGI_COOKIE_H_

#include <ctime>
#include <memory>
#include <string>

namespace fastcgi
{

class Cookie {
public:
	Cookie(const std::string &name, const std::string &value);
	Cookie(const Cookie &cookie);
	virtual ~Cookie();
	Cookie& operator=(const Cookie &cookie);
	bool operator < (const Cookie &cookie) const;
	
	const std::string& name() const;
	const std::string& value() const;
	
	bool secure() const;
	void secure(bool value);
	
	time_t expires() const;
	void expires(time_t value);

	void permanent(bool value);
	bool permanent() const;

	void httpOnly(bool value);
	bool httpOnly() const;

	const std::string& path() const;
	void path(const std::string &value);
	
	const std::string& domain() const;
	void domain(const std::string &value);

	std::string toString() const;

	void urlEncode(bool value);

private:
    class CookieData;
    std::unique_ptr<CookieData> data_;
};

} // namespace fastcgi

#endif // _FASTCGI_COOKIE_H_
