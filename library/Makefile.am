lib_LTLIBRARIES = libfastcgi3-container.la

libfastcgi3_container_la_SOURCES = component.cpp config.cpp cookie.cpp except.cpp \
	handler.cpp handlerset.cpp loader.cpp logger.cpp parser.cpp request.cpp \
	stream.cpp util.cpp xml.cpp componentset.cpp \
	component_factory.cpp component_context.cpp data_buffer.cpp string_buffer.cpp \
	server.cpp request_thread_pool.cpp globals.cpp response_time_statistics.cpp request_filter.cpp \
	attributes_holder.cpp session.cpp session_manager.cpp http_servlet.cpp http_request.cpp http_response.cpp \
	security_principal.cpp security_subject.cpp security_authenticator.cpp security_realm.cpp \
	mmap_file.cpp file_buffer.cpp

AM_CPPFLAGS = -I../include -I../config @xml_CFLAGS@
AM_CXXFLAGS = -pthread
AM_LDFLAGS = -lpthread -ldl -lfcgi -lfcgi++ -lssl -lcrypto @xml_LIBS@
