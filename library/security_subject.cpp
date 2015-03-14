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

#include <functional>
#include <algorithm>

#include "fastcgi3/security_principal.h"
#include "fastcgi3/security_subject.h"

namespace fastcgi
{

namespace security
{

std::string anonymous("Anonymous");
std::mutex Subject::mutex_;
std::shared_ptr<Subject> Subject::anonymousSubject_;

Subject::Subject()
: readOnly_(false) {
}

Subject::~Subject() {
	for (auto& p : principals_) {
		p.reset();
	}
	principals_.clear();
}

std::shared_ptr<Subject>
Subject::getAnonymousSubject() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (!anonymousSubject_) {
		anonymousSubject_ = std::make_shared<Subject>();
		anonymousSubject_->setPrincipal(std::make_shared<Principal>(anonymous));
		anonymousSubject_->setReadOnly();
	}
	return anonymousSubject_;
}

void Subject::setPrincipal(std::shared_ptr<Principal> principal) {
	if (!readOnly_) {
		if (!hasPrincipal(principal->getHashCode())) {
			principals_.push_back(std::move(principal));
		} else {
			throw std::runtime_error("Duplicate principal");
		}
	} else {
		throw std::runtime_error("Attempt to modify read-only subject");
	}
}

void
Subject::setPrincipals(std::vector<std::shared_ptr<Principal>>& principals) {
	if (!readOnly_) {
		for (auto& p : principals) {
			if (!hasPrincipal(p->getHashCode())) {
				principals_.push_back(p);
			} else {
				throw std::runtime_error("Duplicate principal");
			}
		}
	} else {
		throw std::runtime_error("Attempt to modify read-only subject");
	}
}

void
Subject::getPrincipals(std::vector<std::shared_ptr<Principal>>& principals) {
	for (auto& p : principals_) {
		principals.push_back(p);
	}
}

bool
Subject::hasPrincipal(std::size_t hashCode) const {
	for (auto& p : principals_) {
		if (hashCode==p->getHashCode()) {
			return true;
		}
	}
	return false;
}

bool
Subject::hasPrincipal(const std::string& name) const {
	std::size_t hashCode = std::hash<std::string>()(name);
	return hasPrincipal(hashCode);
}

bool
Subject::isAnonymous() const {
	for (auto& p : principals_) {
		if (p->getName()==anonymous) {
			return true;
		}
	}
	return false;
}

bool
Subject::isReadOnly() const noexcept {
	return readOnly_;
}

void
Subject::setReadOnly() noexcept {
	readOnly_ = true;
}

}

}

