/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on GXMame code
 * 2002-2005 Stephane Pontier <shadow_walker@users.sourceforge.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __GUI_PREFS_H__
#define __GUI_PREFS_H__

G_BEGIN_DECLS

#include <gtk/gtk.h>

/* Preferences object */
#define MAME_TYPE_GUI_PREFS            (mame_gui_prefs_get_type ())
#define MAME_GUI_PREFS(o)            (G_TYPE_CHECK_INSTANCE_CAST((o), MAME_TYPE_GUI_PREFS, MameGuiPrefs))
#define MAME_GUI_PREFS_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST ((k), MAME_TYPE_GUI_PREFS, MameGuiPrefsClass))
#define MAME_IS_GUI_PREFS(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_GUI_PREFS))
#define MAME_IS_GUI_PREFS_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_GUI_PREFS))
#define MAME_GUI_PREFS_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_GUI_PREFS, MameGuiPrefsClass))

typedef struct _MameGuiPrefs MameGuiPrefs;
typedef struct _MameGuiPrefsClass MameGuiPrefsClass;
typedef struct _MameGuiPrefsPrivate MameGuiPrefsPrivate;

typedef struct _directory_config directory_config;

struct _directory_config {
	gint prop_id;   /* e.g. PROP_DIR_ARTWORK */
	gchar *name;    /* e.g. artwork-dir, used for storing in keyfile and in signal */
	gchar *default_dir;
};

struct _MameGuiPrefs {
	GObject parent;
	
	MameGuiPrefsPrivate *priv;
	/* define public instance variables here */
};

struct _MameGuiPrefsClass {
	GObjectClass parent;
	/* define vtable methods and signals here */
};

/* Preferences */
enum
{
	PROP_0,
	/* UI preferences */
	PROP_UI_WIDTH,
	PROP_UI_HEIGHT,
	PROP_SHOW_TOOLBAR,
	PROP_SHOW_STATUSBAR,
	PROP_SHOW_FILTERLIST,
	PROP_SHOW_SCREENSHOT,
	PROP_SHOW_FLYER,
	PROP_CURRENT_MODE,
	PROP_PREVIOUS_MODE,
	PROP_COLS_SHOWN,
	PROP_COLS_WIDTH,
	PROP_SORT_COL,
	PROP_SORT_COL_DIR,
	/* Startup preferences */
	PROP_GAMECHECK,
	PROP_VERSIONCHECK,
	PROP_USEXMAMEOPTIONS,
	PROP_USEJOYINGUI,
	/* Miscellaneous preferences */
	PROP_THEPREFIX,
	PROP_CLONECOLOR,
	PROP_CURRENT_ROM,
	/* ROM and Sample paths - handled using GValueArrays, so
	   different to other directory handling */
	PROP_ROM_PATHS,
	PROP_SAMPLE_PATHS,
	/* Directory and file preferences only after this point */
	PROP_DIR_ARTWORK,
	PROP_DIR_SNAPSHOT,
	PROP_DIR_FLYER,
	PROP_DIR_CABINET,
	PROP_DIR_MARQUEE,
	PROP_DIR_TITLE,
	PROP_DIR_CPANEL,
	PROP_DIR_ICONS,
	PROP_DIR_HISCORE,   /* FIXME TODO Move this below */
	PROP_FILE_CHEAT,
	PROP_FILE_HISCORE,
	PROP_FILE_HISTORY,
	PROP_FILE_MAMEINFO,
	PROP_DIR_CFG,
	PROP_DIR_NVRAM,
	PROP_DIR_STATE,
	PROP_DIR_INP,
	PROP_DIR_MEMCARD,
	PROP_DIR_DIFF,
	NUM_PROPERTIES
};

enum {
	DIR_ARTWORK,
	DIR_SNAPSHOT,
	DIR_FLYER,
	DIR_CABINET,
	DIR_MARQUEE,
	DIR_TITLE,
	DIR_CPANEL,
	DIR_ICONS,
	DIR_HISCORE,	/* FIXME TODO Move this below */
	FILE_CHEAT,
	FILE_HISCORE,
	FILE_HISTORY,
	FILE_MAMEINFO,
	DIR_CFG,
	DIR_NVRAM,
	DIR_STATE,
	DIR_INP,
	DIR_MEMCARD,
	DIR_DIFF,
	NUM_DIRS
};

/* FIXME TODO - Have default value which is used if the key file is null - base it on io.c references when loading file */
static const directory_config directory_prefs [] = {
	{ PROP_DIR_ARTWORK, "dir-artwork", "/usr/lib/games/xmame/" },
	{ PROP_DIR_SNAPSHOT, "dir-snapshot", "/usr/lib/games/xmame/"},
	{ PROP_DIR_FLYER, "dir-flyer", "/usr/lib/games/xmame/" },
	{ PROP_DIR_CABINET, "dir-cabinet", "/usr/lib/games/xmame/" },
	{ PROP_DIR_MARQUEE, "dir-marquee", "/usr/lib/games/xmame/" },
	{ PROP_DIR_TITLE, "dir-title", "/usr/lib/games/xmame/" },
	{ PROP_DIR_CPANEL, "dir-cpanel", "/usr/lib/games/xmame/" },
	{ PROP_DIR_ICONS, "dir-icons", "/usr/lib/games/xmame/" },
	{ PROP_DIR_HISCORE, "dir-hiscore", ".gmameui/hi/" },	/* FIXME TODO Move this below */
	{ PROP_FILE_CHEAT, "file-cheat", "/usr/lib/games/xmame/cheat.dat" },
	{ PROP_FILE_HISCORE, "file-hiscore", "/usr/lib/games/xmame/hiscore.dat" },
	{ PROP_FILE_HISTORY, "file-history", "/usr/lib/games/xmame/history.dat" },
	{ PROP_FILE_MAMEINFO, "file-mameinfo", "/usr/lib/games/xmame/mameinfo.dat" },
	{ PROP_FILE_MAMEINFO, "file-mameinfo", "/usr/lib/games/xmame/mameinfo.dat" },
	
	/* The following are per-user directories, and will not be user-configured. As a
	   result, the default directory will be nested under ~/.gmameui/<dir>. The
	   directory will be built when g_object_get is invoked. These properties
	   deliberately do not have a g_object_set clause */
	{ PROP_DIR_CFG, "dir-cfg", "cfg" },
	{ PROP_DIR_NVRAM, "dir-nvram", "nvram" },   /* Save game non-volatile RAM, e.g. for settings and hi-scores, dependant on driver support for each game */
	{ PROP_DIR_STATE, "dir-state", "sta" },   /* Save and load the 'state' of the ROM upon quitting the game */
	{ PROP_DIR_INP, "dir-inp", "inp" },
	{ PROP_DIR_MEMCARD, "dir-memcard", "memcard" },
	{ PROP_DIR_DIFF, "dir-diff", "diff" },		/* Hard drive diff files */
	
	/* TODO

	-input  * Directory for saving input recording files (.inp) *
	-comment
	*/
};

GType mame_gui_prefs_get_type (void);
MameGuiPrefs* mame_gui_prefs_new (void);

GtkWidget *
create_gui_prefs_window (void);

G_END_DECLS


#endif /* __GUI_PREFS_H__ */
