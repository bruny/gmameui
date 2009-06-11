/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
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

#include <string.h>

#include "rom_entry.h"
#include "gui.h"	/* For main_gui.gui_prefs */

static void
mame_rom_entry_save_int (MameRomEntry *rom, GParamSpec *param, gpointer user_data);

ControlType get_control_type (gchar *control_type)
{
	ControlType type;
	
	g_return_val_if_fail (control_type != NULL, 0);

	/* FIXME TODO There are additional types to add */
	if (g_ascii_strcasecmp (control_type, "trackball") == 0)
		type = TRACKBALL;
	else if (g_ascii_strcasecmp (control_type, "lightgun") == 0)
		type = LIGHTGUN;
	else
		type = JOYSTICK;
	
	return type;
}

DriverStatus get_driver_status (gchar *driver_status)
{
	DriverStatus status;
	
	g_return_val_if_fail (driver_status != NULL, 0);

	/* FIXME TODO There are additional types to add */
	if (g_ascii_strcasecmp (driver_status, "good") == 0)
		status = DRIVER_STATUS_GOOD;
	else if (g_ascii_strcasecmp (driver_status, "imperfect") == 0)
		status = DRIVER_STATUS_IMPERFECT;
	else if (g_ascii_strcasecmp (driver_status, "preliminary") == 0)
		status = DRIVER_STATUS_PRELIMINARY;
	else
		status = DRIVER_STATUS_UNKNOWN;
	
	return status;
}


static void mame_rom_entry_class_init (MameRomEntryClass *klass);
static void mame_rom_entry_init (MameRomEntry *pr);
static void mame_rom_entry_finalize (GObject *obj);
static void mame_rom_entry_set_property (GObject *object,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *pspec);
static void mame_rom_entry_get_property (GObject *object,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *pspec);

G_DEFINE_TYPE (MameRomEntry, mame_rom_entry, G_TYPE_OBJECT)

struct _MameRomEntryPrivate {
	
	gchar *romname;
	
	gchar *name_in_list;    /* Used to present full name in the listview */
	gchar *gamename;
	gchar *gamenameext;     /* e.g. (rev 1) or (World) */
	
	/** The year for the game.
	* This is a pointer to a string in the game_list.years g_list
	*/
	const gchar *year;
	gchar *manu;
	gchar *cloneof;
	gchar *romof;   /* This appears to be the same as cloneof in the XML output */
	gchar *sampleof;
	/** The driver for the game.
	This is a pointer to a string in the game_list.drivers g_list
	*/
	const gchar *driver;
	gboolean is_bios;
	
	/** The category for the game.
	This is a pointer to a string in the game_list.categories g_list
	*/
	/*const */gchar *category;
	/*const */gchar *mame_ver_added;
	
	gint num_players;
	gint num_buttons;
	ControlType control;
	gint num_channels;
	gboolean is_vector;

	gboolean is_horizontal;
	guint screen_x;
	guint screen_y;
	guint num_colours;      /* AKA palettesize */
	gfloat screen_freq;
	
	gint num_roms;
	gint num_samples;
	
	CPUInfo cpu_info[NB_CPU];
	SoundCPUInfo sound_info[NB_CPU];
	
	/* Local information (stored in games.ini) */
	gint timesplayed;
	RomStatus has_roms;
	RomStatus has_samples;
	gboolean is_favourite;
	
	/** Driver status
	    Recent versions of MAME have good | imperfect or | preliminary
	*/
	DriverStatus driver_status;
	DriverStatus driver_status_emulation;
	DriverStatus driver_status_color;
	DriverStatus driver_status_sound;
	DriverStatus driver_status_graphics;
	
	/* String to sort the clones next to the original (will be original-clone) */
	gchar *clonesort;
	
	gboolean the_trailer;
	
	/* Status icon or from icons.zip */
	/* FIXME TODO Have both? */
	GdkPixbuf *icon_pixbuf;
	
	GtkTreeIter position;
};

static void
mame_rom_entry_set_property (GObject *object,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *pspec)
{
	MameRomEntry *rom = MAME_ROM_ENTRY (object);

	switch (prop_id) {
		case PROP_ROM_ROMNAME:
			rom->priv->romname = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_GAMENAME:
			rom->priv->gamename = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_GAMENAMEEXT:
			rom->priv->gamenameext = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_MANUFACTURER:
			rom->priv->manu = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_YEAR:
			rom->priv->year = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_CLONEOF:
			rom->priv->cloneof = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_ROMOF:
			rom->priv->romof = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_SAMPLEOF:
			rom->priv->sampleof = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_DRIVER:
			rom->priv->driver = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_DRIVER_STATUS:
			rom->priv->driver_status = g_value_get_int (value);
			break;
		case PROP_ROM_DRIVER_STATUS_EMULATION:
			rom->priv->driver_status_emulation = g_value_get_int (value);
			break;
		case PROP_ROM_DRIVER_STATUS_COLOUR:
			rom->priv->driver_status_color = g_value_get_int (value);
			break;
		case PROP_ROM_DRIVER_STATUS_SOUND:
			rom->priv->driver_status_sound = g_value_get_int (value);
			break;
		case PROP_ROM_DRIVER_STATUS_GRAPHICS:
			rom->priv->driver_status_graphics = g_value_get_int (value);
			break;
		
		case PROP_ROM_NUMPLAYERS:
			rom->priv->num_players = g_value_get_int (value);
			break;
		case PROP_ROM_NUMBUTTONS:
			rom->priv->num_buttons = g_value_get_int (value);
			break;
		case PROP_ROM_CONTROLTYPE:
			rom->priv->control = g_value_get_int (value);
			break;
		case PROP_ROM_CHANNELS:
			rom->priv->num_channels = g_value_get_int (value);
			break;
		case PROP_ROM_TIMESPLAYED:
			rom->priv->timesplayed = g_value_get_int (value);
			break;
		case PROP_ROM_HAS_ROMS:
			rom->priv->has_roms = g_value_get_int (value);
			break;
		case PROP_ROM_HAS_SAMPLES:
			rom->priv->has_samples = g_value_get_int (value);
			break;
		case PROP_ROM_IS_VECTOR:
			rom->priv->is_vector = g_value_get_boolean (value);
			break;
		case PROP_ROM_IS_HORIZONTAL:
			rom->priv->is_horizontal = g_value_get_boolean (value);
			break;
		case PROP_ROM_SCREEN_X:
			rom->priv->screen_x = g_value_get_int (value);
			break;
		case PROP_ROM_SCREEN_Y:
			rom->priv->screen_y = g_value_get_int (value);
			break;
		case PROP_ROM_NUM_COLOURS:
			rom->priv->num_colours = g_value_get_int (value);
			break;
		case PROP_ROM_SCREEN_FREQ:
			rom->priv->screen_freq = g_value_get_float (value);
			break;
		case PROP_ROM_IS_FAVOURITE:
			rom->priv->is_favourite = g_value_get_boolean (value);
			break;
		case PROP_ROM_NUM_ROMS:
			rom->priv->num_roms = g_value_get_int (value);
			break;
		case PROP_ROM_NUM_SAMPLES:
			rom->priv->num_samples = g_value_get_int (value);
			break;
		case PROP_ROM_THE_TRAILER:
			rom->priv->the_trailer = g_value_get_boolean (value);
			break;

		case PROP_ROM_CATEGORY:
			rom->priv->category = g_strdup (g_value_get_string (value));
			break;
		case PROP_ROM_VER_ADDED:
			rom->priv->mame_ver_added = g_strdup (g_value_get_string (value));
			break;
			
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}

}

static void
mame_rom_entry_get_property (GObject *object,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *pspec)
{
	MameRomEntry *rom = MAME_ROM_ENTRY (object);

	switch (prop_id) {
		case PROP_ROM_ROMNAME:
			g_value_set_string (value, rom->priv->romname);
			break;
		case PROP_ROM_GAMENAME:
			g_value_set_string (value, rom->priv->gamename);
			break;
		case PROP_ROM_GAMENAMEEXT:
			g_value_set_string (value, rom->priv->gamenameext);
			break;
		case PROP_ROM_MANUFACTURER:
			g_value_set_string (value, rom->priv->manu);
			break;
		case PROP_ROM_YEAR:
			g_value_set_string (value, rom->priv->year);
			break;
		case PROP_ROM_CLONEOF:
			g_value_set_string (value, rom->priv->cloneof);
			break;
		case PROP_ROM_ROMOF:
			g_value_set_string (value, rom->priv->romof);
			break;
		case PROP_ROM_SAMPLEOF:
			g_value_set_string (value, rom->priv->sampleof);
			break;
		case PROP_ROM_DRIVER:
			g_value_set_string (value, rom->priv->driver);
			break;
		case PROP_ROM_DRIVER_STATUS:
			g_value_set_int (value, rom->priv->driver_status);
			break;
		case PROP_ROM_DRIVER_STATUS_EMULATION:
			g_value_set_int (value, rom->priv->driver_status_emulation);
			break;
		case PROP_ROM_DRIVER_STATUS_COLOUR:
			g_value_set_int (value, rom->priv->driver_status_color);
			break;
		case PROP_ROM_DRIVER_STATUS_SOUND:
			g_value_set_int (value, rom->priv->driver_status_sound);
			break;
		case PROP_ROM_DRIVER_STATUS_GRAPHICS:
			g_value_set_int (value, rom->priv->driver_status_graphics);
			break;
		
		case PROP_ROM_NUMPLAYERS:
			g_value_set_int (value, rom->priv->num_players);
			break;
		case PROP_ROM_NUMBUTTONS:
			g_value_set_int (value, rom->priv->num_buttons);
			break;
		case PROP_ROM_CONTROLTYPE:
			g_value_set_int (value, rom->priv->control);
			break;
		case PROP_ROM_CHANNELS:
			g_value_set_int (value, rom->priv->num_channels);
			break;	
			
		case PROP_ROM_TIMESPLAYED:
			g_value_set_int (value, rom->priv->timesplayed);
			break;
		case PROP_ROM_HAS_ROMS:
			g_value_set_int (value, rom->priv->has_roms);
			break;
		case PROP_ROM_HAS_SAMPLES:
			g_value_set_int (value, rom->priv->has_samples);
			break;
		case PROP_ROM_IS_VECTOR:
			g_value_set_boolean (value, rom->priv->is_vector);
			break;
		case PROP_ROM_IS_HORIZONTAL:
			g_value_set_boolean (value, rom->priv->is_horizontal);
			break;
		case PROP_ROM_SCREEN_X:
			g_value_set_int (value, rom->priv->screen_x);
			break;
		case PROP_ROM_SCREEN_Y:
			g_value_set_int (value, rom->priv->screen_y);
			break;
		case PROP_ROM_NUM_COLOURS:
			g_value_set_int (value, rom->priv->num_colours);
			break;
		case PROP_ROM_SCREEN_FREQ:
			g_value_set_float (value, rom->priv->screen_freq);
			break;
		case PROP_ROM_IS_FAVOURITE:
			g_value_set_boolean (value, rom->priv->is_favourite);
			break;
		case PROP_ROM_NUM_ROMS:
			g_value_set_int (value, rom->priv->num_roms);
			break;
		case PROP_ROM_NUM_SAMPLES:
			g_value_set_int (value, rom->priv->num_samples);
			break;
		case PROP_ROM_THE_TRAILER:
			g_value_set_boolean (value, rom->priv->the_trailer);
			break;
			
		case PROP_ROM_CATEGORY:
			g_value_set_string (value, rom->priv->category);
			break;
		case PROP_ROM_VER_ADDED:
			g_value_set_string (value, rom->priv->mame_ver_added);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
mame_rom_entry_finalize (GObject *obj)
{
	
	MameRomEntry *rom = MAME_ROM_ENTRY (obj);
	
	g_return_if_fail (rom != NULL);
	
	/* Clear all ROM fields */
	g_free (rom->priv->romname);
	g_free (rom->priv->name_in_list);
	g_free (rom->priv->gamename);
	g_free (rom->priv->gamenameext);

	g_free (rom->priv->manu);
	g_free (rom->priv->cloneof);
	g_free (rom->priv->romof);
	g_free (rom->priv->sampleof);

	g_free (rom->priv->category);
	g_free (rom->priv->mame_ver_added);
	
	/* FIXME TODO
	 cpu_info
	 sound_info
	 position
	*/

	/* clonesort may point to previously free'd romname */
	if (rom->priv->clonesort != rom->priv->romname)
		g_free (rom->priv->clonesort);
	
	if (rom->priv->icon_pixbuf)
		g_object_unref (rom->priv->icon_pixbuf);
		
// FIXME TODO	g_free (pr->priv);

}

static void
mame_rom_entry_class_init (MameRomEntryClass *klass)
{
	
/*
	 FIXME TODO
	 
	 When the following properties change, update games.ini keyfile:

		PlayCount
		HasRoms
		HasSamples
		Favorite
	in save_games_ini and load_games_ini
*/

	/* Use G_PARAM_CONSTRUCT to set default values when object is constructed */
	
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->set_property = mame_rom_entry_set_property;
	object_class->get_property = mame_rom_entry_get_property;
	object_class->finalize = mame_rom_entry_finalize;

	g_object_class_install_property (object_class,
					 PROP_ROM_TIMESPLAYED,
					 g_param_spec_int ("times-played", "", "", 0, G_MAXINT, 0, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_HAS_ROMS,
					 g_param_spec_int ("has-roms", "", "", 0, NUMBER_STATUS, UNKNOWN, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_HAS_SAMPLES,
					 g_param_spec_int ("has-samples", "", "", 0, NUMBER_STATUS, UNKNOWN, G_PARAM_READWRITE));
	
	/* Information about the ROM */
	g_object_class_install_property (object_class,
					 PROP_ROM_ROMNAME,
					 g_param_spec_string ("romname", "", "", "", G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_GAMENAME,
					 g_param_spec_string ("gamename", "", "", "", G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_GAMENAMEEXT,
					 g_param_spec_string ("gamenameext", "", "", "", G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
					 PROP_ROM_MANUFACTURER,
					 g_param_spec_string ("manufacturer", "", "", _("Unknown"), G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
					 PROP_ROM_YEAR,
					 g_param_spec_string ("year", "", "", _("Unknown"), G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
					 PROP_ROM_CLONEOF,
					 g_param_spec_string ("cloneof", "", "", "-", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
					 PROP_ROM_SAMPLEOF,
					 g_param_spec_string ("sampleof", "", "", "-", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
					 PROP_ROM_ROMOF,
					 g_param_spec_string ("romof", "", "", "-", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
					 PROP_ROM_DRIVER,
					 g_param_spec_string ("driver", "", "", "", G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
					 PROP_ROM_DRIVER_STATUS,
					 g_param_spec_int ("driver-status", "", "", 0, NUM_DRIVER_STATUS, DRIVER_STATUS_UNKNOWN, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_DRIVER_STATUS_EMULATION,
					 g_param_spec_int ("driver-status-emulation", "", "", 0, NUM_DRIVER_STATUS, DRIVER_STATUS_UNKNOWN, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_DRIVER_STATUS_COLOUR,
					 g_param_spec_int ("driver-status-colour", "", "", 0, NUM_DRIVER_STATUS, DRIVER_STATUS_UNKNOWN, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_DRIVER_STATUS_SOUND,
					 g_param_spec_int ("driver-status-sound", "", "", 0, NUM_DRIVER_STATUS, DRIVER_STATUS_UNKNOWN, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_DRIVER_STATUS_GRAPHICS,
					 g_param_spec_int ("driver-status-graphics", "", "", 0, NUM_DRIVER_STATUS, DRIVER_STATUS_UNKNOWN, G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
					 PROP_ROM_NUMPLAYERS,
					 g_param_spec_int ("num-players", "", "", 0, 8, 1, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_NUMBUTTONS,
					 g_param_spec_int ("num-buttons", "", "", 0, 16, 1, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_CONTROLTYPE,
					 g_param_spec_int ("control-type", "", "", 0, NUM_CONTROL_TYPE, JOYSTICK, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_CHANNELS,
					 g_param_spec_int ("num-channels", "", "", 0, 4, 0, G_PARAM_READWRITE));	
	
	g_object_class_install_property (object_class,
					 PROP_ROM_IS_VECTOR,
					 g_param_spec_boolean ("is-vector", "", "", FALSE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_IS_HORIZONTAL,
					 g_param_spec_boolean ("is-horizontal", "", "", FALSE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_SCREEN_X,
					 g_param_spec_int ("screenx", "", "", 0, 1024, 0, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_SCREEN_Y,
					 g_param_spec_int ("screeny", "", "", 0, 1024, 0, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_NUM_COLOURS,
					 g_param_spec_int ("num-colours", "", "", 0, 65536, 0, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_SCREEN_FREQ,
					 g_param_spec_float ("screen-freq", "", "", 0, 90, 0, G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
					 PROP_ROM_IS_FAVOURITE,
					 g_param_spec_boolean ("is-favourite", "", "", FALSE, G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
					 PROP_ROM_NUM_ROMS,
					 g_param_spec_int ("num-roms", "", "", 0, 128, 0, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_NUM_SAMPLES,
					 g_param_spec_int ("num-samples", "", "", 0, 128, 0, G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
					 PROP_ROM_THE_TRAILER,
					 g_param_spec_boolean ("the-trailer", "", "", FALSE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_CATEGORY,
					 g_param_spec_string ("category", "", "", _("Unknown"), G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
					 PROP_ROM_VER_ADDED,
					 g_param_spec_string ("version-added", "", "", _("Unknown"), G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
mame_rom_entry_init (MameRomEntry *rom)
{
	
	rom->priv = g_new0 (MameRomEntryPrivate, 1);
	
	/* Set the details for the games.ini file 
	don't keep this within each rom!!
	pr->priv->filename = g_build_filename (g_get_home_dir (), ".gmameui", "games.ini", NULL);

	pr->priv->rom_ini_file = g_key_file_new ();
	GError *error = NULL;*/
	
/* FIXME TODO
	 for (i = 0; i < NB_CPU; i++) {
		rom->priv->cpu_info[i].name = g_strdup ("-");
		rom->priv->sound_info[i].name = g_strdup ("-");
	}*/
	
	/* Set handlers so that whenever the values are changed (from anywhere), the signal handler
	   is invoked; the callback then saves to the g_key_file */
	g_signal_connect (rom, "notify::has-roms", (GCallback) mame_rom_entry_save_int, NULL);
	g_signal_connect (rom, "notify::has-samples", (GCallback) mame_rom_entry_save_int, NULL);
	g_signal_connect (rom, "notify::is-favourite", (GCallback) mame_rom_entry_save_int, NULL);

}

MameRomEntry* mame_rom_entry_new (void)
{
	return g_object_new (MAME_TYPE_ROM_ENTRY, NULL);
}

static void
mame_rom_entry_save_int (MameRomEntry *rom, GParamSpec *param, gpointer user_data)
{
/* FIXME TODO Save to GKeyFile for games.ini */
}

void
mame_rom_entry_set_name (MameRomEntry *rom, gchar *value)
{
	char *p;
	
	g_return_if_fail (rom != NULL);

	if (!g_strncasecmp (value, "The ",4))
	{
		value += 4;
		rom->priv->the_trailer = TRUE;
		rom->priv->name_in_list = NULL;
	} else {
		rom->priv->name_in_list = g_strdup (value);
	}

	for (p = value; *p && (*p != '/') && (*p != '(');p++);

	if (*p == '/') {

		/*Fix for F/A (Japan)*/
		if (!strncmp ((p - 1), "F/A", 3))
		{
			rom->priv->gamenameext = g_strdup (p + 2);
			*(p + 2) = '\0';
		}
		else {
			rom->priv->gamenameext = g_strdup (p + 1);
			*p = '\0';
		}
	
	} else if (*p =='(') {
		rom->priv->gamenameext = g_strdup (p);
		*p = 0;
	}

	if (!rom->priv->gamenameext)
		rom->priv->gamenameext = g_strdup ("");

	rom->priv->gamename = g_strdup (value);
}

/* FIXME The strdups below should not be replicated in the calling function */
void
mame_rom_entry_set_romname (MameRomEntry *rom, gchar *romname)
{
	rom->priv->romname = g_strdup (romname);
}

void
mame_rom_entry_set_gamename (MameRomEntry *rom, gchar *gamename)
{
	rom->priv->gamename = g_strdup (gamename);
}

void
mame_rom_entry_set_gamenameext (MameRomEntry *rom, gchar *gamenameext)
{
	rom->priv->gamenameext = g_strdup (gamenameext);
}

void
mame_rom_entry_set_category_version (MameRomEntry *rom, gchar *category, gchar *version)
{
	/* These are pointers to the values, rather than copies, since there
	   are so many duplicated instances */
	rom->priv->category = g_strdup (category);
	rom->priv->mame_ver_added = g_strdup (version);
}

void mame_rom_entry_set_cloneof (MameRomEntry *rom, gchar *clone)
{
	if (clone)
		rom->priv->cloneof = g_strdup (clone);
}

void mame_rom_entry_set_romof (MameRomEntry *rom, gchar *romof)
{
	if (romof)
		rom->priv->romof = g_strdup (romof);
}

void mame_rom_entry_set_isbios (MameRomEntry *rom, gboolean isbios)
{
	rom->priv->is_bios = isbios;
}

void
mame_rom_entry_set_driver (MameRomEntry    *rom,
		      const gchar *driver)
{
	rom->priv->driver = g_strdup (driver);
	mame_gamelist_add_driver (gui_prefs.gl, driver);
}

void
mame_rom_entry_set_year (MameRomEntry    *rom,
		    const gchar *year)
{
	rom->priv->year = g_strdup (year);
	mame_gamelist_add_year (gui_prefs.gl, year);
}

void
mame_rom_entry_set_manufacturer (MameRomEntry *rom, const gchar *manufacturer)
{
	rom->priv->manu = g_strdup (manufacturer);
}

void
mame_rom_entry_set_position (MameRomEntry *rom, GtkTreeIter position)
{
	rom->priv->position = position;
}

void
mame_rom_entry_set_icon (MameRomEntry *rom, GdkPixbuf *icon_pixbuf)
{
	rom->priv->icon_pixbuf = icon_pixbuf;
}

void
mame_rom_entry_set_default_fields (MameRomEntry *rom)
{
	if (g_ascii_strcasecmp (rom->priv->cloneof, "-") == 0) {
		/* original. Point to romname */
		rom->priv->clonesort = rom->priv->romname;

	} else {
		rom->priv->clonesort = g_strdup_printf ("%s-%s",
			rom->priv->cloneof , rom->priv->romname);
	}

}


gchar *
mame_rom_entry_get_clonesort (MameRomEntry *rom)
{
	return rom->priv->clonesort;
}

gboolean
mame_rom_entry_has_samples (MameRomEntry *rom)
{
	return (rom->priv->num_samples > 0);
}

gboolean
mame_rom_entry_is_bios (MameRomEntry *rom)
{
	return (rom->priv->is_bios);
}

gboolean
mame_rom_entry_is_favourite (MameRomEntry *rom)
{
	return (rom->priv->is_favourite);
}

gboolean
mame_rom_entry_is_vector (MameRomEntry *rom)
{
	return (rom->priv->is_vector);
}

gboolean
mame_rom_entry_is_clone (MameRomEntry *rom)
{
	if (g_ascii_strcasecmp (rom->priv->cloneof, "-") == 0)  //not working - default values stuffed
		return FALSE;
	else
		return TRUE;
}

RomStatus
mame_rom_entry_get_rom_status (MameRomEntry *rom)
{
	g_return_val_if_fail ((rom->priv->has_roms >= 0) &&
			      (rom->priv->has_roms < NUMBER_STATUS),
			      INCORRECT);

	return (rom->priv->has_roms);
}

RomStatus
mame_rom_entry_get_sample_status (MameRomEntry *rom)
{
	return (rom->priv->has_samples);
}

const gchar *
mame_rom_entry_get_list_name (MameRomEntry *rom)
{
	gboolean the_prefix;
	
	g_object_get (main_gui.gui_prefs, "theprefix", &the_prefix, NULL);
	
	if (rom->priv->name_in_list != NULL)
		return rom->priv->name_in_list;

	if (!rom->priv->the_trailer) {
		if (!rom->priv->name_in_list) {
			rom->priv->name_in_list = g_strdup_printf ("%s %s", rom->priv->gamename, rom->priv->gamenameext);
		}
	} else  {	
		if (the_prefix) {
			if (!rom->priv->name_in_list || strncmp (rom->priv->name_in_list, "The", 3)) {
				g_free (rom->priv->name_in_list);
				rom->priv->name_in_list = g_strdup_printf ("The %s %s", rom->priv->gamename, rom->priv->gamenameext);
			}
		} else {
			if (!rom->priv->name_in_list || !strncmp (rom->priv->name_in_list, "The", 3)) {
				g_free (rom->priv->name_in_list);
				rom->priv->name_in_list = g_strdup_printf ("%s, The %s", rom->priv->gamename, rom->priv->gamenameext);
			}
		}
	}

	return rom->priv->name_in_list;
}

const gchar *
mame_rom_entry_get_romname (MameRomEntry *rom)
{
	return rom->priv->romname;
}

const gchar *
mame_rom_entry_get_gamename (MameRomEntry *rom)
{
	return rom->priv->gamename;
}

const gchar *
mame_rom_entry_get_parent_romname (MameRomEntry *rom)
{
	return rom->priv->cloneof;
}

const gchar *
mame_rom_entry_get_year (MameRomEntry *rom)
{
	return rom->priv->year;
}

const gchar *
mame_rom_entry_get_manufacturer (MameRomEntry *rom)
{
	return rom->priv->manu;
}

gchar **
mame_rom_entry_get_manufacturers (MameRomEntry *rom)
{
	gchar **manufacturer_fields;

	g_return_val_if_fail (rom->priv->manu != NULL, NULL);

	manufacturer_fields = g_strsplit (rom->priv->manu, "]", 0);
	if (!manufacturer_fields[0])
		return NULL;

	if (manufacturer_fields[1])
	{
		/* we have two winners [Company 1] (company 2)*/
		g_strstrip (g_strdelimit (manufacturer_fields[0], "[", ' '));
		g_strstrip (g_strdelimit (manufacturer_fields[1], "()", ' '));
	} else
	{
		g_strfreev (manufacturer_fields);
		manufacturer_fields = g_strsplit (rom->priv->manu, "+", 0);
		if (manufacturer_fields[1] != NULL)
		{
			/* we have two winners  Company1+Company2*/
			g_strstrip (g_strdelimit (manufacturer_fields[0], "[", ' '));
			g_strstrip (g_strdelimit (manufacturer_fields[1], "()", ' '));
		} else
		{
			g_strfreev (manufacturer_fields);
			manufacturer_fields = g_strsplit (rom->priv->manu, "/", 0);
			if (manufacturer_fields[1] != NULL)
			{
				/* we have two winners  Company1/Company2*/
				g_strstrip (g_strdelimit (manufacturer_fields[0], "[", ' '));
				g_strstrip (g_strdelimit (manufacturer_fields[1], "()", ' '));
			}
			else
			{
				g_strfreev (manufacturer_fields);
				manufacturer_fields = g_strsplit (rom->priv->manu, "(", 0);
				if (manufacturer_fields[1] != NULL)
				{
					/* we have two winners  Company1/Company2*/
					g_strstrip (g_strdelimit (manufacturer_fields[0], "[", ' '));
					g_strstrip (g_strdelimit (manufacturer_fields[1], "()", ' '));
				}
			}
		}
	}
	return manufacturer_fields;
}

void
mame_rom_entry_add_cpu (MameRomEntry *rom, int i, gchar *name, gint clock)
{
	g_return_if_fail (i <= NB_CPU);
	
	if (!strncmp (name, "(sound)", 7)) {
		gchar *p;
		p = name;
		p += 7;
		rom->priv->cpu_info[i].name = g_strdup (p);
		rom->priv->cpu_info[i].sound_flag = TRUE;
	} else {
		rom->priv->cpu_info[i].name = g_strdup (name);
		rom->priv->cpu_info[i].sound_flag = FALSE;
	}

	rom->priv->cpu_info[i].clock = clock;

	/*GMAMEUI_DEBUG ("Adding rom %s with clock %d to romset %s",
	       rom->priv->cpu_info[i].name,
	       rom->priv->cpu_info[i].clock,
	       rom->priv->romname);*/
	       
}

void
mame_rom_entry_add_soundcpu (MameRomEntry *rom, int i, gchar *name, gint clock)
{
	g_return_if_fail (i <= NB_CPU);

	if (strcmp (name, "")) {
		rom->priv->sound_info[i].name = g_strdup (name);
	}
	rom->priv->sound_info[i].clock = clock;

/*	GMAMEUI_DEBUG ("Adding sound rom %s with clock %d to romset %s",
	       rom->priv->sound_info[i].name,
	       rom->priv->sound_info[i].clock,
	       rom->priv->romname);*/
}

void
mame_rom_entry_add_rom (MameRomEntry *rom)
{
	g_return_if_fail (rom != NULL);
	g_return_if_fail (rom->priv != NULL);
	
	rom->priv->num_roms++;
}

void
mame_rom_entry_add_sample (MameRomEntry *rom)
{
	rom->priv->num_samples++;
}

void
mame_rom_entry_rom_played (MameRomEntry *rom, gboolean warning, gboolean error)
{
	g_return_if_fail (rom != NULL);
	
	/* FIXME TODO Set g_object rom info, which triggers signal to update game in list.
	   This will then replace update_game_in_list call below */
	rom->priv->timesplayed++;
	
	/* Update game information if there was a ROM error or warning,
	   otherwise set to correct (if it wasn't already) */
	if (error)
		rom->priv->has_roms = INCORRECT;
	else if (warning)
		rom->priv->has_roms = BEST_AVAIL;
	else
		rom->priv->has_roms = CORRECT;
}

gchar *
get_rom_clone_name (MameRomEntry *rom)
{
	gchar *value;
	
	g_return_val_if_fail (mame_rom_entry_is_clone (rom), NULL);

	/* find the clone name if there is a clone */
	MameRomEntry *tmprom;

	tmprom = get_rom_from_gamelist_by_name (gui_prefs.gl, rom->priv->cloneof);
	if (tmprom) {
		value = g_strdup_printf ("%s - \"%s\"", mame_rom_entry_get_list_name (rom), rom->priv->cloneof);
	} else {
		value = g_strdup_printf (" - \"%s\"", rom->priv->cloneof);
	}

	return value;
}

CPUInfo *
get_rom_cpu (MameRomEntry *rom, int i)
{
	if (!rom->priv->cpu_info[i].name) {
		rom->priv->cpu_info[i].name = "-";
		rom->priv->cpu_info[i].clock = 0;
	}
	return &rom->priv->cpu_info[i];
}

SoundCPUInfo *
get_sound_cpu (MameRomEntry *rom, int i)
{
	if (!rom->priv->sound_info[i].name) {
		rom->priv->sound_info[i].name = "-";
		rom->priv->sound_info[i].clock = 0;
	}
	return &rom->priv->sound_info[i];
}

gchar *
get_rom_cpu_value (MameRomEntry *rom)
{
	char *value = NULL;
	char *values [NB_CPU + 1];
	gint  i, j;

	j = 0;
	values[j] = NULL;

	for (i = 0; (i < NB_CPU) && (rom->priv->cpu_info[i].name); i++) {
		/* There is an array of CPUInfo structs; games that have less
		   less than NB_CPU CPUs will have the names of the non-present
		   CPUs set to "-", i.e. empty */
		if (g_ascii_strcasecmp (rom->priv->cpu_info[i].name, "-")) {
			values[j++] = g_strdup_printf ("%s %f MHz%s",
						       rom->priv->cpu_info[i].name,
						       rom->priv->cpu_info[i].clock / 1000000.0,
						       rom->priv->cpu_info[i].sound_flag ? _(" (sound)") : " ");
		}
	}
	values[j++] = NULL;

	value = g_strjoinv ("\n", values);

	for (i = 0; i < j; i++)
		g_free (values[i]);

	return value;
}

gchar *
get_rom_sound_value (MameRomEntry *rom)
{
	char *value = NULL;
	char *values [NB_CPU + 1];
	gint  i, j;

	j = 0;
	values[j] = NULL;
	for (i = 0; (i < NB_CPU) && (rom->priv->sound_info[i].name); i++) {
		/* There is an array of SoundCPUInfo structs; games that have less
		   less than NB_CPU CPUs will have the names of the non-present
		   CPUs set to "-", i.e. empty */
		if (g_ascii_strcasecmp (rom->priv->sound_info[i].name, "-")) {
			if (rom->priv->sound_info[i].clock == 0)
				values[j++] = g_strdup_printf ("%s", rom->priv->sound_info[i].name);
			else
				values[j++] = g_strdup_printf ("%s %f MHz",
							      rom->priv->sound_info[i].name,
							      rom->priv->sound_info[i].clock / 1000000.0);
		}
	}
	values[j++] = NULL;

	value = g_strjoinv ("\n", values);

	for (i = 0; i < j; i++)
		g_free (values[i]);

	return value;
}

gchar*
mame_rom_entry_get_resolution (MameRomEntry *rom)
{
	gchar *res;
	res = g_strdup_printf ("%i \303\227 %i ", rom->priv->screen_x, rom->priv->screen_y);
	return res;
}

gfloat
mame_rom_entry_get_screen_freq (MameRomEntry *rom)
{
	return rom->priv->screen_freq;
}

GdkPixbuf *
mame_rom_entry_get_icon (MameRomEntry *rom)
{
	return rom->priv->icon_pixbuf;
}

GtkTreeIter
mame_rom_entry_get_position (MameRomEntry *rom)
{
	return rom->priv->position;
}


