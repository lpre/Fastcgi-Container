bin_PROGRAMS = fastcgi3-page-compiler

fastcgi3_page_compiler_SOURCES = main.cpp page_reader.cpp page.cpp code_writer.cpp utils.cpp
fastcgi3_page_compiler_LDADD = ../library/libfastcgi3-container.la
fastcgi3_page_compiler_CPPFLAGS = -I../include -I../config 

AM_CPPFLAGS = -I@top_srcdir@/include -I@top_srcdir@/config

noinst_HEADERS = page.h page_reader.h