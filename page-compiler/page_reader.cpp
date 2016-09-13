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

#include <cctype>
#include <fstream>

#include "page_reader.h"
#include "page.h"
#include "utils.h"

const std::string PageReader::MARKUP_BEGIN("\tresponseStream << \"");
const std::string PageReader::MARKUP_END("\";\n");
const std::string PageReader::EXPR_BEGIN("\tresponseStream << (");
const std::string PageReader::EXPR_END(");\n");


PageReader::PageReader(Page& page, const std::string& path):
	_page(page),
	_pParent(0),
	_path(path),
	_line(0)
{
	_attrs.reserve(4096);
}


PageReader::PageReader(const PageReader& parent, const std::string& path):
	_page(parent._page),
	_pParent(&parent),
	_path(path),
	_line(0)
{
	_attrs.reserve(4096);
}


PageReader::~PageReader() {
}

void
PageReader::parse(std::istream& pageStream) {
	ParsingState state = ParsingState::STATE_MARKUP;

	_page.handler() << MARKUP_BEGIN;

	std::string token;
	nextToken(pageStream, token);

	while (!token.empty()) {
		if (token == "<%") {
			if (state == ParsingState::STATE_MARKUP) {
				_page.handler() << MARKUP_END;
				state = ParsingState::STATE_BLOCK;
			} else {
				_page.handler() << token;
			}
		} else if (token == "<%%") {
			if (state == ParsingState::STATE_MARKUP) {
				_page.handler() << MARKUP_END;
				state = ParsingState::STATE_PREHANDLER;
			} else {
				_page.handler() << token;
			}
		} else if (token == "<%!") {
			if (state == ParsingState::STATE_MARKUP) {
				_page.handler() << MARKUP_END;
				state = ParsingState::STATE_FUNCTIONS;
			} else {
				_page.handler() << token;
			}
		} else if (token == "<%!!") {
			if (state == ParsingState::STATE_MARKUP) {
				_page.handler() << MARKUP_END;
				state = ParsingState::STATE_METHODS;
			} else {
				_page.handler() << token;
			}
		} else if (token == "<%--") {
			if (state == ParsingState::STATE_MARKUP) {
				_page.handler() << MARKUP_END;
				state = ParsingState::STATE_COMMENT;
			} else {
				_page.handler() << token;
			}
		} else if (token == "<%@") {
			if (state == ParsingState::STATE_MARKUP) {
				_page.handler() << MARKUP_END;
				state = ParsingState::STATE_ATTR;
				_attrs.clear();
			} else {
				_page.handler() << token;
			}
		} else if (token == "<%=") {
			if (state == ParsingState::STATE_MARKUP) {
				_page.handler() << MARKUP_END;
				_page.handler() << EXPR_BEGIN;
				state = ParsingState::STATE_EXPR;
			} else {
				_page.handler() << token;
			}
		} else if (token == "%>") {
			if (state == ParsingState::STATE_EXPR) {
				_page.handler() << EXPR_END;
				_page.handler() << MARKUP_BEGIN;
				state = ParsingState::STATE_MARKUP;
			} else if (state == ParsingState::STATE_ATTR) {
				parseAttributes();
				_attrs.clear();
				_page.handler() << MARKUP_BEGIN;
				state = ParsingState::STATE_MARKUP;
			} else if (state != ParsingState::STATE_MARKUP) {
				_page.handler() << MARKUP_BEGIN;
				state = ParsingState::STATE_MARKUP;
			} else {
				_page.handler() << token;
			}
		} else {
			switch (state) {
			case ParsingState::STATE_MARKUP:
				if (token == "\n") {
					_page.handler() << "\\n";
					_page.handler() << MARKUP_END;
					_page.handler() << MARKUP_BEGIN;
				} else if (token == "\t") {
					_page.handler() << "\\t";
				} else if (token == "\"") {
					_page.handler() << "\\\"";
				} else if (token != "\r") {
					_page.handler() << token;
				}
				break;
			case ParsingState::STATE_IMPLDECL:
				_page.implDecls() << token;
				break;
			case ParsingState::STATE_FUNCTIONS:
				_page.functions() << token;
				break;
			case ParsingState::STATE_HDRDECL:
				_page.headerDecls() << token;
				break;
			case ParsingState::STATE_METHODS:
				_page.methods() << token;
				break;
			case ParsingState::STATE_PREHANDLER:
				_page.preHandler() << token;
				break;
			case ParsingState::STATE_BLOCK:
				_page.handler() << token;
				break;
			case ParsingState::STATE_EXPR:
				_page.handler() << token;
				break;
			case ParsingState::STATE_COMMENT:
				break;
			case ParsingState::STATE_ATTR:
				_attrs += token;
				break;
			}
		}
		nextToken(pageStream, token);

	}

	if (state == ParsingState::STATE_MARKUP) {
		_page.handler() << MARKUP_END;
	} else {
		throw std::string("unclosed meta or code block: ")+where();
	}
}


void
PageReader::parseAttributes() {
	static const int eof = std::char_traits<char>::eof();

	std::string basename;
	std::istringstream istr(_attrs);
	int ch = istr.get();

	while (ch != eof && std::isspace(ch)) {
		ch = istr.get();
	}
	while (ch != eof && std::isalnum(ch)) {
		basename += (char) ch; ch = istr.get();
	}
	while (ch != eof && std::isspace(ch)) {
		ch = istr.get();
	}
	while (ch != eof) {
		std::string name(basename + ".");
		std::string value;
		while (ch != eof && std::isalnum(ch)) {
			name += (char) ch; ch = istr.get();
		}
		while (ch != eof && std::isspace(ch)) {
			ch = istr.get();
		}
		if (ch != '=') {
			throw std::string("bad attribute syntax: '=' expected ")+where();
		}
		ch = istr.get();
		while (ch != eof && std::isspace(ch)) {
			ch = istr.get();
		}
		if (ch == '"') {
			ch = istr.get();
			while (ch != eof && ch != '"') {
				value += (char) ch; ch = istr.get();
			}
			if (ch != '"') {
				throw std::string("bad attribute syntax: '\"' expected ")+where();
			}
		} else if (ch == '\'') {
			ch = istr.get();
			while (ch != eof && ch != '\'') {
				value += (char) ch; ch = istr.get();
			}
			if (ch != '\'') {
				throw std::string("bad attribute syntax: ''' expected ")+where();
			}
		} else {
			throw std::string("bad attribute syntax: '\"' or ''' expected")+where();
		}
		ch = istr.get();
		handleAttribute(name, value);
		while (ch != eof && std::isspace(ch)) {
			ch = istr.get();
		}
	}
}


void
PageReader::nextToken(std::istream& istr, std::string& token) {
	token.clear();
	int ch = istr.get();
	if (ch != -1) {
		if (ch == '<' && istr.peek() == '%') {
			token += "<%";
			ch = istr.get();
			ch = istr.peek();
			switch (ch) {
			case '%':
			case '@':
			case '=':
				ch = istr.get();
				token += (char) ch;
				break;
			case '!':
				ch = istr.get();
				token += (char) ch;
				if (istr.peek() == '!') {
					ch = istr.get();
					token += (char) ch;
				}
				break;
			case '-':
				ch = istr.get();
				token += (char) ch;
				if (istr.peek() == '-') {
					ch = istr.get();
					token += (char) ch;
				}
				break;
			}
		} else if (ch == '%' && istr.peek() == '>') {
			token += "%>";
			ch = istr.get();
		} else {
			token += (char) ch;
		}
	}
}


void
PageReader::handleAttribute(const std::string& name, const std::string& value) {
	if (name == "include.page") {
		include(value);
	} else 	if (name == "include.path") {
		_page.handler() << "\tresponse->includePath(\"" << value << "\");";
	} else 	if (name == "include.component") {
		_page.handler() << "\tresponse->includeComponent(\"" << value << "\");";
	} else if (name == "header.include") {
		_page.headerDecls() << "#include \"" << value << "\"\n";
	} else if (name == "header.sinclude") {
		_page.headerDecls() << "#include <" << value << ">\n";
	} else if (name == "impl.include") {
		_page.implDecls() << "#include \"" << value << "\"\n";
	} else if (name == "impl.sinclude") {
		_page.implDecls() << "#include <" << value << ">\n";
	} else {
		if (!_page.hasAttribute(name)) {
			_page.setAttribute<std::string>(name, value);
		}

//		std::string _class;
//		std::string componentName(_page.getAttribute<std::string>(name, _class));
//		if (componentName.empty()) {
//			_page.setAttribute<std::string>(name, value);
//		}
	}
}


void
PageReader::include(const std::string& path) {
	std::string includePath = pathToFile(_path) + "/"+ path;

	_page.handler() << "\t// begin include " << includePath << "\n";

	std::ifstream includeStream(includePath);

	if (includeStream.is_open()) {
		PageReader includeReader(*this, includePath);
		includeReader.parse(includeStream);
		includeStream.close();
	} else {
//		static char buf[1024];
//		sprintf(buf, "Error: could not open file %s for inclusion from %s!\n", includePath.substr(0, 400).c_str(), _path.substr(0, 400).c_str());
//		throw std::runtime_error(buf);

		throw std::string("could not open file  ")+includePath+std::string(" for inclusion ")+where();
	}

	_page.handler() << "\t// end include " << includePath << "\n";
}


std::string
PageReader::where() const {
	std::stringstream result;
	result << "in file '" << _path << "', line " << _line;
	const PageReader* pParent = _pParent;
	while (pParent) {
		result << "\n\tincluded from file '"<<  pParent->_path << "', line " << pParent->_line;
		pParent = pParent->_pParent;
	}
	return result.str();
}



