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

#ifndef PAGE_COMPILER_CODE_WRITER_H_
#define PAGE_COMPILER_CODE_WRITER_H_

#include <ostream>
#include <regex>

class Page;

/// This class implements the code generator for
/// generating C++ header and implementation files
/// from C++ Server Pages.
class CodeWriter {
public:
	CodeWriter(const Page& page, const std::string& clazz);

	virtual ~CodeWriter();

	/// Writes the header file contents to the given stream.
	virtual void writeHeader(std::ostream& ostr, const std::string& headerFileName);

	/// Writes the implementation file contents to the given stream.
	virtual void writeImpl(std::ostream& ostr, const std::string& headerFileName);

	virtual void writeFactory(std::ostream& istr, std::ostream& ostr, const std::string& headerFileName);

	virtual void writeComponentsConfig(std::ostream& ostr, const std::string& moduleName, const std::string& loggerName);

	virtual void writeHandlersConfig(std::ostream& ostr, const std::string& threadPoolName);

	/// Returns a const reference to the Page.
	const Page& page() const;

	/// Returns the name of the handler class.
	const std::string& clazz() const;

protected:
	virtual void writeHeaderIncludes(std::ostream& ostr);
	virtual void writeHandlerClass(std::ostream& ostr);
	virtual void writeHandlerMembers(std::ostream& ostr);
	virtual void writeImplIncludes(std::ostream& ostr);
	virtual void writeConstructor(std::ostream& ostr);
	virtual void writeHandler(std::ostream& ostr);
	virtual void writeResponse(std::ostream& ostr);
	virtual void writeContent(std::ostream& ostr);

	void beginGuard(std::ostream& ostr, const std::string& headerFileName);
	void endGuard(std::ostream& ostr, const std::string& headerFileName);
	void beginNamespace(std::ostream& ostr);
	void endNamespace(std::ostream& ostr);
	void handlerClass(std::ostream& ostr, const std::string& base, const std::string& ctorArg);

private:
	CodeWriter();
	CodeWriter(const CodeWriter&);
	CodeWriter& operator = (const CodeWriter&);

	const Page& _page;
	std::string _class;

	std::regex _removeEmptyString;

	static std::map<std::string, std::string> _components;
	static std::map<std::string, std::string> _filters;
};


inline const
Page& CodeWriter::page() const {
	return _page;
}


inline const std::string&
CodeWriter::clazz() const {
	return _class;
}


#endif /* PAGE_COMPILER_CODE_WRITER_H_ */
