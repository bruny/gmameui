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
#include "filter.h"
#include "audit.h"

#define SELECT_TIMEOUT 110
#define ICON_TIMEOUT 100
#define SEARCH_TIMEOUT 1000

typedef enum {
	GAMENAME,
	HAS_SAMPLES,
	ROMNAME,
	TIMESPLAYED,
	MANU,
	YEAR,
	CLONE,
	DRIVER,
	MAMEVER,
	CATEGORY,
	NUMBER_COLUMN   /* 10 */
} Columns_type;

/* Those are also columns. The only difference is that they are hidden */
typedef enum {
	ROMENTRY = NUMBER_COLUMN,
	TEXTCOLOR,
	PIXBUF,
	HAS_ROMS,
	ROMOF,
	STATUS,
	DRIVERSTATUS,   /* Good | Imperfect | Preliminary */
	COLOR_STATUS,	/* Good | Imperfect | Preliminary */
	SOUND_STATUS,	/* Good | Imperfect | Preliminary */
	GRAPHIC_STATUS,	/* Good | Imperfect | Preliminary */
	CONTROL,
	VECTOR,
	CPU1,
	CPU2,
	CPU3,
	CPU4,
	SOUND1,
	SOUND2,
	SOUND3,
	SOUND4,
	NUMPLAYERS,
	NUMBUTTONS,
	FAVORITE,
	IS_BIOS,
	CHANNELS,
	NUMBER_COLUMN_TOTAL /* 35 */
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
	/* Architecture type filters */
	NEOGEO,
	CPS1,
	CPS2,
	CPS3,
	NAMCOS22,
	SEGAS16,
	SEGAM2,
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
	FILTER_CONTROL_TRACKBALL,	/* 16 */
	FILTER_CONTROL_LIGHTGUN,
	ORIENTATION,
	STEREO,
	FILTER_BIOS,
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

/* FIXME TODO Try and get rid of this struct */
struct gui_prefs_struct {
	RomEntry *current_game;
	MameGamelist *gl;
	GmameuiAudit *audit;
};

struct gui_prefs_struct gui_prefs;

/* global variables */
XmameExecutable *current_exec; /* pointer in the xmame_table */

GMAMEUIFilter *selected_filter;

Joystick *joydata;
GdkPixbuf *Status_Icons[NUMBER_STATUS];

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
process_inp_function (RomEntry *rom, gchar *file, int action);

void
launch_emulation (RomEntry *rom, const gchar *command_line);

void
play_game (RomEntry *rom);

void
exit_gmameui (void);

#if 0
GList*
get_columns_shown_list (void);
#endif

#endif /* __GMAMEUI_H__ */
