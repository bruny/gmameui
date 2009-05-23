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

#ifndef __ROM_ENTRY_H__
#define __ROM_ENTRY_H__

#include "common.h"

#define MAX_ROMNAME 20
#define MAX_CPU 20
#define MAX_CONTROL 20

/* Maximum number of CPUs/Sound CPUs per game
changing this will break the gamelist compatibility
*/
#define NB_CPU 4

typedef struct {
	gchar *name;
	guint clock;
	gboolean sound_flag;
} CPUInfo;

typedef struct {
	gchar *name;
	guint clock;
} SoundCPUInfo;

typedef enum {
	INCORRECT,
	CORRECT,
	UNKNOWN,
	PROBLEMS,
	BEST_AVAIL,
	NOT_AVAIL,
	NOTROMSET,
	NUMBER_STATUS
} RomStatus;

typedef enum {
	JOYSTICK,
	TRACKBALL,
	LIGHTGUN
} ControlType;

typedef enum {
	DRIVER_STATUS_GOOD,
	DRIVER_STATUS_IMPERFECT,
	DRIVER_STATUS_PRELIMINARY,
	DRIVER_STATUS_UNKNOWN,
	NUM_DRIVER_STATUS
} DriverStatus;

/*
gchar *driver_status_str[NUM_DRIVER_STATUS] = {
	"Good",
	"Imperfect",
	"Preliminary",
	"Unknown"
};*/

static gchar* rom_status_string_value[NUMBER_STATUS] = {
	N_("Incorrect"),
	N_("Correct"),
	N_("Unknown"),
	N_("Problems"),
	N_("Best Available"),
	N_("Not Available"),
	N_("Not a valid set")
};

/** 
* Information for a loaded game.
* A RomEntry is invalid when the gamelist changes because there
* are several pointers to the list.
*
*/
typedef struct {
	gchar romname[MAX_ROMNAME];
	gchar *gamename;
	gchar *gamenameext;

	/** The year for the game.
	* This is a pointer to a string in the game_list.years g_list
	*/
	const gchar *year;
	gchar *manu;
	gchar *cloneof;
	gchar *romof;   /* This appears to be the same as cloneof in the XML output */
	gchar *sampleof;
	
	gboolean is_bios;
	
	/** The driver for the game.
	This is a pointer to a string in the game_list.drivers g_list
	*/
	const gchar *driver;
	CPUInfo cpu_info[NB_CPU];
	SoundCPUInfo sound_info[NB_CPU];
	ControlType control;

	/** The category for the game.
	This is a pointer to a string in the game_list.categories g_list
	*/
	const gchar *category;
	const gchar *mame_ver_added;
	gint num_players;
	gint num_buttons;
	gint channels;
	gboolean vector;
	DriverStatus status;

	/** Driver status
	Recent versions of MAME have good | imperfect or | preliminary
	*/
	DriverStatus driver_status_emulation;
	DriverStatus driver_status_color;
	DriverStatus driver_status_sound;
	DriverStatus driver_status_graphic;
	
	gboolean horizontal;
	guint screen_x;
	guint screen_y;
	gfloat screen_freq;
	guint colors;

	gint nb_roms;
	gint nb_samples;
	gboolean the_trailer;

	/* gmameui information 
	  (from game.ini)
	*/
	gint timesplayed;
	RomStatus has_roms;
	RomStatus has_samples;
	gboolean favourite;

	/* Runtime information */

	/** position of the game in the GTKTreeView */
	GtkTreeIter position;   /* FIXME TODO Delete this */
	gboolean is_in_list;	/* FIXME TODO Delete this */
	/** string in order to sort the clones with the original */
	gchar *clonesort;
	/** store the icon of the game in the RomEntry struct
	   when game is visible on the list, used as cache  */
	GdkPixbuf *icon_pixbuf;
	/** Name in list */
	gchar *name_in_list;
} RomEntry;

/**
* Frees a rom entry.
*/
void rom_entry_free (RomEntry *rom_entry);

/**
* Creates a new rom entry.
* Use rom_entry_free () to free this.
*/
RomEntry * rom_entry_new (void);


void rom_entry_set_name   (RomEntry *rom, gchar *value);
void rom_entry_set_driver (RomEntry *rom, const gchar *driver);
void rom_entry_set_year   (RomEntry *rom, const gchar *year);
gchar **rom_entry_get_manufacturers (RomEntry * rom);
const gchar *rom_entry_get_list_name(RomEntry *rom);
void rom_entry_set_list_name(RomEntry *rom, gboolean the_prefix);
ControlType get_control_type (gchar *control_type);
DriverStatus get_driver_status (gchar *driver_status);

#endif
