pkglib_LTLIBRARIES = fastcgi3-authenticator.la

fastcgi3_authenticator_la_SOURCES = factory.cpp \
	abstract_authenticator.cpp basic_authenticator.cpp form_authenticator.cpp delegated_authenticator.cpp
fastcgi3_authenticator_la_LIBADD = ../library/libfastcgi3-container.la
fastcgi3_authenticator_la_LDFLAGS = -module -lpthread

AM_CPPFLAGS = -I../include -I../config
AM_CXXFLAGS = -pthread

noinst_HEADERS = abstract_authenticator.h basic_authenticator.h form_authenticator.h delegated_authenticator.h