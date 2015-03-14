pkglib_LTLIBRARIES = fastcgi3-statistics.la

fastcgi3_statistics_la_SOURCES = response_time_handler.cpp
fastcgi3_statistics_la_LIBADD = ../library/libfastcgi3-container.la
fastcgi3_statistics_la_LDFLAGS = -module -lpthread

AM_CPPFLAGS = -I../include -I../config
AM_CXXFLAGS = -pthread

noinst_HEADERS = response_time_handler.h