sbin_PROGRAMS = fastcgi3-container

fastcgi3_container_SOURCES = main.cpp fcgi_server.cpp endpoint.cpp fcgi_request.cpp
fastcgi3_container_LDADD = ../library/libfastcgi3-container.la -lfcgi -lfcgi++

AM_CPPFLAGS = -I@top_srcdir@/include -I@top_srcdir@/config
AM_LDFLAGS = 

noinst_HEADERS = fcgi_server.h endpoint.h fcgi_request.h
dist_sysconf_DATA = fastcgi.conf.example
