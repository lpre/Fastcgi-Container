AC_DEFUN([AX_CHECK_DMALLOC],
[
	ac_have_dmalloc="no"
	ac_dmalloc_found="no"

	AC_ARG_ENABLE(dmalloc, 
		AS_HELP_STRING(--enable-dmalloc,enables dmalloc support),
	[
		if test "f$enableval" = "fyes"; then
			ac_have_dmalloc="yes";
		fi
	])

	if test "f$ac_have_dmalloc" = "fyes"; then
		
		AC_LANG_SAVE
		AC_LANG_CPLUSPLUS
		AC_CHECK_HEADER([dmalloc.h], [], [ac_dmalloc_found="no"])
		AC_CHECK_LIB(dmallocthcxx, dmalloc_shutdown, [], [ac_dmalloc_found="no"])
		AC_LANG_RESTORE
		
		if test "f$ac_dmalloc_found" = "fyes"; then
			ifelse([$1], , :, [$1])
			AC_SUBST([dmalloc_LIBS], [-ldmallocthcxx])
			AC_SUBST([dmalloc_CFLAGS], [-DDMALLOC_FUNC_CHECK])
			AC_DEFINE(HAVE_DMALLOC_H, 1, [Define to 1 if you have the <dmalloc.h> header file])
		else
			ifelse([$2], , :, [$2])
		fi
	fi
	AM_CONDITIONAL([HAVE_DMALLOC], [ test "f$ac_dmalloc_found" = "fyes" ])
])
