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

#ifndef _FASTCGI_COMPONENT_H_
#define _FASTCGI_COMPONENT_H_

#include <string>
#include <memory>

namespace fastcgi
{

class Config;
class Component;

class ComponentContext {
public:
	virtual ~ComponentContext();

//	ComponentContext(const ComponentContext&) = delete;
	ComponentContext& operator=(const ComponentContext&) = delete;
	
	virtual const Config* getConfig() const = 0;
	virtual std::string getComponentXPath() const = 0;

	template<typename T>
	std::shared_ptr<T> findComponent(const std::string &name) {
		return std::dynamic_pointer_cast<T>(findComponentInternal(name));
	}

protected:	
	virtual std::shared_ptr<Component> findComponentInternal(const std::string &name) const = 0;
};

class Component {
public:
	Component(std::shared_ptr<ComponentContext> context);
	virtual ~Component();

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	virtual void onLoad() = 0;
	virtual void onUnload() = 0;

protected:
	std::shared_ptr<ComponentContext> context();
	const std::shared_ptr<ComponentContext> context() const;

private:
	std::shared_ptr<ComponentContext> m_context;
};

} // namespace fastcgi

#endif // _FASTCGI_COMPONENT_H_
