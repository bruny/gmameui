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

#ifndef __ROM_ENTRY_H__
#define __ROM_ENTRY_H__

#include "common.h"
#include "mame-exec.h"
#include "gmameui-romfix-list.h"	/* FIXME TODO Included to reference structs - should we link to it here? */

G_BEGIN_DECLS

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
	gboolean sound_flag;	/* This seems to be a deprecated option */
} CPUInfo;

typedef struct {
	gchar *name;
	guint clock;
} SoundCPUInfo;

typedef struct {
	gchar *name;    /* Name */
	gint uncomp_size;   /* Uncompressed size in bytes */
	gchar *sha1;   	/* SHA1 checksum - AAA FIXME TODO Don't use as we have to uncompress then pass to method to get this! */
	gchar *crc;		/* CRC */
	gchar *merge;	/* Name of ROM shared with parent */
	gchar *region;	/* Type of ROM, i.e. mainbios, maincpu, fixed, fixedbios, zoomy, audiobios, audiocpu, ym, sprites */
	gchar *status;	/* Status of the known ROM (e.g. baddump, nodump, good) */
	gboolean present;
} individual_rom;

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
	LIGHTGUN,
	NUM_CONTROL_TYPE
} ControlType;

typedef enum {
	DRIVER_STATUS_GOOD,
	DRIVER_STATUS_IMPERFECT,
	DRIVER_STATUS_PRELIMINARY,
	DRIVER_STATUS_UNKNOWN,
	NUM_DRIVER_STATUS
} DriverStatus;

static const gchar* driver_status_string_value[NUM_DRIVER_STATUS] = {
	N_("Good"),
	N_("Imperfect"),
	N_("Preliminary"),
	N_("Unknown")
};

static const gchar* control_type_string_value[NUM_CONTROL_TYPE] = {
	N_("Joystick"),
	N_("Trackball"),
	N_("Lightgun")
};

static gchar* rom_status_string_value[NUMBER_STATUS] = {
	N_("Incorrect"),
	N_("Correct"),
	N_("Unknown"),
	N_("Problems"),
	N_("Best Available"),
	N_("Not Available"),
	N_("Not a valid set")
};

/* Preferences object */
#define MAME_TYPE_ROM_ENTRY            (mame_rom_entry_get_type ())
#define MAME_ROM_ENTRY(o)            (G_TYPE_CHECK_INSTANCE_CAST((o), MAME_TYPE_ROM_ENTRY, MameRomEntry))
#define MAME_ROM_ENTRY_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST ((k), MAME_TYPE_ROM_ENTRY, MameRomEntryClass))
#define MAME_IS_ROM_ENTRY(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_ROM_ENTRY))
#define MAME_IS_ROM_ENTRY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_ROM_ENTRY))
#define MAME_ROM_ENTRY_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_ROM_ENTRY, MameRomEntryClass))

typedef struct _MameRomEntry MameRomEntry;
typedef struct _MameRomEntryClass MameRomEntryClass;
typedef struct _MameRomEntryPrivate MameRomEntryPrivate;

struct _MameRomEntry {
	GObject parent;
	
	MameRomEntryPrivate *priv;
	/* define public instance variables here */
};

struct _MameRomEntryClass {
	GObjectClass parent;
	/* define vtable methods and signals here */
};

/* Preferences */
enum
{
	PROP_ROM_0,

	/* ROM properties */
	PROP_ROM_ROMNAME,
	PROP_ROM_GAMENAME,
	PROP_ROM_GAMENAMEEXT,
	PROP_ROM_CLONESORT,

	PROP_ROM_YEAR,
	PROP_ROM_MANUFACTURER,
	PROP_ROM_CLONEOF,
	PROP_ROM_ROMOF,
	PROP_ROM_SAMPLEOF,
	PROP_ROM_DRIVER,
	PROP_ROM_IS_BIOS,

	PROP_ROM_NUMPLAYERS,
	PROP_ROM_NUMBUTTONS,
	PROP_ROM_CONTROLTYPE,
	PROP_ROM_CHANNELS,

	PROP_ROM_TIMESPLAYED,
	PROP_ROM_HAS_ROMS,
	PROP_ROM_HAS_SAMPLES,

	PROP_ROM_DRIVER_STATUS,
	PROP_ROM_DRIVER_STATUS_EMULATION,
	PROP_ROM_DRIVER_STATUS_COLOUR,
	PROP_ROM_DRIVER_STATUS_SOUND,
	PROP_ROM_DRIVER_STATUS_GRAPHICS,

	PROP_ROM_IS_VECTOR,
	PROP_ROM_IS_HORIZONTAL,
	PROP_ROM_SCREEN_X,
	PROP_ROM_SCREEN_Y,
	PROP_ROM_NUM_COLOURS,
	PROP_ROM_SCREEN_FREQ,

	PROP_ROM_IS_FAVOURITE,

	PROP_ROM_NUM_ROMS,
	PROP_ROM_NUM_SAMPLES,
	PROP_ROM_THE_TRAILER,

	PROP_ROM_CATEGORY,
	PROP_ROM_VER_ADDED,

	NUM_ROM_ENTRY_PROPERTIES
};


GType mame_rom_entry_get_type (void);
MameRomEntry* mame_rom_entry_new (void);

void mame_rom_entry_set_name (MameRomEntry *rom, gchar *value);
void mame_rom_entry_set_romname (MameRomEntry *rom, gchar *romname);
void mame_rom_entry_set_gamename (MameRomEntry *rom, gchar *gamename);
void mame_rom_entry_set_gamenameext (MameRomEntry *rom, gchar *gamenameext);
void mame_rom_entry_set_category_version (MameRomEntry *rom, gchar *category, gchar *version);
void mame_rom_entry_set_cloneof (MameRomEntry *rom, gchar *clone);
void mame_rom_entry_set_romof (MameRomEntry *rom, gchar *romof);
void mame_rom_entry_set_isbios (MameRomEntry *rom, gboolean isbios);
void mame_rom_entry_set_driver (MameRomEntry *rom, const gchar *driver);
void mame_rom_entry_set_year   (MameRomEntry *rom, const gchar *year);
void mame_rom_entry_set_manufacturer   (MameRomEntry *rom, const gchar *manufacturer);
void mame_rom_entry_set_default_fields (MameRomEntry *rom);
void mame_rom_entry_set_icon (MameRomEntry *rom, GdkPixbuf *icon_pixbuf);
void mame_rom_entry_set_position (MameRomEntry *rom, GtkTreeIter iter);

gchar* mame_rom_entry_get_clonesort (MameRomEntry *rom);
gboolean mame_rom_entry_is_bios (MameRomEntry *rom);
gboolean mame_rom_entry_is_favourite (MameRomEntry *rom);
gboolean mame_rom_entry_is_vector (MameRomEntry *rom);
gboolean mame_rom_entry_is_clone (MameRomEntry *rom);

gboolean mame_rom_entry_has_samples (MameRomEntry *rom);
const gchar * mame_rom_entry_get_list_name (MameRomEntry *rom);
const gchar * mame_rom_entry_get_gamename (MameRomEntry *rom);
const gchar * mame_rom_entry_get_romname (MameRomEntry *rom);
const gchar * mame_rom_entry_get_parent_romname (MameRomEntry *rom);
const gchar * mame_rom_entry_get_year (MameRomEntry *rom);
const gchar * mame_rom_entry_get_driver (MameRomEntry *rom);
const gchar * mame_rom_entry_get_manufacturer (MameRomEntry *rom);
RomStatus mame_rom_entry_get_rom_status (MameRomEntry *rom);
RomStatus mame_rom_entry_get_sample_status (MameRomEntry *rom);
gchar* mame_rom_entry_get_resolution (MameRomEntry *rom);
gfloat mame_rom_entry_get_screen_freq (MameRomEntry *rom);
GdkPixbuf * mame_rom_entry_get_icon (MameRomEntry *rom);
GtkTreeIter mame_rom_entry_get_position (MameRomEntry *rom);

gchar **mame_rom_entry_get_manufacturers (MameRomEntry * rom);

GList* mame_rom_entry_get_brothers (MameRomEntry *rom);
gchar **mame_rom_entry_get_clones (MameRomEntry *rom);

void mame_rom_entry_add_rom (MameRomEntry *rom);
void mame_rom_entry_add_sample (MameRomEntry *rom);
void mame_rom_entry_add_cpu (MameRomEntry *rom, int i, gchar *name, gint clock);
void mame_rom_entry_add_soundcpu (MameRomEntry *rom, int i, gchar *name, gint clock);
/* FIXME TODO Combine this and mame_rom_entry_add_rom above; create equivalent sample function */
void mame_rom_entry_add_rom_ref (MameRomEntry *rom, individual_rom *rom_ref);

void mame_rom_entry_rom_played (MameRomEntry *rom, gboolean warning, gboolean error);

gchar* get_rom_clone_name (MameRomEntry *rom);
gchar* get_rom_cpu_value (MameRomEntry *rom);
gchar* get_rom_sound_value (MameRomEntry *rom);

CPUInfo * get_rom_cpu (MameRomEntry *rom, int i);
SoundCPUInfo * get_sound_cpu (MameRomEntry *rom, int i);

ControlType get_control_type (gchar *control_type);
DriverStatus get_driver_status (gchar *driver_status);

MameRomEntry *create_gamelist_xmlinfo_for_rom (MameExec *exec, MameRomEntry *rom);

GFile *mame_rom_entry_get_disk_location (gchar *romname);

// AAA FIXME TODO - Once working, make static
romset_fixes *
mame_rom_entry_find_fixes (MameRomEntry *rom);
GList *
mame_rom_entry_get_roms (MameRomEntry *rom);
void
mame_rom_entry_add_roms_to_hashtable (MameRomEntry *romset);

G_END_DECLS

#endif /* __ROM_ENTRY_H__ */
