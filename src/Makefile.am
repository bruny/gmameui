## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src data po intl

EXTRA_DIST = autogen.sh
	gmameui.desktop.in \
	gmameui.png \
	gmameui.6 \
	BUGS \
	pixmaps-builtin \
	setup-gettext \
	intltool-update.in \
	intltool-merge.in \
	intltool-extract.in \
	m4 \
	Doxyfile \
	po/LINGUAS \
	debian 

Graphicsdir = $(datadir)/applications
Graphics_in_files = gmameui.desktop.in
Graphics_DATA = $(Graphics_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = gmameui.png 

icondir = $(datadir)/gmameui
icon_DATA = 	gmameui.png \
		gmameui-general.png \
		gmameui-screen.png

docdir = $(datadir)/doc/$(PACKAGE)
doc_DATA = BUGS NEWS COPYING AUTHORS README TODO ChangeLog

man_MANS = gmameui.6

distclean-local:
	-rm -rf autom4te.cache

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(datadir)/pixmaps/$(PACKAGE); \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(datadir)/pixmaps/$(PACKAGE); \
	    fi \
	  done \
	fi

dist-hook:
	rm -rf `find $(distdir) -name CVS`
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

