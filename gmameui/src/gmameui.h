/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
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
#include "mame-exec.h"
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
TEXTSTYLE,
FILTERED,
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
	CONTROL_PANELS,
	NUM_GMAMEUI_PICTURE_TYPES
} screenshot_type;

typedef enum  {
	/* Availability */
	FILTER_ALL,
	FILTER_AVAILABLE,
	FILTER_UNAVAILABLE,
	/* Status */
	FILTER_CORRECT,
	FILTER_BEST_AVAIL,
	FILTER_INCORRECT,
	/* Architecture type filters */
	FILTER_ARCH_NEOGEO,
	FILTER_ARCH_CPS1,
	FILTER_ARCH_CPS2,
	FILTER_ARCH_CPS3,
	FILTER_ARCH_NAMCOS22,
	FILTER_ARCH_SEGAS16,
	FILTER_ARCH_SEGAM2,
	FILTER_ARCH_BIOS,
/* TODO	IMPERFECT,  * i.e. imperfect color/sound/graphics */
	FILTER_IMPERFECT_COLORS,
	FILTER_IMPERFECT_SOUND,
	FILTER_IMPERFECT_GRAPHIC,

	FILTER_DETAILS_ORIGINALS,
	FILTER_DETAILS_CLONES,
	FILTER_DETAILS_SAMPLES,
	FILTER_DETAILS_STEREO,
	FILTER_DETAILS_RASTERS,
	FILTER_DETAILS_VECTORS,

	FILTER_CONTROL_TRACKBALL,
	FILTER_CONTROL_LIGHTGUN,
/*TODO	FILTER_ORIENTATION_V,
	FILTER_ORIENTATION_H,*/

	FILTER_CUSTOM_PLAYED,
	FILTER_CUSTOM_FAVORITES,

/*TODO	MANUFACTURERS,
	YEARS,
	WORKING,
	NONWORKING,
	CUSTOMS,
	
	DRIVERS,*/
	FILTER_CATEGORIES,
	FILTER_VERSIONS,
	NUM_FILTERS
} folder_filters_list;

typedef enum {
	LIST,
#ifdef TREESTORE
	LIST_TREE,
#endif
	DETAILS,
#ifdef TREESTORE
	DETAILS_TREE
#endif
} ListMode;

/* FIXME TODO Try and get rid of this struct */
struct gui_prefs_struct {
	RomEntry *current_game;
	MameGamelist *gl;
	GmameuiAudit *audit;
};

struct gui_prefs_struct gui_prefs;

GMAMEUIFilter *selected_filter;

Joystick *joydata;
GdkPixbuf *Status_Icons[NUMBER_STATUS];

gboolean dirty_icon_cache;

/* Others Functions */
gboolean
game_filtered (RomEntry * rom, gint rom_filter_opt);

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
