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

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <string.h>
#include <functional>

#include <dlfcn.h>

#include "details/loader.h"
#include "fastcgi3/config.h"
#include "fastcgi3/component_factory.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

Loader::Loader() {
}

Loader::~Loader() {
	std::for_each(handles_.rbegin(), handles_.rend(), std::bind(&dlclose, std::placeholders::_1));
}

void
Loader::init(const Config *config) {	
	std::vector<std::string> v;
	std::string key("/fastcgi/modules/module");
	
	config->subKeys(key, v);
	for (auto& i : v) {
		const std::string name = config->asString(i + "/@name");
		const std::string path = config->asString(i + "/@path");
		load(name.c_str(), path.c_str());
	}
}

ComponentFactory*
Loader::findComponentFactory(const std::string &type) const {
	auto i = factories_.find(type);
	if (i != factories_.end()) {
		return i->second;
	}
	return nullptr;
}

void
Loader::load(const char *name, const char *path) {

	void *handle = nullptr;
	try {
		handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
		checkLoad(dlerror());
		
		void *libraryEntry = dlsym(handle, "getFactoryMap");
		checkLoad(dlerror());
		FastcgiGetFactoryMapFunction getFactoryMap = nullptr;
		memcpy(&getFactoryMap, &libraryEntry, sizeof(FastcgiGetFactoryMapFunction));

		FactoryMap *factoryMap = getFactoryMap();
		if (nullptr!=factoryMap) {
			for (auto& i : *factoryMap) {
				const std::string fullFactoryName = std::string(name) + ":" + i.first;
				factories_.insert(make_pair(fullFactoryName, i.second));
			}
		}
		
		handles_.push_back(handle);
		
	} catch (const std::exception &e) {
		if (nullptr != handle) {
			dlclose(handle);
		}
		throw;
	}
}

void
Loader::checkLoad(const char *err) {
	if (nullptr != err) {
		throw std::runtime_error(err);
	}
}

} // namespace fastcgi
