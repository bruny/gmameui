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

#ifndef __GMAMEUI_H__
#define __GMAMEUI_H__

#include "common.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtktreemodel.h>

#include "gtkjoy.h"
#include "xmame_executable.h"
#include "game_list.h"

#define SELECT_TIMEOUT 110
#define ICON_TIMEOUT 100
#define SEARCH_TIMEOUT 1000

#define EXECUTABLE 1
#define GAMELIST 2

#define BUTTON_WIDTH 90

typedef enum {
	GAMENAME,
	HAS_ROMS,
	HAS_SAMPLES,
	ROMNAME,
	VECTOR,
	CONTROL,
	TIMESPLAYED,
	MANU,
	YEAR,
	CLONE,
	DRIVER,
	STATUS,       /*  Available / Not Available */
	ROMOF,
	DRIVERSTATUS, /*  Working / Not Working */
COLOR_STATUS,	/* Good | Imperfect | Preliminary */
SOUND_STATUS,	/* Good | Imperfect | Preliminary */
GRAPHIC_STATUS,	/* Good | Imperfect | Preliminary */
	NUMPLAYERS,
	NUMBUTTONS,
	CPU1,
	CPU2,
	CPU3,
	CPU4,
	SOUND1,
	SOUND2,
	SOUND3,
	SOUND4,
	MAMEVER,
	CATEGORY,
	FAVORITE,
	CHANNELS,
	NUMBER_COLUMN
} Columns_type;

/* Those are also columns. The only difference is that they are hidden */
typedef enum {
	ROMENTRY = NUMBER_COLUMN,
	TEXTCOLOR,
	PIXBUF,
	NUMBER_COLUMN_TOTAL
} Columns_type_hidden;

typedef enum  {
	SNAPSHOTS,
	FLYERS,
	CABINETS,
	MARQUEES,
	TITLES,
	CONTROL_PANELS
} screenshot_type;

typedef enum  {
	NULL_FOLDER_ID,	/* 0	just for the folderID compliance*/
	ALL,		/* 1 */
	AVAILABLE,	/* 2 */
	UNAVAILABLE,	/* 3 */
	FILTER_INCORRECT,
	NEOGEO,		/* 4 */
	CPS1,		/* 18 */
	CPS2,		/* 19 */
IMPERFECT,
IMPERFECT_COLORS,
IMPERFECT_SOUND,
IMPERFECT_GRAPHIC,
	MANUFACTURERS, 	/* 5 just for the folderID compliance*/
	YEARS, 		/* 6 just for the folderID compliance*/
	WORKING,	/* 7 */
	NONWORKING,	/* 8 */
	CUSTOMS, 	/* 9 just for the folderID compliance*/
	PLAYED,		/* 10 */
	FAVORITES,	/* 11 */
	ORIGINALS,	/* 12 */
	CLONES,		/* 13 */
	RASTERS,	/* 14 */
	VECTORS,	/* 15 */
SAMPLES,
	TRACKBALL,	/* 16 */
	STEREO,
	DRIVERS,
	CATEGORIES,
	VERSIONS,
	NUMBER_FOLDER
} folder_filters_list;

typedef enum {
	LIST,
	LIST_TREE,
	DETAILS,
	DETAILS_TREE
} ListMode;

struct gui_prefs_struct {
	RomEntry *current_game;
	/* 1:All, 2:available, 3=unavailable, 4=NeoGeo, 5:Manufacturers
	   6=Years, 7=Working, 8=NonWorking, 9=Custom, 10=Played, 11=Favorites
	   12=originals, 13=clones 14=Raster, 15=Vector, 16=Trackball, 17=Stereo
	   18=CPS1, 19=CPS2
	   Mame32K			| GMAMEUI
	   20=Namco 21=hack...		| 22=3D Games
	   got the year in good order, just need to find the good ofsset given by the number of manufacturer
	   240=year>1980 		| 293=year>1980
	   ...				| ...
	   248=<U>			| 301=<U>
	   249=1986			| 302=1986
	   ...				| ...
	   266=2000			| 319=2000
	  will be able to stick with mame32k only with first folders or
	  I have to assign a folder ID to all manufacturer and year at
	  the condition that I got the same manufacturers list.*/
	folder_filters_list FolderID;
	/*Show screenshot panel*/
	gboolean ShowScreenShot;
	/* 0:Snaps, 1:Flyers, 2:Cabinets, 3:Marquees, 4:Titles?*/
	gboolean ShowScreenShotTab;
	screenshot_type ShowFlyer;
	gint ShowToolBar;
	gboolean ShowStatusBar;
	gboolean ShowFolderList;
	gboolean GameCheck;
	gboolean VersionCheck;
/*	gint MMXCheck;*/
	gboolean ModifyThe;
/*	gint UseFavoriteNetPlay;*/
/*	gint UseIMEInChat;*/
/*	gint ChatDrawMode;*/
	gint SortColumn;
	gboolean SortReverse;
	/* Link table between absolute column Id and displayed column number */
	gint ColumnShownId[NUMBER_COLUMN];
	gint ColumnHiddenId[NUMBER_COLUMN];  /* Only needed on column_layout */
	gint GUIPosX;
	gint GUIPosY;
	gint GUIWidth;
	gint GUIHeight;
/*set by local
	gchar *Language;*/
	ListMode current_mode;
	ListMode previous_mode;

	//gint Splitters[2];
	gint *Splitters;
	gchar *ListFont;
	GdkFont *ListFontStruct;
	guint ListFontHeight;
	/* Width of each column ex: width of "game name" column is ColumnWidth[0] */
	//gint ColumnWidth[NUMBER_COLUMN];
gint *ColumnWidth;
	/* Order of each column ex: "game name" column is the ColumnOrder[0] th column */
	//gint ColumnOrder[NUMBER_COLUMN];
gint *ColumnOrder;
	/* ColumnShown[0]==true -> "game name" column is shown */
	//gboolean ColumnShown[NUMBER_COLUMN];
gint *ColumnShown;
	gchar *SaveVersion;
	GdkColor clone_color;
	gint ResetGUI;
	gint ResetGameDefaults;
	
	/* directories specification */
	gchar **RomPath;
	gchar **SamplePath;
	gchar *ArtworkDirectory;
	gchar *SnapshotDirectory;
	gchar *HiscoreDirectory;
	gchar *FlyerDirectory;
	gchar *CabinetDirectory;
	gchar *MarqueeDirectory;
	gchar *TitleDirectory;
	gchar *CPanelDirectory;
	gchar *DiffDirectory;

	gchar *IconDirectory;

	gchar *InputDirectory;
	gchar *NVRamDirectory;
	gchar *MemCardDirectory;
	gchar *ConfigDirectory;
	gchar *StateDirectory;
	gchar *CtrlrDirectory;
	gchar *inipath;
	
	/* specifics option to GMAMEUI (path to catver, common path...)
	   not in dir.ini */
	gchar *catverDirectory;
	gchar *CheatFile;
 	gchar *HiscoreFile;
 	gchar *HistoryFile;
 	gchar *MameInfoFile;
	
	/* GUI Joystick options */
	gboolean gui_joy;
	gchar *Joystick_in_GUI;
	/* Don't pass any options to xmame */
	gboolean use_xmame_options;
};

typedef struct {
	folder_filters_list FolderID;
	Columns_type type;
	gboolean is;
	const gchar *value;
	gint int_value; /* should be a union with value */
	gboolean update_list;
} simple_filter;

/* global variables */
XmameExecutable *current_exec; /* pointer in the xmame_table */

guint visible_games;

simple_filter *current_filter;
gboolean displayedlist_keypressed;
Joystick *joydata;
GdkPixbuf *Status_Icons[NUMBER_STATUS];

struct gui_prefs_struct gui_prefs;

gboolean dirty_icon_cache;

/* Column number that is used in the gamelist popup menu */
gint ColumnHide_selected;

/* Debug Functions */
void
column_debug (void);

/* Others Functions */
gboolean
game_filtered (RomEntry * rom);

void
launch_emulation (RomEntry *rom, const gchar *command_line);

void
play_game (RomEntry *rom);

void
exit_gmameui (void);

const gchar *rom_entry_get_list_name(RomEntry *rom);

void
playback_game (RomEntry *rom, gchar *user_data);

void
record_game (RomEntry *rom, gchar *user_data);

/* Columns management functions */
void
update_columns_tab (void);

GList*
get_columns_shown_list (void);

GList*
get_columns_hidden_list (void);

const char *
column_title (int column_num);

#endif /* __GMAMEUI_H__ */
