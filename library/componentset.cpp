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


// #include "settings.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "fastcgi3/config.h"
#include "fastcgi3/component.h"
#include "fastcgi3/component_factory.h"

#include "details/loader.h"
#include "details/componentset.h"
#include "details/component_context.h"
#include "details/globals.h"

#include "core/memory.hpp"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

ComponentSet::ComponentContainer::ComponentContainer()
: component(nullptr), context(nullptr), isLoadingStarted(false) {
}

ComponentSet::ComponentSet()
: globals_(nullptr) {
}

ComponentSet::~ComponentSet() {
    sendOnUnloadToComponents();
    components_.clear();
}

void
ComponentSet::init(const Globals *globals) {
    globals_ = globals;
    std::vector<std::string> v;
    std::string key("/fastcgi/components/component");
    globals->config()->subKeys(key, v);
    for (auto& i : v) {
        const std::string name = globals->config()->asString(i + "/@name");
        const std::string type = globals->config()->asString(i + "/@type");
        add(name, type, i);
    }
    sendOnLoadToComponents();
}

std::shared_ptr<Component>
ComponentSet::find(const std::string &name) const {
    ComponentSet *nonConstThis = const_cast<ComponentSet*>(this);
    auto i = nonConstThis->components_.find(name);
    if (i != components_.end()) {
        ComponentContainer &cont = i->second;
        if (!isComponentLoaded(name)) {
            nonConstThis->sendOnLoad(name, cont);
        }
        return cont.component;
    }
    return std::shared_ptr<Component>();
}

void
ComponentSet::add(const std::string &name, const std::string &type, const std::string &componentXPath) {
    ComponentFactory *factory = globals_->loader()->findComponentFactory(type);
    if (nullptr == factory) {
        throw std::runtime_error("Cannot find component factory for type: " + type);
    }
    ComponentContainer c;
    c.context = std::make_shared<ComponentContextImpl>(globals_, componentXPath);
    try {
        c.component = factory->createComponent(c.context);
    }
    catch (const std::exception &e) {
        throw std::runtime_error(e.what());
    }
    catch (...) {
        throw std::runtime_error("Unknown exception caught");
    }
    components_.insert(make_pair(name, c));
}

void
ComponentSet::sendOnLoadToComponents() {
    for (auto& i : components_) {
        sendOnLoad(i.first, i.second);
    }
}

void
ComponentSet::sendOnLoad(const std::string &componentName, ComponentContainer &cont) {
    if (!isComponentLoaded(componentName)) {
        if (cont.isLoadingStarted) {
            throw std::runtime_error("Cyclic component dependence found");
        }
        cont.isLoadingStarted = true;
        try {
            cont.component->onLoad();
        }
        catch (const std::exception &e) {
            throw std::runtime_error(e.what());
        }
        catch (...) {
            throw std::runtime_error("Unknown exception caught");
        }
        loadingStack_.push_back(componentName);
    }
}

void
ComponentSet::sendOnUnloadToComponents() {
    for (auto& i : loadingStack_) {
        ComponentContainer cont = components_.find(i)->second;
        try {
            cont.component->onUnload();
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "Unknown exception caught" << std::endl;
        }
    }
}

bool
ComponentSet::isComponentLoaded(const std::string &componentName) const {
    return std::find(loadingStack_.begin(), loadingStack_.end(), componentName) != loadingStack_.end();
}

} // namespace fastcgi
