## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

### Don't execute make in the help subdirectory if DISABLE_DOC is set
if DISABLE_DOC
SUBDIRS = src data po intl
else
SUBDIRS = src data po intl help
endif

EXTRA_DIST = autogen.sh \
	gmameui.desktop.in \
	gmameui.png \
	gmameui-screen.png \
	gmameui-general.png \
	gmameui.6 \
	BUGS \
	setup-gettext \
	intltool-update.in \
	intltool-merge.in \
	intltool-extract.in \
	gnome-doc-utils.make \
	m4 

DISTCLEANFILES=gnome-doc-utils.make

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

rpmdir = /usr/src/rpm/SPECS
rpm_DATA = gmameui.spec

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

release: dist rpm
	mkdir -p release
	rm -f release/*
	mv -f rpmbuild/RPMS/*/*		release/
	mv -f rpmbuild/SRPMS/*		release/
	mv -f rpmbuild/SOURCES/*	release/
	rm -rf rpmbuild

pot:
	make -C po update-po

rpm: build-rpm

build-rpm:
	$(MAKE) $(PACKAGE).spec $(PACKAGE).spec.in
	$(MAKE) dist
	mkdir -p rpmbuild/{BUILD,RPMS/{noarch,athlon,i386,i486,i588,i686,x86_64},SOURCES,SPECS,SRPMS}
	mv @PACKAGE@-@VERSION@.tar.gz rpmbuild/SOURCES/
	unset MAKEFLAGS ; test ! -f $$HOME/.bashrc || . $$HOME/.bashrc; rpmbuild --define "_topdir $(PWD)/rpmbuild" --define "debug_package %{nil}" -ba @PACKAGE@.spec

