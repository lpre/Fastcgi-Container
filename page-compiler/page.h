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

#ifndef PAGE_COMPILER_PAGE_H_
#define PAGE_COMPILER_PAGE_H_

#include <string>
#include <sstream>

#include "fastcgi3/attributes_holder.h"

/// This class represents a server page consisting of
/// handler code and declarations, as well as page attributes.
class Page: public fastcgi::AttributesHolder
{
public:
	Page();

	~Page();

	/// Returns the user-specified declarations for the header file.
	std::stringstream& headerDecls();

	/// Returns the user-specified declarations for the header file.
	const std::stringstream& headerDecls() const;

	std::stringstream& methods();

	const std::stringstream& methods() const;

	std::stringstream& functions();

	const std::stringstream& functions() const;


	/// Returns the user-specified declarations for the source file.
	std::stringstream& implDecls();

	/// Returns the user-specified declarations for the source file.
	const std::stringstream& implDecls() const;

	/// Returns the request handler code.
	std::stringstream& handler();

	/// Returns the request prehandler code.
	const std::stringstream& handler() const;

	/// Returns the request handler code.
	std::stringstream& preHandler();

	/// Returns the request prehandler code.
	const std::stringstream& preHandler() const;

	/// Returns the boolean value of the given property.
	///
	/// The return value will be true if the property
	/// has one of the following values:
	///    - true
	///    - yes
	///    - on
	///
	/// Otherwise, the return value will be false.
	bool getBool(const std::string& property, bool deflt = false) const;

	/// Returns the integer value of the given property.
	int getInt(const std::string& property, int deflt = 0) const;

private:
	Page(const Page&);
	Page& operator = (const Page&);

	std::stringstream _headerDecls;
	std::stringstream _implDecls;
	std::stringstream _handler;
	std::stringstream _preHandler;

	std::stringstream _methods;
	std::stringstream _functions;

};


//
// inlines
//
inline std::stringstream& Page::methods() {
	return _methods;
}

inline std::stringstream& Page::functions() {
	return _functions;
}

inline std::stringstream& Page::headerDecls() {
	return _headerDecls;
}

inline const std::stringstream& Page::headerDecls() const {
	return _headerDecls;
}

inline const std::stringstream& Page::methods() const {
	return _methods;
}

inline const std::stringstream& Page::functions() const {
	return _functions;
}

inline std::stringstream& Page::implDecls() {
	return _implDecls;
}

inline const std::stringstream& Page::implDecls() const {
	return _implDecls;
}

inline std::stringstream& Page::handler() {
	return _handler;
}

inline const std::stringstream& Page::handler() const {
	return _handler;
}

inline std::stringstream& Page::preHandler() {
	return _preHandler;
}

inline const std::stringstream& Page::preHandler() const {
	return _preHandler;
}

#endif /* PAGE_COMPILER_PAGE_H_ */
