// Fastcgi Container - framework for development of high performance FastCGI applications in C++
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

#ifndef fastcgi3_SECURITY_SUBJECT_H_
#define fastcgi3_SECURITY_SUBJECT_H_

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "fastcgi3/security_principal.h"

namespace fastcgi
{

/**
 * A Subject represents a grouping of related information
 * for a single entity, such as a person.
 * Such information includes the Subject's identities as well as
 * its security-related attributes
 * (passwords and cryptographic keys, for example).
 */
namespace security
{

class Subject {
public:
	Subject();
	~Subject();

	Subject(const Subject&) = delete;
	Subject& operator=(const Subject&) = delete;

	static std::shared_ptr<Subject> getAnonymousSubject();

	void setPrincipal(std::shared_ptr<Principal> principal);

	void setPrincipals(std::vector<std::shared_ptr<Principal>>& principals);

	void getPrincipals(std::vector<std::shared_ptr<Principal>>& principals);

	template<class T>
	void getPrincipals(std::vector<std::shared_ptr<T>>& principals) {
		for (auto& p : principals_) {
			std::shared_ptr<T> pt = std::dynamic_pointer_cast<T>(p);
			if (pt) {
				principals.push_back(std::move(pt));
			}
		}
	}

	bool hasPrincipal(std::size_t hashCode) const;

	template<class T>
	bool hasPrincipal(std::size_t hashCode) {
		for (auto& p : principals_) {
			std::shared_ptr<T> pt = std::dynamic_pointer_cast<T>(p);
			if (pt && hashCode==pt->getHashCode()) {
				return true;
			}
		}
		return false;
	}

	bool hasPrincipal(const std::string& name) const;

	template<class T>
	bool hasPrincipal(const std::string& name) {
		std::size_t hashCode = std::hash<std::string>()(name);
		return hasPrincipal<T>(hashCode);
	}

	bool isAnonymous() const;

	bool isReadOnly() const noexcept;

	void setReadOnly() noexcept;

private:
	static std::mutex mutex_;
	static std::shared_ptr<Subject> anonymousSubject_;

	std::vector<std::shared_ptr<Principal>> principals_;

	bool readOnly_;
};

} // namespace security

} // namespace fastcgi



#endif /* fastcgi3_SECURITY_SUBJECT_H_ */
