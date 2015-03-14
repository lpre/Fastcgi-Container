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

#ifndef _FASTCGI_COMPONENT_FACTORY_H_
#define _FASTCGI_COMPONENT_FACTORY_H_

#include <string>
#include <map>
#include <memory>

namespace fastcgi
{

class Component;
class ComponentFactory;
class ComponentContext;
	
using FactoryMap = std::map<std::string, ComponentFactory*>;

class ComponentFactory {
public:
	ComponentFactory();
	virtual ~ComponentFactory();

	ComponentFactory(const ComponentFactory&) = delete;
	ComponentFactory& operator=(const ComponentFactory&) = delete;

	virtual std::shared_ptr<Component> createComponent(std::shared_ptr<ComponentContext> context) = 0;
}; 

template<typename T>
class DefaultComponentFactory : public ComponentFactory {
public:
	virtual std::shared_ptr<Component> createComponent(std::shared_ptr<ComponentContext> context) override {
		return std::make_shared<T>(context);
	}
	
	virtual ~DefaultComponentFactory() override {
	}
};

} // namespace fastcgi

using FastcgiGetFactoryMapFunction = fastcgi::FactoryMap* (*)();

#if __GNUC__ >= 4
#	define FCGIDAEMON_DSO_GLOBALLY_VISIBLE \
		__attribute__ ((visibility ("default")))
#else
#	define FCGIDAEMON_DSO_GLOBALLY_VISIBLE
#endif

#define FCGIDAEMON_REGISTER_FACTORIES_BEGIN() \
	extern "C" FCGIDAEMON_DSO_GLOBALLY_VISIBLE \
	const fastcgi::FactoryMap* getFactoryMap() { \
		static fastcgi::FactoryMap m;
			        
#define FCGIDAEMON_ADD_DEFAULT_FACTORY(name, Type) \
		m.insert(std::make_pair((name), new fastcgi::DefaultComponentFactory<Type>));

#define FCGIDAEMON_ADD_FACTORY(name, factory) \
		m.insert(std::make_pair((name), (factory)));

#define FCGIDAEMON_REGISTER_FACTORIES_END() \
	    return &m; \
	}

#endif //_FASTCGI_COMPONENT_FACTORY_H_

