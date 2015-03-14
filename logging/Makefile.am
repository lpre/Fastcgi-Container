pkglib_LTLIBRARIES = fastcgi3-logger.la

fastcgi3_logger_la_SOURCES = logger.cpp
fastcgi3_logger_la_LIBADD = ../library/libfastcgi3-container.la
fastcgi3_logger_la_LDFLAGS = -module -lpthread -llog4cxx

AM_CPPFLAGS = -I../include -I../config
AM_CXXFLAGS = -pthread -std=c++0x

noinst_HEADERS = logger.h
