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

#ifndef _FASTCGI_DETAILS_XML_CONFIG_H_
#define _FASTCGI_DETAILS_XML_CONFIG_H_

#include <regex>
#include <map>

#include <libxml/tree.h>

#include "details/xml.h"

#include "fastcgi3/config.h"
#include "fastcgi3/helpers.h"

namespace fastcgi
{

class XmlConfig : public Config
{
public:
	XmlConfig(const char *file);
	virtual ~XmlConfig();

	XmlConfig(const Config&) = delete;
	XmlConfig& operator=(const XmlConfig&) = delete;

	virtual int asInt(const std::string &value) const;
	virtual int asInt(const std::string &value, int defval) const;
	
	virtual std::string asString(const std::string &value) const;
	virtual std::string asString(const std::string &value, const std::string &defval) const;

	virtual void subKeys(const std::string &value, std::vector<std::string> &v) const;

private:
	void findVariables(const XmlDocHelper &doc);
	void resolveVariables(std::string &val) const;
	const std::string& findVariable(const std::string &key) const;
	
private:
	XmlDocHelper doc_;
	std::regex regex_;
	std::map<std::string, std::string> vars_;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_XML_CONFIG_H_
