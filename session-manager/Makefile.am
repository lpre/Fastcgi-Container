pkglib_LTLIBRARIES = fastcgi3-session-manager.la

fastcgi3_session_manager_la_SOURCES = simple_session_manager.cpp
fastcgi3_session_manager_la_LIBADD = ../library/libfastcgi3-container.la
fastcgi3_session_manager_la_LDFLAGS = -module -lpthread

AM_CPPFLAGS = -I../include -I../config
AM_CXXFLAGS = -pthread

noinst_HEADERS = simple_session_manager.h