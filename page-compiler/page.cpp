// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (c) 2008 Applied Informatics Software Engineering GmbH and Contributors (POCO Project).
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

#include <algorithm>
#include <string>

#include "page.h"

Page::Page() : fastcgi::AttributesHolder(false)
{
}


Page::~Page()
{
}


bool Page::getBool(const std::string& property, bool deflt) const
{
	if (hasAttribute(property)) {
		std::string value = getAttribute<std::string>(property);
		std::transform(value.begin(), value.end(), value.begin(), ::tolower);
		return (value=="true" || value=="yes" || value=="on");
	}
	else return deflt;
}


int Page::getInt(const std::string& property, int deflt) const
{
	if (hasAttribute(property)) {
		std::string value = getAttribute<std::string>(property);
		return std::stoi(value);
	}
	else return deflt;
}




