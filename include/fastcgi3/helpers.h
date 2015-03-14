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

#ifndef _FASTCGI_HELPERS_H_
#define _FASTCGI_HELPERS_H_

#include <cassert>
#include <algorithm>

namespace fastcgi
{

template<typename Type, typename Clean>
class Helper
{
public:
	Helper() noexcept;
	explicit Helper(Type tptr) noexcept;
	
	Helper(const Helper<Type, Clean> &h) noexcept;
	Helper<Type, Clean>& operator = (const Helper<Type, Clean> &h) noexcept;
	
	~Helper() noexcept;

	Type get() const noexcept;
	Type operator -> () const noexcept;
	
	Type release() noexcept;
	void reset(Type tptr) noexcept;

private:
	Type releaseInternal() const noexcept;

private:
	mutable Type tptr_;
};

template<typename Type, typename Clean>
Helper<Type, Clean>::Helper() noexcept
: tptr_(nullptr) {
}

template<typename Type, typename Clean>
Helper<Type, Clean>::Helper(Type tptr) noexcept
: tptr_(tptr) {
}

template<typename Type, typename Clean>
Helper<Type, Clean>::Helper(const Helper<Type, Clean> &h) noexcept
: tptr_(nullptr) {
	std::swap(tptr_, h.tptr_);
	assert(nullptr == h.get());
}

template<typename Type, typename Clean> Helper<Type, Clean>&
Helper<Type, Clean>::operator = (const Helper<Type, Clean> &h) noexcept {
	if (&h != this) {
		reset(h.releaseInternal());
		assert(nullptr == h.get());
	}
	return *this;
}

template<typename Type, typename Clean> inline
Helper<Type, Clean>::~Helper() noexcept {
	reset(nullptr);
}

template<typename Type, typename Clean> Type
Helper<Type, Clean>::get() const noexcept {
	return tptr_;
}

template<typename Type, typename Clean> Type
Helper<Type, Clean>::operator -> () const noexcept {
	assert(nullptr != tptr_);
	return tptr_;
}

template<typename Type, typename Clean> Type
Helper<Type, Clean>::release() noexcept {
	return releaseInternal();
}

template<typename Type, typename Clean> void
Helper<Type, Clean>::reset(Type tptr) noexcept {
	if (nullptr != tptr_) {
		Clean::clean(tptr_);
	}
	tptr_ = tptr;
}

template<typename Type, typename Clean> Type
Helper<Type, Clean>::releaseInternal() const noexcept {
	Type ptr = nullptr;
	std::swap(ptr, tptr_);
	return ptr;
}

} // namespace fastcgi

#endif // _FASTCGI_HELPERS_H_
