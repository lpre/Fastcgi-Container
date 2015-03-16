pkglib_LTLIBRARIES = example.la server_pages.la

example_la_SOURCES = example.cpp
example_la_LDFLAGS = -module
example_la_LIBADD = ../library/libfastcgi3-container.la

factory.cpp:  
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../library/libfastcgi3-container.la ../page-compiler/fastcgi3-page-compiler -updateFactory=true -updateMakefile=false *.cpsp

server_pages_la_SOURCES = factory.cpp Login.cpp
server_pages_la_LDFLAGS = -module
server_pages_la_LIBADD = ../library/libfastcgi3-container.la

AM_CPPFLAGS = -I../include -I../config
AM_CXXFLAGS = -pthread
AM_LDFLAGS =
