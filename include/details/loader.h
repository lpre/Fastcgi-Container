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

#ifndef _FASTCGI_DETAILS_LOADER_H_
#define _FASTCGI_DETAILS_LOADER_H_

#include <string>
#include <vector>

#include "fastcgi3/component_factory.h"

namespace fastcgi
{

class Config;
class ComponentFactory;

class Loader {
public:
	Loader();
	virtual ~Loader();

	Loader(const Loader&) = delete;
	Loader& operator=(const Loader&) = delete;

	virtual void init(const Config *config);
	virtual ComponentFactory *findComponentFactory(const std::string &type) const;
	
protected:
	virtual void load(const char *name, const char *path);
	void checkLoad(const char *err);

private:
	std::vector<void*> handles_;
	FactoryMap factories_;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_LOADER_H_
