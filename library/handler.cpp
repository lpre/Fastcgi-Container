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

#include "details/handler_context.h"
#include "details/handlerset.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

HandlerContext::~HandlerContext() {
}
	
core::any HandlerContextImpl::getParam(const std::string &name) const {
	return getAttribute(name);
}

void HandlerContextImpl::setParam(const std::string &name, const core::any &value) {
	setAttribute(name, value);
}
	
Handler::Handler() {
}

Handler::~Handler() {
}

void
Handler::onThreadStart() {
}



Filter::Filter() {
}

Filter::~Filter() {
}

void
Filter::onThreadStart() {
}



} // namespace fastcgi
