/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
 * based on GXMame code
 * 2002-2005 Stephane Pontier <shadow_walker@users.sourceforge.net>
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

#ifndef __GMAMEUI_H__
#define __GMAMEUI_H__

#include "common.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtktreemodel.h>

#include "gtkjoy.h"
#include "mame-exec.h"
#include "game_list.h"
#include "gmameui-romfix-list.h"
#include "filter.h"
#include "audit.h"
#include "io.h"

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

/* Those are also columns. The only difference is that they are hidden and used
   for filtering */
typedef enum {
	ROMENTRY = NUMBER_COLUMN,
TEXTSTYLE,
FILTERED,
	PIXBUF,
	HAS_ROMS,
	ROMOF,
	STATUS,
	DRIVER_STATUS,   /* Good | Imperfect | Preliminary */
	COLOR_STATUS,	/* Good | Imperfect | Preliminary */
	SOUND_STATUS,	/* Good | Imperfect | Preliminary */
	GRAPHIC_STATUS,	/* Good | Imperfect | Preliminary */
	CONTROL,
	VECTOR,
	NUMPLAYERS,
	NUMBUTTONS,
	FAVORITE,
	IS_BIOS,
	CHANNELS,
	NUMBER_COLUMN_TOTAL
} Columns_type_hidden;

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

/* TODO	IMPERFECT,  * i.e. imperfect color/sound/graphics */
	FILTER_IMPERFECT_DRIVER,
	FILTER_IMPERFECT_COLORS,
	FILTER_IMPERFECT_SOUND,
	FILTER_IMPERFECT_GRAPHIC,

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
	DETAILS,
} ListMode;

/* FIXME TODO Try and get rid of this struct */
struct gui_prefs_struct {
	MameRomEntry *current_game;
	MameGamelist *gl;
	GmameuiAudit *audit;
	GMAMEUIIOHandler *io_handler;
	GHashTable *rom_hashtable;
	GMAMEUIRomfixList *fixes;
};

struct gui_prefs_struct gui_prefs;

GMAMEUIFilter *selected_filter;

Joystick *joydata;
GdkPixbuf *Status_Icons[NUMBER_STATUS];

gboolean dirty_icon_cache;

/* Others Functions */

void process_inp_function (MameRomEntry *rom, gchar *file, int action);

void launch_emulation (MameRomEntry *rom, const gchar *command_line);

void play_game (MameRomEntry *rom);

void exit_gmameui (void);

#endif /* __GMAMEUI_H__ */
