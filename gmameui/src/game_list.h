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

#ifndef __GAME_LIST_H__
#define __GAME_LIST_H__

#include "common.h"
#include <stdio.h>
#include "xmame_executable.h"

#include "rom_entry.h"



typedef struct {
	gchar *name;
	gchar *version;
	gint num_games;
	gint num_sample_games;
	GList *roms;
	GList *years;
	GList *manufacturers;
	GList *drivers;
	GList *categories;
	GList *versions;
	GList *not_checked_list;
} GameList;

GameList game_list;

const gchar * glist_insert_unique (GList **list, const gchar *data);

void gamelist_init  (void);
void gamelist_add   (RomEntry *rom);
void gamelist_free  (void);

void gamelist_check (XmameExecutable *exec);

int xmame_exec_get_game_count(XmameExecutable *exec);

RomEntry* get_rom_from_gamelist_by_name (gchar *romname);
void rom_entry_set_driver (RomEntry *rom, const gchar *driver);
void rom_entry_set_year   (RomEntry *rom, const gchar *year);

/**
* Creates a new gamelist using current_exec
* Code in mameio.c
*/
gboolean gamelist_parse (XmameExecutable *exec);

/**
* Loads the game list from the gamelist file.
*/
gboolean gamelist_load (void);

/**
* Saves the game list to the gamelist file.
*/
gboolean gamelist_save (void);

#endif
