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

#ifndef __GAME_LIST_H__
#define __GAME_LIST_H__

#include "mame-exec.h"
#include "rom_entry.h"

G_BEGIN_DECLS

/* Gamelist object */
#define MAME_TYPE_GAMELIST            (mame_gamelist_get_type ())
#define MAME_GAMELIST(o)            (G_TYPE_CHECK_INSTANCE_CAST((o), MAME_TYPE_GAMELIST, MameGamelist))
#define MAME_GAMELIST_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST ((k), MAME_TYPE_GAMELIST, MameGamelistClass))
#define MAME_IS_GAMELIST(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_GAMELIST))
#define MAME_IS_GAMELIST_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_GAMELIST))
#define MAME_GAMELIST_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_GAMELIST, MameGamelistClass))

typedef struct _MameGamelist MameGamelist;
typedef struct _MameGamelistClass MameGamelistClass;
typedef struct _MameGamelistPrivate MameGamelistPrivate;

struct _MameGamelist {
	GObject parent;
	
	MameGamelistPrivate *priv;
	/* define public instance variables here */
};

struct _MameGamelistClass {
	GObjectClass parent;
	/* define vtable methods and signals here */
};

/* Preferences */
enum
{
	PROP_GAMELIST_0,
	PROP_GAMELIST_NAME,
	PROP_GAMELIST_VERSION,
	PROP_GAMELIST_NUM_GAMES,
	PROP_GAMELIST_NUM_SAMPLES,
	NUM_GAMELIST_PROPERTIES
};

GType mame_gamelist_get_type (void);
MameGamelist* mame_gamelist_new (void);

void gamelist_check (MameExec *exec);

MameRomEntry* get_rom_from_gamelist_by_name (MameGamelist *gl, gchar *romname);
GList* mame_gamelist_get_roms_glist (MameGamelist *gl);
GList* mame_gamelist_get_categories_glist (MameGamelist *gl);
GList* mame_gamelist_get_versions_glist (MameGamelist *gl);

void mame_gamelist_add_driver (MameGamelist *gl, const gchar *driver);
void mame_gamelist_add_year (MameGamelist *gl, const gchar *year);
void mame_gamelist_add_version (MameGamelist *gl, gchar *version);
void mame_gamelist_add_category (MameGamelist *gl, gchar *category);
void mame_gamelist_add_manufacturer (MameGamelist *gl, gchar *manufacturer);
void mame_gamelist_set_not_checked_list (MameGamelist *gl, GList *source);

/**
* Creates a new gamelist using current_exec
* Code in mameio.c
*/
gboolean gamelist_parse (MameExec *exec);

/**
* Loads the game list from the gamelist file.
*/
gboolean mame_gamelist_load (MameGamelist *gl);

/**
* Saves the game list to the gamelist file.
*/
gboolean mame_gamelist_save (MameGamelist *gl);

void mame_gamelist_add (MameGamelist *gl, MameRomEntry *rom);

G_END_DECLS

#endif
