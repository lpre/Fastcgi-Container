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

#ifndef PAGE_COMPILER_PAGE_READER_H_
#define PAGE_COMPILER_PAGE_READER_H_


#include <string>
#include <istream>
#include <ostream>
#include <sstream>


class Page;

class PageReader {
public:
	PageReader(Page& page, const std::string& path);

	/// Creates the PageReader, using the given PageReader as parent.
	PageReader(const PageReader& parent, const std::string& path);

	~PageReader();

	/// Parses a HTML file containing server page directives,
	/// converts the file into C++ code and adds the code
	/// to the reader's Page object. Also parses page
	/// attributes and include directives.
	void parse(std::istream& pageStream);

protected:
	enum class ParsingState
	{
		STATE_MARKUP,
		STATE_IMPLDECL,
		STATE_HDRDECL,
		STATE_METHODS,
		STATE_FUNCTIONS,
		STATE_PREHANDLER,
		STATE_BLOCK,
		STATE_EXPR,
		STATE_COMMENT,
		STATE_ATTR
	};

	static const std::string MARKUP_BEGIN;
	static const std::string MARKUP_END;
	static const std::string EXPR_BEGIN;
	static const std::string EXPR_END;

	void include(const std::string& path);
	void parseAttributes();
	void nextToken(std::istream& istr, std::string& token);
	void handleAttribute(const std::string& name, const std::string& value);
	std::string where() const;

private:
	PageReader();
	PageReader(const PageReader&);
	PageReader& operator = (const PageReader&);

	Page& _page;
	const PageReader* _pParent;
	std::string _path;
	std::string _attrs;
	int _line;
};




#endif /* PAGE_COMPILER_PAGE_READER_H_ */
