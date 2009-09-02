/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
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

	void (*col_toggled) (MameGuiPrefs *prefs, GValueArray *cols);
	void (*theprefix_toggled) (MameGuiPrefs *prefs, gboolean theprefix);
	void (*prefercustomicons_toggled) (MameGuiPrefs *prefs, gboolean prefercustomicons);
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
	PROP_CURRENT_ROMFILTER,
	PROP_CURRENT_MODE,
	PROP_PREVIOUS_MODE,
	PROP_COLS_SHOWN,
	PROP_COLS_WIDTH,
	PROP_SORT_COL,
	PROP_SORT_COL_DIR,
	PROP_XPOS_FILTERS,
	PROP_XPOS_GAMELIST,
	/* Startup preferences */
	PROP_GAMECHECK,
	PROP_VERSIONCHECK,
	PROP_USEXMAMEOPTIONS,
	PROP_PREFERCUSTOMICONS,
	PROP_USEJOYINGUI,
	PROP_JOYSTICKNAME,
	/* Miscellaneous preferences */
	PROP_THEPREFIX,
	PROP_CURRENT_ROM,
	PROP_CURRENT_EXECUTABLE,
	/* Executable, ROM and Sample paths - handled using GValueArrays, so
	   different to other directory handling */
	PROP_EXECUTABLE_PATHS,
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
	PROP_DIR_CTRLR,
	PROP_FILE_CATVER,
	PROP_FILE_CHEAT,
	PROP_FILE_HISCORE,
	PROP_FILE_HISTORY,
	PROP_FILE_MAMEINFO,
	PROP_DIR_CFG,
	PROP_DIR_HISCORE,
	PROP_DIR_NVRAM,
	PROP_DIR_STATE,
	PROP_DIR_INP,
	PROP_DIR_MEMCARD,
	PROP_DIR_DIFF,
	PROP_DIR_INI,
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
	DIR_CTRLR,
	FILE_CATVER,
	FILE_CHEAT,
	FILE_HISCORE,
	FILE_HISTORY,
	FILE_MAMEINFO,
	DIR_CFG,
	DIR_HISCORE,
	DIR_NVRAM,
	DIR_STATE,
	DIR_INP,
	DIR_MEMCARD,
	DIR_DIFF,
	DIR_INI,
	NUM_DIRS
};

/* FIXME TODO - Have default value which is used if the key file is null - base it on io.c references when loading file */
static const directory_config directory_prefs [] = {
	{ PROP_DIR_ARTWORK, "dir-artwork", "artwork" },
	{ PROP_DIR_SNAPSHOT, "dir-snapshot", "snap" },
	{ PROP_DIR_FLYER, "dir-flyer", "flyers" },
	{ PROP_DIR_CABINET, "dir-cabinet", "cabs" },
	{ PROP_DIR_MARQUEE, "dir-marquee", "marquee" },
	{ PROP_DIR_TITLE, "dir-title", "titles" },
	{ PROP_DIR_CPANEL, "dir-cpanel", "cpanel" },
	{ PROP_DIR_ICONS, "dir-icons", "icons" },
	{ PROP_DIR_CTRLR, "dir-ctrlr", "ctrlr" },

	{ PROP_FILE_CATVER, "file-catver", "catver.ini" },
	{ PROP_FILE_CHEAT, "file-cheat", "cheat.dat" },
	{ PROP_FILE_HISCORE, "file-hiscore", "hiscore.dat" },
	{ PROP_FILE_HISTORY, "file-history", "history.dat" },
	{ PROP_FILE_MAMEINFO, "file-mameinfo", "mameinfo.dat" },
	
	/* The following are per-user directories, and will not be user-configured. As a
	   result, the default directory will be nested under ~/.config/mame/<dir>. The
	   directory will be built when g_object_get is invoked. These properties
	   deliberately do not have a g_object_set clause */
	{ PROP_DIR_CFG, "dir-cfg", "cfg" },
	{ PROP_DIR_HISCORE, "dir-hiscore", "hi" },
	{ PROP_DIR_NVRAM, "dir-nvram", "nvram" },   /* Save game non-volatile RAM, e.g. for settings and hi-scores, dependant on driver support for each game */
	{ PROP_DIR_STATE, "dir-state", "sta" },   /* Save and load the 'state' of the ROM upon quitting the game */
	{ PROP_DIR_INP, "dir-inp", "inp" },
	{ PROP_DIR_MEMCARD, "dir-memcard", "memcard" },
	{ PROP_DIR_DIFF, "dir-diff", "diff" },		/* Hard drive diff files */
	{ PROP_DIR_INI, "dir-ini", "ini" },
	
	/* TODO
	-comment
	*/
};

GType mame_gui_prefs_get_type (void);
MameGuiPrefs* mame_gui_prefs_new (void);

/* FIXME TODO Move to gui_prefs_dialog.c */
GtkWidget *
create_gui_prefs_window (void);

G_END_DECLS


#endif /* __GUI_PREFS_H__ */
