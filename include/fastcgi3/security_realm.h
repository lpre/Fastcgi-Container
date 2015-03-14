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

#ifndef fastcgi3_SECURITY_REALM_H_
#define fastcgi3_SECURITY_REALM_H_

#include <string>

#include "fastcgi3/component.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/security_subject.h"

namespace fastcgi
{

namespace security
{

class Realm : public fastcgi::Component {
public:
	Realm(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~Realm();

    virtual void onLoad() override;
    virtual void onUnload() override;

	virtual std::shared_ptr<Subject> authenticate(const std::string& username, const std::string& credentials);

	virtual std::shared_ptr<Subject> getSubject(const std::string& username);

	const std::string& getName() const;


protected:
	std::string name_;
	std::shared_ptr<fastcgi::Logger> logger_;
};

} // namespace security

} // namespace fastcgi



#endif /* fastcgi3_SECURITY_REALM_H_ */
