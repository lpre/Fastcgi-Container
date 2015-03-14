AC_DEFUN([AX_CHECK_FASTCGI],
[
	ac_have_fcgi="yes"
	
	ac_stored_libs=$LIBS
	ac_stored_cppflags="$CPPFLAGS"

	AC_CHECK_LIB(fcgi, FCGX_Accept_r, [], [ac_have_fcgi="no"])
	
	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS
	LIBS="$ac_stored_libs -lfcgi -lfcgi++"
	
	AC_MSG_CHECKING([linkage with libfcgi++])
	AC_LINK_IFELSE(
		[AC_LANG_PROGRAM([#include <fcgio.h>], [[char buf[256]; fcgi_streambuf stbuf(buf, sizeof(buf));]])],
		[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); ac_have_fcgi="no"])
	
	LIBS=$ac_stored_libs
	AC_LANG_RESTORE

	if test "f$ac_have_fcgi" = "fyes"; then
		ifelse([$1], , :, [$1])
	else
		ifelse([$2], , :, [$2])
	fi
])
