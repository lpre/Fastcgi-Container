pkglib_LTLIBRARIES = fastcgi3-request-cache.la

fastcgi3_request_cache_la_SOURCES = file_cache.cpp  
fastcgi3_request_cache_la_LIBADD = ../library/libfastcgi3-container.la
fastcgi3_request_cache_la_LDFLAGS = -module -lpthread

AM_CPPFLAGS = -I../include -I../config
AM_CXXFLAGS = -pthread

noinst_HEADERS = file_cache.h