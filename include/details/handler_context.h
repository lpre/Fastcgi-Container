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

#ifndef _FASTCGI_DETAILS_HANDLER_CONTEXT_H_
#define _FASTCGI_DETAILS_HANDLER_CONTEXT_H_

#include "fastcgi3/handler.h"
#include "fastcgi3/attributes_holder.h"

namespace fastcgi 
{

class HandlerContextImpl : public HandlerContext, virtual public AttributesHolder {
public:
	virtual core::any getParam(const std::string &name) const;
	virtual void setParam(const std::string &name, const core::any &value);
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_HANDLER_CONTEXT_H_
