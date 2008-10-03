dnl Process this file with autoconf to produce a configure script.

AC_INIT([gmameui], [0.2.6])
AC_PREREQ(2.5)

AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

AC_ISC_POSIX
AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_FUNCS(strchr)
AC_CHECK_LIB(z,inflate,,AC_MSG_ERROR(Cannot find zlib))
AC_CHECK_LIB(expat, XML_ParserCreate,,
  AC_CHECK_LIB(xmlparse, XML_ParserCreate,,AC_MSG_ERROR(Cannot find libexpat))
)

dnl uncomment these to test for use of deprecated GTK functions.
dnl AC_DEFINE(GTK_DISABLE_DEPRECATED,,Do not support deprecated GTK+ widgets)
dnl AC_DEFINE(GDK_DISABLE_DEPRECATED,,Do not support deprecated GDK widgets) 
dnl AC_DEFINE(GDK_PIXBUF_DISABLE_DEPRECATED,,Do not support deprecated GDK-pixbuf widgets) 

AM_PATH_GTK_2_0(2.4.0, , AC_MSG_ERROR(Cannot find GTK2))

dnl Check if the version of GTK supports gtk_show_uri for spawning Help
AM_PATH_GTK_2_0(2.13.4,AC_DEFINE(ENABLE_GTKSHOWURI, 1, Enable gtk_show_uri to spawn Help),AC_MSG_WARN(Version of GTK does not support gtk_show_uri))

PKG_CHECK_MODULES(GLADE2, libglade-2.0, [], AC_MSG_ERROR([*** libglade2 not found!]))
AC_SUBST(GLADE2_CFLAGS)
AC_SUBST(GLADE2_LIBS)

dnl Need to add a way to not build using libgnome if user is using XFCE for e.g.
dnl or if GTK2.13 is available, since libgnome is only used to spawn Help
AC_ARG_ENABLE(libgnome,
  AC_HELP_STRING([--enable-libgnome],[include libgnome support (default: yes)])
  ,,enable_libgnome=yes)

AC_MSG_CHECKING(whether libgnome support should be included)
if test ${enable_libgnome} = no; then
   AC_MSG_RESULT([no])
else
   AC_MSG_RESULT([yes])

   dnl libgnome support included, needed to check for modules, and define
   dnl it so that IFDEF code can be built
   PKG_CHECK_MODULES(GNOME, libgnome-2.0, AC_DEFINE(ENABLE_LIBGNOME, 1, libgnome support will be built), AC_MSG_ERROR([*** libgnome2 not found!]))
   AC_SUBST(GNOME_CFLAGS)
   AC_SUBST(GNOME_LIBS)
fi

AC_PATH_PROG(gmameuipath, gmameui)
dnl Subst PACKAGE_PIXMAPS_DIR.
PACKAGE_PIXMAPS_DIR="${prefix}/${DATADIRNAME}/pixmaps/${PACKAGE}"
AC_SUBST(PACKAGE_PIXMAPS_DIR)

GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["${GETTEXT_PACKAGE}"], [gettext domain])

AM_GNU_GETTEXT_VERSION([0.12.1])
AM_GNU_GETTEXT

AC_PROG_INTLTOOL

dnl ##########################
dnl Gnome-Doc-Utils
dnl ##########################
AC_ARG_ENABLE(doc,
  AC_HELP_STRING([--disable-doc],[do not build documentation (default: yes)])
  ,,enable_doc=yes)

AC_MSG_CHECKING(whether documentation should be built)
if test ${enable_doc} = no; then
   AC_MSG_RESULT([no])
else
   AC_MSG_RESULT([yes])
fi

if test ${enable_doc} = yes; then
GNOME_DOC_INIT
else
dnl Do not care if GDU is not found
GNOME_DOC_INIT(,,[:]) 
dnl ENABLE_SK_TRUE="#"
dnl ENABLE_SK_FALSE=""
fi

AM_CONDITIONAL(DISABLE_DOC, test ${enable_doc} = no)

AC_ARG_WITH([xmame-dir],
  AC_HELP_STRING([--with-xmame-dir=DIR],[default path for xmame files]),
  AC_DEFINE_UNQUOTED(XMAME_ROOT, "$withval", [Path to xmame data files]),
  AC_DEFINE(XMAME_ROOT, "/usr/lib/games/xmame",[Path to xmame data files]))

AC_ARG_ENABLE([debug], 
  AC_HELP_STRING([--enable-debug],[compile with debugging support (default: no)])
  ,,enable_debug=no)

AC_ARG_ENABLE([joystick],
  AC_HELP_STRING([--enable-joystick],[compile with joystick support (default: auto)])
  ,,enable_joystick=auto)

if test "$enable_debug" = yes ; then
	if test "$GCC" = yes; then
		CFLAGS="-Wall -Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wcast-align -Wsign-compare -std=gnu89 -ggdb -g3 -O0"
	else
		CFLAGS="-g -O0"
	fi
	AC_DEFINE(ENABLE_DEBUG, 1, [Enable debug code])
else
	if test "$GCC" = yes; then
		CFLAGS="-Wall -Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wcast-align -Wsign-compare -std=gnu89 -O2 -fomit-frame-pointer -ffast-math -pipe"
	else
		CFLAGS="-O2"
	fi
fi

case $host_os in
  mingw32*)
    CFLAGS="$CFLAGS -mms-bitfields" ;
esac

if test "$enable_joystick" = auto ; then
     AC_CHECK_HEADERS([linux/joystick.h],
                     [enable_joystick=yes],
                     [enable_joystick=no])

elif test "$enable_joystick" = yes ; then
    AC_CHECK_HEADERS([linux/joystick.h])
fi

if test "$enable_joystick" = yes; then
    AC_DEFINE(ENABLE_JOYSTICK, 1, [Enable joystick support])
fi

AC_CONFIG_FILES([ Makefile
src/Makefile
data/Makefile
intl/Makefile
po/Makefile.in
help/Makefile
gmameui.spec
])

AC_OUTPUT

echo
echo $PACKAGE $VERSION
echo
echo Print debugging messages...... : $enable_debug
echo Generate documentation........ : $enable_doc
echo Joystick support.............. : $enable_joystick
echo
eval eval echo GMAMEUI will be installed in $bindir.
if test "x$gmameuipath" != "x" ; then
	echo Warning: You have an old copy of gmameui at $gmameuipath.
fi
echo
echo configure complete, now type \'make\'
echo

