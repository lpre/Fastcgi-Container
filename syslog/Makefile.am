pkglib_LTLIBRARIES = fastcgi3-syslog.la

fastcgi3_syslog_la_SOURCES = syslog-logger.cpp
fastcgi3_syslog_la_LIBADD = ../library/libfastcgi3-container.la
fastcgi3_syslog_la_LDFLAGS = -module -lpthread

AM_CPPFLAGS = -I../include -I../config
AM_CXXFLAGS = -pthread

noinst_HEADERS = syslog-logger.h