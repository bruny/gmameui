## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gmameui

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
	progression_window.c progression_window.h \
	directories.c directories.h \
	about.c about.h \
	audit.c audit.h \
	properties.c properties.h \
	options.c options.h \
	options_string.c options_string.h \
	gtkjoy.c gtkjoy.h \
	gui_prefs.c gui_prefs.h \
	column_layout.c column_layout.h \
	unzip.c unzip.h \
	keyboard.c keyboard.h \
	network_game.h network_game.c \
	xmame_options.h xmame_options.c \
	xmame_executable.h xmame_executable.c \
	mame_options.c mame_options.h \
	mame_options_dialog.c mame_options_dialog.h \
	cell-renderer-captioned-image.c cell-renderer-captioned-image.h

gmameui_LDADD = @GTK_LIBS@ $(GLADE2_LIBS) $(GNOME_LIBS) $(INTLLIBS)

AM_CPPFLAGS = $(GTK_CFLAGS) $(GLADE2_CFLAGS) $(GNOME_CFLAGS) \
		-DDATADIR=\""$(datadir)"\" \
		-DGLADEDIR=\"${datadir}/gmameui/glade/\" \
		-DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\"
