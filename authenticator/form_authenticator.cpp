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

#include "abstract_authenticator.h"
#include "form_authenticator.h"

#include <functional>
#include <iostream>
#include <algorithm>
#include <regex>

#include "fastcgi3/component_factory.h"
#include "fastcgi3/config.h"
#include "fastcgi3/logger.h"
#include "fastcgi3/util.h"
#include "fastcgi3/data_buffer.h"
#include "fastcgi3/stream.h"
#include "fastcgi3/except.h"

#include "details/component_context.h"
#include "details/file_buffer.h"

namespace fastcgi
{

namespace security
{

const std::string FormAuthenticator::COMPONENT_NAME {"form-authenticator"};
const std::string FormAuthenticator::SECURITY_CHECK_URL {"/j_security_check"};
const std::string FormAuthenticator::FORM_ACTION {"/login"};
const std::string FormAuthenticator::FORM_USERNAME {"j_username"};
const std::string FormAuthenticator::FORM_PASSWORD {"j_password"};
const std::string FormAuthenticator::PARAM_NAME_STORED_REQUEST {"auth-stored-request"};
const std::string FormAuthenticator::PARAM_NAME_STORED_REQUEST_URI {"auth-stored-request-uri"};
const std::string FormAuthenticator::DEFAULT_CACHE_DIR {"/tmp/fastcgi3-container/cache/form-auth-cache/"};

FormAuthenticator::FormAuthenticator(std::shared_ptr<fastcgi::ComponentContext> context)
: fastcgi::Component(context), AbstractAuthenticator(context) {
    const fastcgi::Config *config = context->getConfig();
	const std::string componentXPath = context->getComponentXPath();

	urlPrefix_ = config->asString("/fastcgi/handlers/@urlPrefix", StringUtils::EMPTY_STRING);
	securityCheckUrl_ = config->asString(componentXPath+"/security-check-url", FormAuthenticator::SECURITY_CHECK_URL);
	formAction_ = config->asString(componentXPath+"/form-page", FormAuthenticator::FORM_ACTION);

	if (*securityCheckUrl_.begin() != '/') {
		securityCheckUrl_.insert(0, "/");
	}
	if (*formAction_.begin() != '/') {
		formAction_.insert(0, "/");
	}

	cache_dir_ = config->asString(componentXPath + "/cache-dir", StringUtils::EMPTY_STRING);
	if (cache_dir_.empty()) {
		cache_dir_ = FormAuthenticator::DEFAULT_CACHE_DIR;
		std::cout << "FormAuthenticator: cache directory is not configured; using default directory \"" << FormAuthenticator::DEFAULT_CACHE_DIR << "\"" << std::endl;
	}
	if (*cache_dir_.rbegin() != '/') {
		cache_dir_.push_back('/');
	}
	try {
		FileSystemUtils::createDirectories(cache_dir_);
		if (!FileSystemUtils::isWritable(cache_dir_)) {
			throw std::runtime_error("Permission denied");
		}
	} catch (const std::exception &e) {
		std::cerr << "FormAuthenticator: could not create cache directory \"" << cache_dir_ << "\": " << e.what() << std::endl;
		throw;
	}

	window_ = config->asInt(componentXPath + "/file-window", 1024*1024);

	storeRequest_ = "true"==config->asString(componentXPath + "/store-request", "false");

	securityCheckUrlRegex_ = std::regex("(action\\s*=\\s*[\"']?).*"+securityCheckUrl_.substr(1)+"\\b", std::regex_constants::ECMAScript|std::regex_constants::icase);
}

FormAuthenticator::~FormAuthenticator() {
	printf("FormAuthenticator deleted\n");
}

bool
FormAuthenticator::isAuthenticationRequired(fastcgi::Request* request) const {
	if (!formAction_.empty() && fastcgi::StringUtils::endsWith(request->getScriptName(), formAction_)) {
		// Do not request authentication for login form
		return false;
	}
	if (request->hasArg(FORM_USERNAME) && request->hasArg(FORM_PASSWORD)) {
		// Remove the existing authentication information
		setSubject(request, Subject::getAnonymousSubject());

		// Always request authentication for data submitted from login form
		// Otherwise the method "authenticate" won't be invoked
		return true;
	}
	return AbstractAuthenticator::isAuthenticationRequired(request);
}

void
FormAuthenticator::doFilter(fastcgi::Request *request, fastcgi::HandlerContext *context, std::function<void(fastcgi::Request *request, fastcgi::HandlerContext *context)> next) {

	Authenticator::doFilter(request, context, next);

	// Is this the login page?
	if (fastcgi::StringUtils::endsWith(request->getScriptName(), formAction_)) {
		std::shared_ptr<fastcgi::Session> session = request->getSession();
		if (session && session->hasAttribute(PARAM_NAME_STORED_REQUEST) && session->hasAttribute(PARAM_NAME_STORED_REQUEST_URI)) {
			// Replace the form action URL with original URL (if available)

			fastcgi::RequestStream stream(request);
			std::string loginForm = stream.asString();
			stream.reset();

			std::string url = session->getAttribute<std::string>(PARAM_NAME_STORED_REQUEST_URI);
			stream << std::regex_replace(loginForm, securityCheckUrlRegex_, "$1"+url);
		}
	}

}

bool
FormAuthenticator::authenticate(fastcgi::Request* request) const {
	// Is this the action request from the login page?
//	if (fastcgi::StringUtils::endsWith(request->getScriptName(), securityCheckUrl_)) {
	if (request->hasArg(FORM_USERNAME) && request->hasArg(FORM_PASSWORD)) {
		// Yes: validate the specified credentials and redirect to the error page if they are not correct

		 const std::string &username = request->getArg(FORM_USERNAME);
		 const std::string &password = request->getArg(FORM_PASSWORD);

		 if (doLogin(request, username, password)) {
			 // Success
			 // Redirect the user to the original request URI (which will cause
			 // the original request to be restored)

			 std::shared_ptr<fastcgi::Session> session = request->getSession();
			 if (session) {
				 if (session->hasAttribute(PARAM_NAME_STORED_REQUEST)) {
					 fastcgi::DataBuffer buffer = session->getAttribute<fastcgi::DataBuffer>(PARAM_NAME_STORED_REQUEST);
					 session->removeAttribute(PARAM_NAME_STORED_REQUEST);
					 throw fastcgi::DispatchException(fastcgi::DispatchException::DispatchType::FORWARD, buffer);

				 } else if (session->hasAttribute(PARAM_NAME_STORED_REQUEST_URI)) {
					 std::string url = session->getAttribute<std::string>(PARAM_NAME_STORED_REQUEST_URI);
					 session->removeAttribute(PARAM_NAME_STORED_REQUEST_URI);
					 request->redirectToPath(url);
					 request->markAsProcessed();
					 // Return "false" to stop the processing chain
					 return false;
				 }
			 }

		 } else {
			 // Invalid credentials
			 std::shared_ptr<fastcgi::Session> session = request->getSession();
			 if (session && (session->hasAttribute(PARAM_NAME_STORED_REQUEST) || session->hasAttribute(PARAM_NAME_STORED_REQUEST_URI))) {
				 // If original request has already been stored,
				 // redirect to login screen again
				 request->markAsProcessed();
				 request->redirectToPath(urlPrefix_+formAction_);
				 return false;
			 }
		 }

	} else {
		// No: save this request and redirect to the form login page

		std::shared_ptr<fastcgi::Session> session = request->getSession();
		if (session) {

			if (!storeRequest_ && "POST"!=request->getRequestMethod() && 0==request->countRemoteFiles()) {
				// Store the original URL
				session->setAttribute<std::string>(PARAM_NAME_STORED_REQUEST_URI, request->getURI());
			} else {
				// Store the original URL and request
				fastcgi::DataBuffer buffer = createFileBuffer(session->getId());
				if (!buffer.isNil()) {
					request->serialize(buffer);
					session->setAttribute<fastcgi::DataBuffer>(PARAM_NAME_STORED_REQUEST, buffer);
					session->setAttribute<std::string>(PARAM_NAME_STORED_REQUEST_URI, request->getURI());
				}
			}

			// Redirect to login screen
			request->markAsProcessed();
			request->redirectToPath(urlPrefix_+formAction_);
			return false;
		}
	}

	return false;
}

fastcgi::DataBuffer
FormAuthenticator::createFileBuffer(const std::string &key) const {
	const std::string path = cache_dir_ + key;
	fastcgi::FileSystemUtils::createDirectories(cache_dir_);
	return fastcgi::DataBuffer::create(new fastcgi::FileBuffer(path.c_str(), window_));
}

std::string
FormAuthenticator::getSecurityCheckUrl(fastcgi::Request *request) const {
	std::shared_ptr<fastcgi::Session> session = request->getSession();
	if (session && session->hasAttribute(PARAM_NAME_STORED_REQUEST) && session->hasAttribute(PARAM_NAME_STORED_REQUEST_URI)) {
		return session->getAttribute<std::string>(PARAM_NAME_STORED_REQUEST_URI);
	} else {
		return securityCheckUrl_;
	}
}

const std::string&
FormAuthenticator::getFormUsername() const {
	return FORM_USERNAME;
}

const std::string&
FormAuthenticator::getFormPassword() const {
	return FORM_PASSWORD;
}


}

}

