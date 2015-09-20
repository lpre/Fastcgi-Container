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

#include <functional>

#include "fastcgi3/security_principal.h"

namespace fastcgi
{

namespace security
{

Principal::Principal(const std::string &name)
: name_(name) {
}

Principal::~Principal() {

}

bool
Principal::operator==(const Principal& p) {
    return getHashCode()==p.getHashCode();
}

bool
Principal::operator!=(const Principal& p) {
    return !(getHashCode()==p.getHashCode());
}

bool
Principal::operator==(std::size_t hashCode) {
    return getHashCode()==hashCode;
}

bool
Principal::operator!=(std::size_t hashCode) {
    return !(getHashCode()==hashCode);
}

bool
Principal::operator==(const std::string& name) {
    return getHashCode()==std::hash<std::string>()(name);
}

bool
Principal::operator!=(const std::string& name) {
    return !(getHashCode()==std::hash<std::string>()(name));
}

std::size_t
Principal::getHashCode() const {
	return std::hash<std::string>()(getName());
}

const std::string&
Principal::getName() const {
	return name_;
}

}

}

