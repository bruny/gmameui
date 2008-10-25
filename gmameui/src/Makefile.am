## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gmameui

BUILT_SOURCES = \
	gmameui-marshaller.c gmameui-marshaller.h

gmameui_SOURCES = \
	common.c common.h \
	gmameui.c gmameui.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	io.c io.h \
	game_list.h game_list.c \
	game_options.h game_options.c \
	rom_entry.h rom_entry.c \
	mameio.c \
	gui.c gui.h \
	gmameui-gamelist-view.c gmameui-gamelist-view.h \
	progression_window.c progression_window.h \
	directories.c directories.h \
	about.c about.h \
	audit.c audit.h \
	gmameui-audit-dlg.c gmameui-audit-dlg.h \
	properties.c properties.h \
	options.c options.h \
	options_string.c options_string.h \
	gtkjoy.c gtkjoy.h \
	gui_prefs.c gui_prefs.h \
	gui_prefs_dialog.c gui_prefs_dialog.h \
	unzip.c unzip.h \
	keyboard.c keyboard.h \
	xmame_options.h xmame_options.c \
	xmame_executable.h xmame_executable.c \
	mame-exec-list.h mame-exec-list.c \
	mame_options.c mame_options.h \
	mame_options_dialog.c mame_options_dialog.h \
	cell-renderer-captioned-image.c cell-renderer-captioned-image.h \
	filters_list.c filters_list.h \
	filter.c filter.h \
	gossip-cell-renderer-expander.c gossip-cell-renderer-expander.h \
	$(BUILT_SOURCES)

REDUNDANT_FILES = \
	network_game.h network_game.c 

gmameui-marshaller.h: gmameui-marshallers.list 
	glib-genmarshal --prefix=gmameui_marshaller --header $< > $@

gmameui-marshaller.c: gmameui-marshallers.list 
	echo "#include \"gmameui-marshaller.h\"" > $@ && \
	glib-genmarshal --prefix=gmameui_marshaller --body $< >> $@

gmameui_LDADD = @GTK_LIBS@ $(GLADE2_LIBS) $(GNOME_LIBS) $(INTLLIBS) 

AM_CPPFLAGS = -g -Wall $(GTK_CFLAGS) $(GLADE2_CFLAGS) $(GNOME_CFLAGS) \
		-DDATADIR=\""$(datadir)"\" \
		-DGLADEDIR=\"${datadir}/gmameui/glade/\" \
		-DGMAMEUI_UI_DIR=\"${datadir}/gmameui/ui/\" \
		-DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\"

ui_DATA = gmameui-ui.xml
uidir = $(datadir)/gmameui/ui/

EXTRA_DIST = $(ui_DATA) gmameui-marshallers.list $(REDUNDANT_FILES)

CLEANFILES = $(BUILT_SOURCES)
