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

#include "settings.h"

#include "details/component_context.h"
#include "details/componentset.h"
#include "details/globals.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

ComponentContextImpl::ComponentContextImpl(const Globals *globals, const std::string &componentXPath)
: globals_(globals), componentXPath_(componentXPath) {
}
	
ComponentContextImpl::~ComponentContextImpl() {
}

const Config*
ComponentContextImpl::getConfig() const {
    return globals_->config();
}

const Globals*
ComponentContextImpl::globals() const {
    return globals_;
}

std::string
ComponentContextImpl::getComponentXPath() const {
    return componentXPath_;
}

std::shared_ptr<Component>
ComponentContextImpl::findComponentInternal(const std::string &name) const {
    return globals_->components()->find(name);
}

} // namespace fastcgi
