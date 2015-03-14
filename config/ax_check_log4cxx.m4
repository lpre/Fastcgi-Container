AC_DEFUN([AX_CHECK_LOG4CXX],
[
	ac_have_log4cxx="no"
	ac_log4cxx_found="no"
	AC_ARG_ENABLE(log4cxx, 
		AS_HELP_STRING(--enable-cppunit,enables compilation of log4cxx logger),
	[
		if test "f$enableval" = "fyes"; then
			ac_have_log4cxx="yes"
		fi
	])

	if test "f$ac_have_log4cxx" = "fyes"; then
		ac_log4cxx_found="yes"
		
		AC_LANG_SAVE
		AC_LANG_CPLUSPLUS
		AC_CHECK_HEADER([log4cxx/logger.h], [], [ac_log4cxx_found="no"])
	
		ac_stored_libs=$LIBS
		LIBS="$ac_stored_libs -llog4cxx"

		AC_MSG_CHECKING([linkage with log4cxx])
		AC_LINK_IFELSE(
			[AC_LANG_PROGRAM([#include <log4cxx/logger.h>], [log4cxx::LoggerPtr l = log4cxx::Logger::getRootLogger();])],
			[AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no]); ac_log4cxx_found="no"])
	
		LIBS=$ac_stored_libs
		AC_LANG_RESTORE

		if test "f$ac_log4cxx_found" = "fyes"; then
			ifelse([$1], , :, [$1])
			AC_DEFINE([HAVE_LOG4CXX], 1, [Define to 1 if you want to use log4cxx logger])
		else
			ifelse([$2], , :, [$2])
		fi	
	fi
	AM_CONDITIONAL(HAVE_LOG4CXX, [test "f$ac_log4cxx_found" = "fyes"])
])
