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

#include <stdexcept>
#include <libxml/xmlerror.h>

#include "details/xml.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

XmlUtils::XmlUtils() {

	xmlInitParser();
	
	xmlLoadExtDtdDefaultValue = 1;
	xmlSubstituteEntitiesDefault(0);
}

XmlUtils::~XmlUtils() {
	xmlCleanupParser();
}

void
XmlUtils::throwUnless(bool value) {
	if (!value) {
		const char *message = "unknown xml error";
		xmlErrorPtr err =xmlGetLastError();
		if (err && err->message) {
			message = err->message;
		}
		std::runtime_error exc(message);
		xmlResetLastError();
		throw exc;
	}
}

const char*
XmlUtils::value(xmlNodePtr node) {
	assert(node);
	xmlNodePtr child = node->children;
	if (child && xmlNodeIsText(child) && child->content) {
		return (const char*) child->content;
	}
	return nullptr;
}

const char*
XmlUtils::value(xmlAttrPtr attr) {
	assert(attr);
	xmlNodePtr child = attr->children;
	if (child && xmlNodeIsText(child) && child->content) {
		return (const char*) child->content;
	}
	return nullptr;
}

const char*
XmlUtils::attrValue(xmlNodePtr node, const char *name) {
	assert(node);
	xmlAttrPtr attr = xmlHasProp(node, (const xmlChar*) name);
	return attr ? value(attr) : nullptr;
}

void
XmlDocCleaner::clean(xmlDocPtr doc) {
	xmlFreeDoc(doc);
}

void
XmlNodeCleaner::clean(xmlNodePtr node) {
	xmlFreeNode(node);
}

void
XmlXPathObjectCleaner::clean(xmlXPathObjectPtr obj) {
	xmlXPathFreeObject(obj);
}

void
XmlXPathContextCleaner::clean(xmlXPathContextPtr ctx) {
	xmlXPathFreeContext(ctx);
}

} // namespace fastcgi
