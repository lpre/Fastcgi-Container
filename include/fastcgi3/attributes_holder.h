// Fastcgi Container - framework for development of high performance FastCGI applications in C++
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

#ifndef INCLUDE_FASTCGI3_ATTRIBUTES_HOLDER_H_
#define INCLUDE_FASTCGI3_ATTRIBUTES_HOLDER_H_

#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <vector>
#include <functional>

#include "core/any.hpp"

namespace fastcgi
{

class AttributesHolder {
public:
	enum class ListenerEventType {
		UPDATE_ATTRIBUTE,
		REMOVE_ATTRIBUTE
	};

	using ListenerType = std::function<void(const AttributesHolder*, ListenerEventType, const std::string&, const core::any&)>;

public:
	AttributesHolder(bool invokeListeners=false);
	virtual ~AttributesHolder();

	AttributesHolder(const AttributesHolder&) = delete;
	AttributesHolder& operator=(const AttributesHolder&) = delete;

	template<class T> void
	setAttribute(const std::string &name, const T& value) {
		T _value = value;
		setAttributeInternal(name, std::move(_value));
	}

	template<class T> T
	getAttribute(const std::string &name) const {
		core::any attr = getAttribute(name);
		if (!attr.empty()) {
			return core::any_cast<T>(attr);
		}
		throw std::runtime_error("Attribute not found");
	}

	template<class T> T
	getAttribute(const std::string &name, const T &defaultValue) const {
		core::any attr = getAttribute(name);
		if (!attr.empty()) {
			return core::any_cast<T>(attr);
		}
		return defaultValue;
	}

	virtual void setAttribute(const std::string &name, const core::any &value);
	virtual core::any getAttribute(const std::string &name) const;
	virtual bool hasAttribute(const std::string &name) const;
	virtual void removeAttribute(const std::string& name);
	virtual void removeAllAttributes();

	std::type_info const& type(const std::string &name) const;

	std::size_t addListener(ListenerType f);
	void removeListener(std::size_t index);

protected:
	void setAttributeInternal(const std::string &name, const core::any &attr);

	mutable std::mutex mutex_;
	std::map<std::string, core::any> attributes_;

	std::vector<ListenerType> listeners_;
	bool invokeListeners_;
};


} // namespace fastcgi


#endif /* INCLUDE_FASTCGI3_ATTRIBUTES_HOLDER_H_ */
