// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
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

#include "settings.h"

#include "details/handlerset.h"
#include "details/componentset.h"
#include "details/request_filter.h"

#include "fastcgi3/util.h"
#include "fastcgi3/config.h"
#include "fastcgi3/component.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/request.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

HandlerSet::HandlerSet() {
}

HandlerSet::~HandlerSet() {
}


void
HandlerSet::initInternal(const Config *config, const ComponentSet *componentSet, const std::string &url_prefix, std::vector<std::string> &v) {

    for (auto& k : v) {
        HandlerDescription handlerDesc;
        handlerDesc.poolName = config->asString(k + "/@pool", defaultPoolName_);
        handlerDesc.id = config->asString(k + "/@id", "");

        std::string url_filter = config->asString(k + "/@url", "");
        if (!url_filter.empty()) {
              handlerDesc.selectors.push_back(std::make_pair("url", std::make_shared<UrlFilter>(url_filter, url_prefix)));
        }

        std::string host_filter = config->asString(k + "/@host", "");
        if (!host_filter.empty()) {
              handlerDesc.selectors.push_back(std::make_pair("host", std::make_shared<HostFilter>(host_filter)));
        }

        std::string port_filter = config->asString(k + "/@port", "");
        if (!port_filter.empty()) {
              handlerDesc.selectors.push_back(std::make_pair("port", std::make_shared<PortFilter>(port_filter)));
        }

        std::string address_filter = config->asString(k + "/@address", "");
        if (!address_filter.empty()) {
              handlerDesc.selectors.push_back(std::make_pair("address", std::make_shared<AddressFilter>(address_filter)));
        }

        std::string referer_filter = config->asString(k + "/@referer", "");
        if (!referer_filter.empty()) {
              handlerDesc.selectors.push_back(std::make_pair("referer", std::make_shared<RefererFilter>(referer_filter)));
        }

        std::vector<std::string> q;
        config->subKeys(k + "/param", q);
        for (auto& it : q) {
            std::string name = config->asString(it + "/@name", "");
            if (name.empty()) {
                continue;
            }
            std::string value = config->asString(it, "");
            if (value.empty()) {
                continue;
            }
            handlerDesc.selectors.push_back(std::make_pair("param", std::make_shared<ParamFilter>(name, value)));
        }

        std::vector<std::string> components;
        config->subKeys(k + "/component", components);
        for (auto& c : components) {
            const std::string componentName = config->asString(c + "/@name");
            std::shared_ptr<Component> handlerComponent = componentSet->find(componentName);
            if (!handlerComponent) {
                throw std::runtime_error("Cannot find component: " + componentName);
            }

            std::shared_ptr<Handler> handler = std::dynamic_pointer_cast<Handler>(handlerComponent);
            if (!handler) {
                throw std::runtime_error("Component " + componentName + " does not implement interface Handler");
            }

            handlerDesc.handlers.push_back(handler);
        }
        handlers_.push_back(handlerDesc);
    }

}


void
HandlerSet::init(const Config *config, const ComponentSet *componentSet) {
    const std::string url_prefix = config->asString("/fastcgi/handlers/@urlPrefix", StringUtils::EMPTY_STRING);
    defaultPoolName_ = config->asString("/fastcgi/pools/@default", StringUtils::EMPTY_STRING);

    // Selectors with available attribute "url"
    std::vector<std::string> v;
    config->subKeys("/fastcgi/handlers/handler[count(@url)=1]", v);
    initInternal(config, componentSet, url_prefix, v);

    // Selectors without attribute "url"
	v.clear();
    config->subKeys("/fastcgi/handlers/handler[not(@url)]", v);
    initInternal(config, componentSet, url_prefix, v);

    // Filters
	v.clear();
    config->subKeys("/fastcgi/handlers/filter", v);
    for (auto& k : v) {
        FilterDescription filterDesc;
        filterDesc.id = config->asString(k + "/@id", "");

        std::string url_filter = config->asString(k + "/@url", "");
        if (!url_filter.empty()) {
        	filterDesc.selectors.push_back(std::make_pair("url", std::make_shared<UrlFilter>(url_filter, url_prefix)));
        }

        std::string host_filter = config->asString(k + "/@host", "");
        if (!host_filter.empty()) {
        	filterDesc.selectors.push_back(std::make_pair("host", std::make_shared<HostFilter>(host_filter)));
        }

        std::string port_filter = config->asString(k + "/@port", "");
        if (!port_filter.empty()) {
        	filterDesc.selectors.push_back(std::make_pair("port", std::make_shared<PortFilter>(port_filter)));
        }

        std::string address_filter = config->asString(k + "/@address", "");
        if (!address_filter.empty()) {
        	filterDesc.selectors.push_back(std::make_pair("address", std::make_shared<AddressFilter>(address_filter)));
        }

        std::string referer_filter = config->asString(k + "/@referer", "");
        if (!referer_filter.empty()) {
        	filterDesc.selectors.push_back(std::make_pair("referer", std::make_shared<RefererFilter>(referer_filter)));
        }

        std::vector<std::string> q;
        config->subKeys(k + "/param", q);
        for (auto& it : q) {
            std::string name = config->asString(it + "/@name", "");
            if (name.empty()) {
                continue;
            }
            std::string value = config->asString(it, "");
            if (value.empty()) {
                continue;
            }
            filterDesc.selectors.push_back(std::make_pair(
                "param", std::shared_ptr<RequestFilter>(new ParamFilter(name, value))));
        }

        std::vector<std::string> components;
        config->subKeys(k + "/component", components);
        for (auto& c : components) {
            const std::string componentName = config->asString(c + "/@name");
            std::shared_ptr<Component> handlerComponent = componentSet->find(componentName);
            if (!handlerComponent) {
                throw std::runtime_error("Cannot find component: " + componentName);
            }

            std::shared_ptr<Filter> handler = std::dynamic_pointer_cast<Filter>(handlerComponent);
            if (!handler) {
                throw std::runtime_error("Component " + componentName + " does not implement interface Filter");
            }

            filterDesc.handlers.push_back(handler);
        }
        filters_.push_back(filterDesc);
    }

}

const HandlerSet::HandlerDescription*
HandlerSet::findURIHandler(const Request *request) const {

	// Find the single matching handler

    for (auto& i : handlers_) {
        bool matched = true;
        for (auto& f : i.selectors) {
            if (f.first == "url" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "host" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "address" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "port" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "referer" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "param" && !f.second->check(request)) {
                matched = false;
                break;
            }
        }

        if (matched) {
            return &i;
        }
    }
    return nullptr;
}

void
HandlerSet::findURIFilters(const Request *request, std::vector<std::shared_ptr<Filter>> &v) const {

	// Find all matching filters

    for (auto& i : filters_) {
        bool matched = true;
        for (auto& f : i.selectors) {
            if (f.first == "url" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "host" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "address" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "port" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "referer" && !f.second->check(request)) {
                matched = false;
                break;
            }
            else if (f.first == "param" && !f.second->check(request)) {
                matched = false;
                break;
            }
        }

        if (matched) {
        	for (unsigned int n=0; n<i.handlers.size(); ++n) {
        		v.push_back(i.handlers[n]);
        	}
        }
    }

}

void
HandlerSet::findPoolHandlers(const std::string &poolName, std::set<std::shared_ptr<Handler>> &handlers) const {
    handlers.clear();
    for (auto& it : handlers_) {
        if (it.poolName == poolName) {
            handlers.insert(it.handlers.begin(), it.handlers.end());
        }
    }
}

std::set<std::string>
HandlerSet::getPoolsNeeded() const {
    std::set<std::string> pools;
    for (auto& i : handlers_) {
        pools.insert(i.poolName);
    }
    if (pools.find(defaultPoolName_)==pools.end()) {
        pools.insert(defaultPoolName_);
    }
    return pools;
}

const std::string&
HandlerSet::getDefaultPool() const {
	return defaultPoolName_;
}


} // namespace fastcgi

