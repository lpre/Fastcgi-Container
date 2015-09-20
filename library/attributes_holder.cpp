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

#include <iostream>

#include "core/any.hpp"

#include "fastcgi3/attributes_holder.h"

namespace fastcgi
{

AttributesHolder::AttributesHolder(bool invokeListeners)
: invokeListeners_(invokeListeners) {

}

AttributesHolder::~AttributesHolder() {
	removeAllAttributes();
}

std::type_info const&
AttributesHolder::type(const std::string &name) const {
	return getAttribute(name).type();
}

void
AttributesHolder::setAttributeInternal(const std::string &name, const core::any &attr) {
	std::lock_guard<std::mutex> lock(mutex_);

	auto it = attributes_.find(name);
	if (it == attributes_.end()) {
		attributes_.insert({name, attr});
	} else {
		it->second = attr;
	}

	if (invokeListeners_) {
		for (auto& f : listeners_) {
			try {
				f(this, ListenerEventType::UPDATE_ATTRIBUTE, name, attr);
			} catch (...) {

			}
		}
	}
}


core::any AttributesHolder::getAttribute(const std::string &name) const {
	std::lock_guard<std::mutex> lock(mutex_);

	auto it = attributes_.find(name);
	if (it != attributes_.end()) {
		return it->second;
	}
	return core::any();
}

void AttributesHolder::setAttribute(const std::string &name, const core::any &attr) {
	setAttributeInternal(name, attr);
}

bool
AttributesHolder::hasAttribute(const std::string &name) const {
//	std::lock_guard<std::mutex> lock(mutex_);

	return attributes_.find(name) != attributes_.end();
}

void
AttributesHolder::removeAttribute(const std::string& name) {
	std::lock_guard<std::mutex> lock(mutex_);

	auto it = attributes_.find(name);
	if (it != attributes_.end()) {
		attributes_.erase(it);

		if (invokeListeners_) {
			for (auto& f : listeners_) {
				try {
					f(this, ListenerEventType::REMOVE_ATTRIBUTE, name, *it);
				} catch (...) {

				}
			}
		}

	}
}

void
AttributesHolder::removeAllAttributes() {
	std::lock_guard<std::mutex> lock(mutex_);

	attributes_.clear();
}

std::size_t
AttributesHolder::addListener(ListenerType f) {
	std::lock_guard<std::mutex> lock(mutex_);

	listeners_.push_back(f);
	return listeners_.size()-1;
}

void
AttributesHolder::removeListener(std::size_t index) {
	std::lock_guard<std::mutex> lock(mutex_);

	if (listeners_.size()>index) {
		listeners_.erase(listeners_.begin() + index);
	}
}

}



