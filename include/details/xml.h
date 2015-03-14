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

#ifndef _FASTCGI_DETAILS_XML_H_
#define _FASTCGI_DETAILS_XML_H_

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "fastcgi3/helpers.h"

namespace fastcgi
{

class XmlUtils
{
public:
	XmlUtils();
	virtual ~XmlUtils();
	
	XmlUtils(const XmlUtils&) = delete;
	XmlUtils& operator=(const XmlUtils&) = delete;

	static void throwUnless(bool value);
	static const char* value(xmlAttrPtr node);
	static const char* value(xmlNodePtr node);
	static const char* attrValue(xmlNodePtr node, const char *name);
};

struct XmlDocCleaner
{
	static void clean(xmlDocPtr doc);
};

struct XmlNodeCleaner
{
	static void clean(xmlNodePtr node);
};

struct XmlXPathObjectCleaner
{
	static void clean(xmlXPathObjectPtr obj);
};

struct XmlXPathContextCleaner
{
	static void clean(xmlXPathContextPtr ctx);
};

using XmlDocHelper = Helper<xmlDocPtr, XmlDocCleaner>;
using XmlNodeHelper = Helper<xmlNodePtr, XmlNodeCleaner>;
using XmlXPathObjectHelper = Helper<xmlXPathObjectPtr, XmlXPathObjectCleaner>;
using XmlXPathContextHelper = Helper<xmlXPathContextPtr, XmlXPathContextCleaner>;

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_XML_H_
