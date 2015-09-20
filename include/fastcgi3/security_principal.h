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

#ifndef fastcgi3_SECURITY_PRINCIPAL_H_
#define fastcgi3_SECURITY_PRINCIPAL_H_

#include <string>

namespace fastcgi
{

namespace security
{

/**
 * This abstract class represents the abstract notion of a principal, which
 * can be used to represent any entity, such as an individual, a
 * corporation, and a login id.
 */
class Principal {
public:
	Principal(const std::string &name);
	virtual ~Principal();

	Principal(const Principal&) = delete;
	Principal& operator=(const Principal&) = delete;

	bool operator==(const Principal& p);
	bool operator!=(const Principal& p);

	bool operator==(std::size_t hashCode);
	bool operator!=(std::size_t hashCode);

	bool operator==(const std::string& name);
	bool operator!=(const std::string& name);

	std::size_t getHashCode() const;

	const std::string& getName() const;

private:
	std::string name_;
};

} // namespace security

} // namespace fastcgi



#endif /* fastcgi3_SECURITY_PRINCIPAL_H_ */
