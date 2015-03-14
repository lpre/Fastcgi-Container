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

#ifndef INCLUDE_FASTCGI3_SESSION_H_
#define INCLUDE_FASTCGI3_SESSION_H_

#include <string>
#include <mutex>
#include <chrono>

#include "fastcgi3/attributes_holder.h"
#include "fastcgi3/security_subject.h"

namespace fastcgi
{

class SessionManager;

namespace security
{
class Authenticator;
}

class Session : virtual public AttributesHolder {
private:
	friend class SessionManager;
	friend class security::Authenticator;
	Session(const std::string &id);

protected:
	virtual void init();
	void changeId(std::string& newId);
	void setSubject(std::shared_ptr<security::Subject>);

public:
	virtual ~Session();
	const std::string& getId() const;
	void invalidate();

	Session() = delete;
	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;

	template<class T>
	void setMaxInactiveInterval(unsigned int d) {
		max_inactive_interval_ = std::chrono::duration_cast<std::chrono::minutes>(T(d));
	}

	template<class T>
	std::chrono::duration<T> getMaxInactiveInterval() const {
		return std::chrono::duration_cast<T>(max_inactive_interval_);
	}

	const std::chrono::steady_clock::time_point getCreationTime() const;
	const std::chrono::steady_clock::time_point getLastAccessedTime() const;
	void updateLastAccessedTime();
	bool isExpired() const;

	std::shared_ptr<security::Subject> getSubject() const;

protected:
	std::string id_;

    std::chrono::steady_clock::time_point created_;
    std::chrono::steady_clock::time_point last_accessed_;
    std::chrono::minutes max_inactive_interval_;

    /// The Authenticator may cache a previously authenticated Subject,
    /// and avoid potentially expensive Realm.authenticate() calls on every request.
	std::shared_ptr<security::Subject> subject_;
};


} // namespace fastcgi


#endif /* INCLUDE_FASTCGI3_SESSION_H_ */
