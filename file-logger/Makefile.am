pkglib_LTLIBRARIES = fastcgi3-filelogger.la

fastcgi3_filelogger_la_SOURCES = file_logger.cpp
fastcgi3_filelogger_la_LIBADD = ../library/libfastcgi3-container.la
fastcgi3_filelogger_la_LDFLAGS = -module -lpthread

AM_CPPFLAGS = -I../include -I../config
AM_CXXFLAGS = -pthread

noinst_HEADERS = file_logger.h