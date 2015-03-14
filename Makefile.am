ACLOCAL_AMFLAGS = -I config
AUTOMAKE_OPTIONS = 1.9 foreign

SUBDIRS = include library main example syslog request-cache session-manager authenticator statistics file-logger page-compiler

if HAVE_CPPUNIT
SUBDIRS += tests
endif

EXTRA_DIST = autogen.sh config/settings.h.in config/ac_cxx_gnucxx_hashmap.m4 \
	config/ac_cxx_have_ext_hash_map.m4 config/ac_cxx_namespaces.m4 \
	config/ac_cxx_stlport_hashmap.m4 config/ax_check_cppunit.m4 \
	config/ax_check_dmalloc.m4 config/ax_check_fastcgi.m4 \
	ax_check_compiler_flags.m4 config/cppunit.m4 extra/fastcgi-container3 \
	extra/fastcgistart3.sh
