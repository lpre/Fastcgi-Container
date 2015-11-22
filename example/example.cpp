#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "fastcgi3/logger.h"
#include "fastcgi3/config.h"
#include "fastcgi3/stream.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/request.h"
#include "fastcgi3/component.h"
#include "fastcgi3/component_factory.h"

#include "fastcgi3/http_request.h"
#include "fastcgi3/http_response.h"
#include "fastcgi3/http_servlet.h"

#include "fastcgi3/security_realm.h"


#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace example
{

class ExampleHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
public:
	ExampleHandler(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~ExampleHandler();

	virtual void onLoad();
	virtual void onUnload();

	virtual void handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext);

private:
	std::shared_ptr<fastcgi::Logger> logger_;
};

class ExampleHandler2 : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
public:
    ExampleHandler2(std::shared_ptr<fastcgi::ComponentContext> context);
    virtual ~ExampleHandler2();

    virtual void onLoad();
    virtual void onUnload();

    virtual void handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext);
};

class ExampleServlet : virtual public fastcgi::Servlet
{
public:
	ExampleServlet(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~ExampleServlet();

	void doGet(std::shared_ptr<fastcgi::HttpRequest> httpReq, std::shared_ptr<fastcgi::HttpResponse> httpResp);
};


class ExampleFilter : virtual public fastcgi::Filter, virtual public fastcgi::Component
{
public:
	ExampleFilter(std::shared_ptr<fastcgi::ComponentContext> context);
    virtual ~ExampleFilter();

    virtual void onLoad();
    virtual void onUnload();

	void doFilter(fastcgi::Request *req, fastcgi::HandlerContext *context, std::function<void(fastcgi::Request *req, fastcgi::HandlerContext *context)> next) override;
};


struct UserData {
	std::string password;
	std::vector<std::string> roles;
};

class ExampleRealm : virtual public fastcgi::security::Realm {
public:
	ExampleRealm(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~ExampleRealm();

    virtual void onLoad() override;
    virtual void onUnload() override;

	virtual std::shared_ptr<fastcgi::security::Subject> authenticate(const std::string& username, const std::string& credentials)  override;

	virtual std::shared_ptr<fastcgi::security::Subject> getSubject(const std::string& username) override;

private:
    std::unordered_map<std::string, std::shared_ptr<UserData>> users_;
};



ExampleHandler::ExampleHandler(std::shared_ptr<fastcgi::ComponentContext> context) : fastcgi::Component(context), logger_(nullptr) {
}

ExampleHandler::~ExampleHandler() {
}

void
ExampleHandler::onLoad() {	
	std::cout << "onLoad handler1 executed" << std::endl;
	const std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
	logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
	if (!logger_) {
		throw std::runtime_error("cannot get component " + loggerComponentName);
	}
}

void 
ExampleHandler::onUnload() {
	std::cout << "onUnload handler1 executed" << std::endl;
}

void
ExampleHandler::handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext) {

printf("1\n");

	core::any param = handlerContext->getParam("testParam");
	if (param.empty()) {
		std::cout << "testParam not found" << std::endl;
	} else {
		try {
			std::cout << "testParam = " << core::any_cast<std::string>(param) << std::endl;
		} catch (const core::bad_any_cast &) {
			std::cout << "bad_any_cast: testParam is not string" << std::endl;
		}
	}

printf("2\n");

//	req->setContentType("text/plain");
//	req->setContentType("text/html");
	fastcgi::RequestStream stream(req);

printf("3\n");

	std::vector<std::string> names;
	req->argNames(names);
	for (auto& i : names) {
		stream << "arg " << i << " has value " << req->getArg(i) << "\n";
	}
	req->headerNames(names);
	for (auto& i : names) {
		stream << "header " << i << " has value " << req->getHeader(i) << "\n";
	}
	req->cookieNames(names);
	for (auto& i : names) {
		stream << "cookie " << i << " has value " << req->getCookie(i) << "\n";
	}

printf("4\n");

	stream << "Remote User: " << req->getRemoteUser() << "\n";

printf("5\n");

	stream << "ExampleHandler: test ok 222\n";

printf("6\n");

	if (req->getSession()) {
printf("7\n");
		stream << "session_id=" << req->getSession()->getId() << "\n";

printf("8\n");

		if (req->hasArg("a1")) {
			req->getSession()->setAttribute<std::string>("a1", req->getArg("a1"));
		}
printf("9\n");

		if (req->getSession()->hasAttribute("a1")) {
			stream << "session attribute a1=" << req->getSession()->getAttribute<std::string>("a1");
		}
	} else {
		stream << "session is not supported in current configuration\n";
	}

printf("7\n");

//	logger_->info("request processed");

	std::string html_source =
R"html(
<table border="1">
	<tr><td>1</td><td>2</td></tr>
	<tr><td>a</td><td>b</td></tr>
</table>
)html";

	stream << html_source;

printf("6\n");

	req->setStatus(200);

printf("7\n");

	handlerContext->setParam("param1", std::string("hi!"));
}

ExampleHandler2::ExampleHandler2(std::shared_ptr<fastcgi::ComponentContext> context) : fastcgi::Component(context) {
}

ExampleHandler2::~ExampleHandler2() {
}

void
ExampleHandler2::onLoad() {  
	std::cout << "onLoad handler2 executed" << std::endl;
}

void
ExampleHandler2::onUnload() {
	std::cout << "onUnload handler2 executed" << std::endl;
}

void
ExampleHandler2::handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext) {
	core::any param1 = handlerContext->getParam("param1");
	core::any param2 = handlerContext->getParam("param2");

	fastcgi::RequestStream stream(req);
	stream << "ExampleHandler2: OK\n";


//	if (param1.empty()) {
//		std::cout << "param1 not found" << std::endl;
//	} else {
//		try {
//			std::cout << "value of param1 = " << core::any_cast<std::string>(param1) << std::endl;
//		} catch (const core::bad_any_cast &) {
//			std::cout << "bad_any_cast: param1 is not string" << std::endl;
//		}
//	}
//
//	if (param2.empty()) {
//		std::cout << "param2 not found" << std::endl;
//	} else {
//		std::cout << "param1 found" << std::endl;
//	}

	if (req->getScriptName() == "/upload" && req->getRequestMethod() == "POST") {
		fastcgi::DataBuffer f = req->remoteFile("file");
		std::string file;
		f.toString(file);
		std::cout << "file=\n" << file << std::endl;
	}
}



ExampleServlet::ExampleServlet(std::shared_ptr<fastcgi::ComponentContext> context) :
	fastcgi::Servlet(context) {
	;
}

ExampleServlet::~ExampleServlet() {
	;
}


void
ExampleServlet::doGet(std::shared_ptr<fastcgi::HttpRequest> httpReq, std::shared_ptr<fastcgi::HttpResponse> httpResp) {

printf("Handle example servlet\n");

	fastcgi::HttpResponseStream stream(httpResp);

	stream << "\ntest ok 33 (servlet)\n";
}



ExampleFilter::ExampleFilter(std::shared_ptr<fastcgi::ComponentContext> context) : fastcgi::Component(context) {

}

ExampleFilter::~ExampleFilter() {
	;
}

void ExampleFilter::onLoad() {
	;
}

void ExampleFilter::onUnload() {
	;
}

void ExampleFilter::doFilter(fastcgi::Request *req, fastcgi::HandlerContext *context, std::function<void(fastcgi::Request *req, fastcgi::HandlerContext *context)> next) {

	fastcgi::RequestStream stream(req);

//	stream << "\nFilter : 1\n";

	printf("Filter: 1 %s\n", req->getHeader("USER-AGENT").c_str());


	next(req, context);

try {
	req->setHeader("X-TestHeader", "TestValue");
} catch (const std::exception &e) {
	printf("Exception %s\n", e.what());
} catch (...) {
	printf("Exception\n");
}

	printf("Filter: 2\n");

//	stream << "\nFilter : 2\n";
}


ExampleRealm::ExampleRealm(std::shared_ptr<fastcgi::ComponentContext> context)
: fastcgi::security::Realm(context) {

    const fastcgi::Config *config = context->getConfig();
	const std::string componentXPath = context->getComponentXPath();

    std::vector<std::string> users;
    config->subKeys(componentXPath+"/users/user[count(@name)=1]", users);
    for (auto& u : users) {
        std::string username = config->asString(u + "/@name", "");

        std::shared_ptr<UserData> data = std::make_shared<UserData>();
        data->password = config->asString(u + "/@password", "");

        std::vector<std::string> roles;
        config->subKeys(u+"/role[count(@name)=1]", roles);
        for (auto& r : roles) {
        	data->roles.push_back(config->asString(r + "/@name", ""));
        }

        users_.insert({username, std::move(data)});
    }

}

ExampleRealm::~ExampleRealm() {
	;
}

void
ExampleRealm::onLoad() {
	fastcgi::security::Realm::onLoad();
}

void
ExampleRealm::onUnload() {
	fastcgi::security::Realm::onUnload();
}

std::shared_ptr<fastcgi::security::Subject> ExampleRealm::authenticate(const std::string& username, const std::string& credentials) {
	std::shared_ptr<fastcgi::security::Subject> subject;

printf("Try user >%s< >%s<\n", username.c_str(), credentials.c_str());

	auto it = users_.find(username);
	if (users_.end()!=it && it->second && credentials==it->second->password) {
		subject = std::make_shared<fastcgi::security::Subject>();

		for (auto &r : it->second->roles) {
			subject->setPrincipal(std::make_shared<fastcgi::security::Principal>(r));
		}

		subject->setReadOnly();

		// Logging
		std::vector<std::shared_ptr<fastcgi::security::Principal>> principals;
		subject->getPrincipals(principals);
		std::stringstream s;
		for (auto &p : principals) {
			s << p->getName() << " ";
		}

printf("User %s is logged in as %s\n", username.c_str(), s.str().c_str());
		logger_->info("User %s is logged in as %s\n", username.c_str(), s.str().c_str());
	}

	return subject;
}

std::shared_ptr<fastcgi::security::Subject> ExampleRealm::getSubject(const std::string& username)  {
	std::shared_ptr<fastcgi::security::Subject> subject;

	auto it = users_.find(username);
	if (users_.end()!=it && it->second) {
		subject = std::make_shared<fastcgi::security::Subject>();

		for (auto &r : it->second->roles) {
			subject->setPrincipal(std::make_shared<fastcgi::security::Principal>(r));
		}

		subject->setReadOnly();
	}

	return subject;
}


FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("example", ExampleHandler)
FCGIDAEMON_ADD_DEFAULT_FACTORY("example2", ExampleHandler2)
FCGIDAEMON_ADD_DEFAULT_FACTORY("example3", ExampleServlet)
FCGIDAEMON_ADD_DEFAULT_FACTORY("filter1", ExampleFilter)
FCGIDAEMON_ADD_DEFAULT_FACTORY("example-realm", ExampleRealm)
FCGIDAEMON_REGISTER_FACTORIES_END()

} // namespace example
