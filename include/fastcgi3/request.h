// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
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

#ifndef _FASTCGI_REQUEST_H_
#define _FASTCGI_REQUEST_H_

#include <set>
#include <map>
#include <iosfwd>
#include <functional>
#include <sstream>
#include <chrono>

#include "fastcgi3/util.h"
#include "fastcgi3/cookie.h"
#include "fastcgi3/session.h"
#include "fastcgi3/session_manager.h"
#include "fastcgi3/security_subject.h"
#include "fastcgi3/data_buffer.h"
#include "fastcgi3/config.h"
#include "fastcgi3/range.h"
#include "fastcgi3/functors.h"

namespace fastcgi
{

class File
{
public:
	File(DataBuffer filename, DataBuffer type, DataBuffer content);
	File(const std::string &filename, const std::string &type, DataBuffer content);

	const std::string& type() const;
	const std::string& remoteName() const;

	DataBuffer data() const;

private:
	std::string name_, type_;
	DataBuffer data_;
};

class Logger;
class Request;
class RequestCache;
class RequestIOStream;
class RequestsThreadPool;

using VarMap = std::map<std::string, std::string>;
using HeaderMap = std::map<std::string, std::string, StringCILess>;

class Request {
public:
	Request(std::shared_ptr<Logger> logger, std::shared_ptr<RequestCache> cache, std::shared_ptr<SessionManager> sessionManager);
	~Request();

	Request(const Request&) = delete;
	Request& operator=(const Request&) = delete;

	const std::string& getEnvVariable(const std::string &name) const;

	const std::string& getRole() const;

	unsigned short getServerPort() const;
	const std::string& getHost() const;
	const std::string& getServerAddr() const;

	const std::string& getPathInfo() const;
	const std::string& getPathTranslated() const;

	const std::string& getScriptName() const;
	const std::string& getScriptFilename() const;

	const std::string& getDocumentRoot() const;

	const std::string& getRemoteUser() const;
	const std::string& getRemotePassword() const;
	const std::string& getRemoteAddr() const;
	const std::string& getQueryString() const;
	const std::string& getRequestMethod() const;
	const std::string& getRequestId() const;

	std::streamsize getContentLength() const;
	const std::string& getContentType() const;

    std::string getURI() const;
    std::string getUrl() const;

	unsigned int countArgs() const;
	bool hasArg(const std::string &name) const;
	const std::string& getArg(const std::string &name) const;
	void getArg(const std::string &name, std::vector<std::string> &v) const;
	void argNames(std::vector<std::string> &v) const;

	unsigned int countHeaders() const;
	bool hasHeader(const std::string &name) const;
	const std::string& getHeader(const std::string &name) const;
	void headerNames(std::vector<std::string> &v) const;

	unsigned int countCookie() const;
	bool hasCookie(const std::string &name) const;
	const std::string& getCookie(const std::string &name) const;
	void deleteCookie(const std::string &name);
	void cookieNames(std::vector<std::string> &v) const;

	unsigned int countRemoteFiles() const;
	bool hasRemoteFile(const std::string &name) const;
	void remoteFiles(std::vector<std::string> &v) const;
	bool hasFile(const std::string &name) const; // deprecated

	const std::string& remoteFileName(const std::string &name) const;
	const std::string& remoteFileType(const std::string &name) const;
	DataBuffer remoteFile(const std::string &name) const;

	bool isSecure() const;
	DataBuffer requestBody() const;

	bool isBot() const;

	void setCookie(const Cookie &cookie);

	bool headersSent() const;
	void setStatus(unsigned short status);
	void sendError(unsigned short status);
	void setHeader(const std::string &name, const std::string &value);

	std::shared_ptr<Session> createSession();
	std::shared_ptr<Session> getSession();
	void changeSessionId();

	void setSubject(std::shared_ptr<security::Subject> subject);
	std::shared_ptr<security::Subject> getSubject() const;

	bool isUserInRole(const std::string& roleName) const;

	template<class T> bool
	isUserInRole(const std::string &roleName) {
		return getSubject()->hasPrincipal<T>(roleName);
	}

	std::stringstream* getResponseStream();

	void write(std::streambuf *buf);
	std::streamsize write(const char *buf, std::streamsize size);
	std::string outputHeader(const std::string &name) const;

	bool isProcessed() const;
	void markAsProcessed();
	void tryAgain(std::chrono::milliseconds delay);

	void parse(DataBuffer buffer);
	void serialize(DataBuffer &buffer);
	void saveToCache(Request *request);

	void restore(DataBuffer buffer);

    void redirectBack();
    void redirectToPath(const std::string &path);
    void forwardToPath(const std::string &path);

    void setContentType(const std::string &type);
    void setContentEncoding(const std::string &encoding);

	void reset();
	void sendHeaders();
	void attach(RequestIOStream *stream, char *env[]);

	unsigned short status() const;

private:
	friend class Parser;
	friend RequestsThreadPool;
	void sendHeadersInternal();
	bool disablePostParams() const;

	std::uint64_t serializeEnv(DataBuffer &buffer, std::uint64_t add_size);
	std::uint64_t serializeInt(DataBuffer &buffer, std::uint64_t pos, std::uint64_t val);
	std::uint64_t serializeString(DataBuffer &buffer, std::uint64_t pos, const std::string &val);
	std::uint64_t serializeBuffer(DataBuffer &buffer, std::uint64_t pos, const DataBuffer &src);
	std::uint64_t serializeFiles(DataBuffer &buffer, std::uint64_t pos);
	std::uint64_t serializeArgs(DataBuffer &buffer, std::uint64_t pos);

	std::uint64_t parseInt(DataBuffer buffer, std::uint64_t pos, std::uint64_t &val);
	std::uint64_t parseString(DataBuffer buffer, std::uint64_t pos, std::string &val);
	std::uint64_t parseHeaders(DataBuffer buffer, std::uint64_t pos);
	std::uint64_t parseCookies(DataBuffer buffer, std::uint64_t pos);
	std::uint64_t parseVars(DataBuffer buffer, std::uint64_t pos);
	std::uint64_t parseBody(DataBuffer buffer, std::uint64_t pos);
	std::uint64_t parseFiles(DataBuffer buffer, std::uint64_t pos);
	std::uint64_t parseArgs(DataBuffer buffer, std::uint64_t pos);

	std::uint64_t bodySerializedSize();
	std::uint64_t filesSerializedSize();
	std::uint64_t argsSerializedSize();

private:
	bool headers_sent_;
	unsigned short status_;
	bool processed_;
	std::chrono::milliseconds delay_;

	RequestIOStream* stream_;
	VarMap vars_, cookies_;
	DataBuffer body_;
	HeaderMap headers_, out_headers_;

	std::set<Cookie> out_cookies_;
	std::map<std::string, File> files_;
	std::vector<StringUtils::NamedValue> args_;

	std::shared_ptr<Logger> logger_;
	std::shared_ptr<RequestCache> cache_;

	/// Current session
	std::shared_ptr<Session> session_;

	std::shared_ptr<SessionManager> session_manager_;

	std::shared_ptr<security::Subject> subject_;

	std::stringstream response_stream_;
};

} // namespace fastcgi

#endif // _FASTCGI_REQUEST_H_

