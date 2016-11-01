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

#include <string>
#include <cstring>
#include <regex>


#include "page.h"
#include "code_writer.h"
#include "utils.h"

std::map<std::string, std::string> CodeWriter::_components {};
std::map<std::string, std::string> CodeWriter::_filters {};

CodeWriter::CodeWriter(const Page& page, const std::string& clazz):
	_page(page), _class(clazz),
	_removeEmptyString("(\\s?responseStream << \"\";|\\s?responseStream << \"\\\\n\";)", std::regex_constants::ECMAScript|std::regex_constants::icase)
{
}

CodeWriter::~CodeWriter() {
}


void
CodeWriter::writeHeader(std::ostream& ostr, const std::string& headerFileName) {
	beginGuard(ostr, headerFileName);
	writeHeaderIncludes(ostr);
	ostr << "\n\n";

	ostr << "#undef CPSP_CLASS_NAME\n";
	ostr << "#define CPSP_CLASS_NAME " << _class << "\n\n";

	std::string decls(_page.headerDecls().str());
	if (!decls.empty()) {
		ostr << decls << "\n\n";
	}

	beginNamespace(ostr);
	writeHandlerClass(ostr);
	endNamespace(ostr);
	endGuard(ostr, headerFileName);
}


void
CodeWriter::writeImpl(std::ostream& ostr, const std::string& headerFileName) {
	ostr << "#include \"" << headerFileName << "\"\n";
	writeImplIncludes(ostr);
	ostr << "\n";

	std::string decls(_page.implDecls().str());
	if (!decls.empty()) {
		ostr << decls << "\n\n";
	}

	std::string functions(_page.functions().str());
	if (!functions.empty()) {
		ostr << functions << "\n\n";
	}

	beginNamespace(ostr);

	std::string path = _page.getAttribute<std::string>("page.path", "");
	if (!path.empty()) {
		ostr << "\tconst std::string " << _class << "::PATH(\"" << path << "\");\n\n\n";
	}

	writeConstructor(ostr);
	writeHandler(ostr);
	endNamespace(ostr);
}

void
CodeWriter::beginNamespace(std::ostream& ostr) {
	std::string ns = _page.getAttribute<std::string>("page.namespace", "");
	if (!ns.empty()) {
		std::vector<std::string> v;
		split(ns, ':', v);
		for (auto& n : v) {
			ostr << "namespace " << n << " {\n";
		}
		ostr << "\n\n";
	}
}

void
CodeWriter::endNamespace(std::ostream& ostr) {
	std::string ns = _page.getAttribute<std::string>("page.namespace", "");
	if (!ns.empty()) {
		ostr << "\n\n";
		std::vector<std::string> v;
		split(ns, ':', v);
		for (auto& n : v) {
			ostr << "} ";
		}
		ostr << "// namespace " << ns << "\n";
	}
}


void
CodeWriter::beginGuard(std::ostream& ostr, const std::string& headerFileName) {
	std::string guard = replaceAll(basename(headerFileName), ".", "_"); // -
	guard += "_INCLUDED";

	ostr << "#ifndef " << guard << "\n";
	ostr << "#define " << guard << "\n";
	ostr << "\n\n";
}


void
CodeWriter::endGuard(std::ostream& ostr, const std::string& headerFileName) {
	std::string guard = replaceAll(basename(headerFileName), ".", "_"); // -
	guard += "_INCLUDED";
	ostr << "\n\n";
	ostr << "#endif // " << guard << "\n";
}


void
CodeWriter::handlerClass(std::ostream& ostr, const std::string& base, const std::string& ctorArg) {
	std::string exprt(_page.getAttribute<std::string>("page.export", ""));
	if (!exprt.empty()) {
		exprt += ' ';
	}

	ostr << "class " << exprt << _class << ": public " << base << " {\n";
	ostr << "public:\n";
	if (!ctorArg.empty()) {
		ostr << "\t" << _class << "(" << ctorArg << ");\n";
		ostr << "\n";
	}
	ostr << "\tvoid handleRequest(fastcgi::HttpRequest* request, fastcgi::HttpResponse* response) override;\n";

	writeHandlerMembers(ostr);

	std::string path = _page.getAttribute<std::string>("page.path", "");
	if (!path.empty()) {
		ostr << "\n\tstatic const std::string PATH;\n";
	}

	ostr << "};\n";
}

void
CodeWriter::writeHeaderIncludes(std::ostream& ostr) {
	ostr << "#include \"fastcgi3/component.h\"\n";
	ostr << "#include \"fastcgi3/http_request.h\"\n";
	ostr << "#include \"fastcgi3/http_response.h\"\n";
	ostr << "#include \"fastcgi3/http_servlet.h\"\n";
}


void
CodeWriter::writeHandlerClass(std::ostream& ostr) {
	std::string base(_page.getAttribute<std::string>("page.baseClass", "fastcgi::Servlet"));
	std::string ctorArg(_page.getAttribute<std::string>("page.ctorArg", "std::shared_ptr<fastcgi::ComponentContext> context"));
	handlerClass(ostr, base, ctorArg);
}


void
CodeWriter::writeHandlerMembers(std::ostream& ostr) {
	std::string decls(_page.methods().str());
	if (!decls.empty()) {
		ostr << "\n";
		ostr << "private:\n";
		ostr << decls << "\n\n";
	}
}

void
CodeWriter::writeImplIncludes(std::ostream& ostr) {
	ostr << "#include <iostream>\n";
	ostr << "#include <stdexcept>\n";
	ostr << "#include \"fastcgi3/logger.h\"\n";
	ostr << "#include \"fastcgi3/config.h\"\n";
	ostr << "#include \"fastcgi3/except.h\"\n";
	ostr << "#include \"fastcgi3/stream.h\"\n";
	ostr << "#include \"fastcgi3/handler.h\"\n";
	ostr << "#include \"fastcgi3/request.h\"\n";
	ostr << "#include \"fastcgi3/component.h\"\n";;
	ostr << "#include \"fastcgi3/component_factory.h\"\n";
	ostr << "#include \"fastcgi3/http_request.h\"\n";
	ostr << "#include \"fastcgi3/http_response.h\"\n";
	ostr << "#include \"fastcgi3/http_servlet.h\"\n";
}

void
CodeWriter::writeConstructor(std::ostream& ostr) {
	std::string base(_page.getAttribute<std::string>("page.baseClass", "fastcgi::Servlet"));
	std::string ctorArg(_page.getAttribute<std::string>("page.ctorArg", "std::shared_ptr<fastcgi::ComponentContext> context"));

	ostr << _class << "::" << _class << "(" << ctorArg << "):\n";
	ostr << "\t " << base << "(context) {\n";
	ostr << "}\n";
	ostr << "\n\n";
}

void
CodeWriter::writeHandler(std::ostream& ostr) {
	ostr << "void\n" << _class << "::handleRequest(fastcgi::HttpRequest* request, fastcgi::HttpResponse* response) {\n";
	writeResponse(ostr);
	if (_page.hasAttribute("page.precondition")) {
		ostr << "\tif (!(" << _page.getAttribute<std::string>("page.precondition") << ")) return;\n\n";
	}
	ostr << _page.preHandler().str();
	writeContent(ostr);
	ostr << "}\n";
}

void
CodeWriter::writeFactory(std::ostream& istr, std::ostream& ostr, const std::string& headerFileName) {
	std::string componentName(_page.getAttribute<std::string>("component.name", _class));
	if (!componentName.empty()) {
		istr << "#include \"" << headerFileName << "\"\n";

		beginNamespace(ostr);
		ostr << "FCGIDAEMON_ADD_DEFAULT_FACTORY(\"" << componentName << "\", "<< _class << ")\n";
		endNamespace(ostr);
	}
}

void
CodeWriter::writeComponentsConfig(std::ostream& ostr, const std::string& moduleName, const std::string& loggerName) {
	std::string componentName(_page.getAttribute<std::string>("component.name", _class));
	if (!componentName.empty()) {
		std::string key = moduleName+":"+componentName;

		if (_components.find(key)==_components.end()) {
			ostr << "\t<component name=\"" << componentName << "\" type=\"" << moduleName << ":" << componentName << "\">\n";
			if (!loggerName.empty()) {
				ostr << "\t\t<logger>" << loggerName << "</logger>\n";
			}
			ostr << "\t</component>\n";
			_components[key] = _class;
		} else {
			throw std::string("duplicate component name ")+key+std::string(" for class ")+_class+std::string("; previously defined for class ")+_components[key];
		}
	}
}

void
CodeWriter::writeHandlersConfig(std::ostream& ostr, const std::string& threadPoolName) {
	std::string componentName(_page.getAttribute<std::string>("component.name", _class));
	if (!componentName.empty()) {
		std::stringstream sfilter;

		std::string url(_page.getAttribute<std::string>("component.url", "/"+componentName));
		if (!url.empty()) {
			sfilter << "url=\"" << url << "\" ";
		}

		if (_page.hasAttribute("component.host")) {
			std::string value(_page.getAttribute<std::string>("component.host"));
			if (!value.empty()) {
				sfilter << "host=\"" << value << "\" ";
			}
		}

		if (_page.hasAttribute("component.port")) {
			std::string value(_page.getAttribute<std::string>("component.port", "/"+componentName));
			if (!value.empty()) {
				sfilter << "port=\"" << value << "\" ";
			}
		}

		if (_page.hasAttribute("component.address")) {
			std::string value(_page.getAttribute<std::string>("component.address", "/"+componentName));
			if (!value.empty()) {
				sfilter << "address=\"" << value << "\" ";
			}
		}

		if (_page.hasAttribute("component.referer")) {
			std::string value(_page.getAttribute<std::string>("component.referer", "/"+componentName));
			if (!value.empty()) {
				sfilter << "referer=\"" << value << "\" ";
			}
		}

		std::string filter = sfilter.str();
		if (!filter.empty()) {
			if (_filters.find(filter)==_filters.end()) {
				ostr << "\t<handler " << filter << " pool=\"" << threadPoolName << "\">\n";
				ostr << "\t\t<component name=\"" << componentName << "\"/>\n";
				ostr << "\t</handler>\n";
				_filters[filter] = componentName;
			} else {
				throw std::string("duplicate filter ")+filter+std::string(" for component ")+componentName+std::string("; previously defined for component ")+_filters[filter];
			}
		}
	}
}

void
CodeWriter::writeResponse(std::ostream& ostr) {
	std::string contentType(_page.getAttribute<std::string>("page.contentType", "text/html"));
	std::string contentLang(_page.getAttribute<std::string>("page.contentLanguage", ""));

	ostr << "\tresponse->setContentType(\"" << contentType << "\");\n";
	if (!contentLang.empty()) {
		ostr << "\tif (request->hasHeader(\"Accept-Language\")) {\n"
			 << "\t\tresponse->setHeader(\"Content-Language\", \"" << contentLang << "\");\n"
			 << "\t}";
	}

	ostr << "\n";
}

void
CodeWriter::writeContent(std::ostream& ostr) {
	ostr << "\tfastcgi::HttpResponseStream responseStream(response);\n";
	ostr << std::regex_replace(_page.handler().str(), _removeEmptyString, "");
}





