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

#include "game_list.h"
#include <string.h>
#include <stdlib.h>

#include "io.h"
#include "gui.h"
#include "rom_entry.h"


#define LINE_BUF 1024

/* Separator for game list fields.
* This avoids problems when source
* files are saved in different encodings.
*/
#define SEP "\xAC"


/**
* Inserts a string to a sorted glist if it's not present.
* The function always returns a pointer to the string in the list.
*/
const gchar *
glist_insert_unique (GList **list, const gchar *data) {
	GList *listpointer;
	gchar *data_copy;

	if (!data)
		return NULL;

	listpointer = g_list_first (*list);

	while ( (listpointer != NULL))
	{
		if (!strcmp (listpointer->data, data))
			return listpointer->data;
		
		listpointer = g_list_next (listpointer);
	}

	data_copy = g_strdup (data);
	*list = g_list_insert_sorted (*list, data_copy, (GCompareFunc)strcmp);
	return data_copy;
}

void
rom_entry_set_driver (RomEntry    *rom,
		      const gchar *driver)
{
	rom->driver = g_strdup (driver);
	glist_insert_unique (&game_list.drivers, driver);
}

void
rom_entry_set_year (RomEntry    *rom,
		    const gchar *year)
{
	rom->year = g_strdup (year);
	glist_insert_unique (&game_list.years, year);
}

static gint
compare_game_name (RomEntry *rom1,
		   RomEntry *rom2)
{
	return strcmp (rom1->clonesort, rom2->clonesort);
}


static FILE *
gamelist_open (const char *mode)
{
	FILE *handle;
	gchar *filename = 
		g_build_filename (g_get_home_dir (), ".gmameui", "gamelist", NULL);
  
	handle = fopen (filename, mode);
	g_free (filename);

	return handle;
}

void
gamelist_init (void)
{
	memset (&game_list, 0, sizeof (GameList));

	gui_prefs.current_game = NULL;
}

void
gamelist_free (void)
{

	if (game_list.name)
		g_free (game_list.name);
	if (game_list.version)
		g_free (game_list.version);
GMAMEUI_DEBUG ("Freeing roms");
	if (game_list.roms)
	{
		g_list_foreach (game_list.roms, (GFunc) rom_entry_free, NULL);
		g_list_free (game_list.roms);
	}
GMAMEUI_DEBUG ("Freeing roms... done");
	if (game_list.years)
	{
		g_list_foreach (game_list.years, (GFunc) g_free, NULL);
		g_list_free (game_list.years);
	}
	if (game_list.manufacturers)
	{
		g_list_foreach (game_list.manufacturers, (GFunc) g_free, NULL);
		g_list_free (game_list.manufacturers);
	}
	if (game_list.drivers)
	{
		g_list_foreach (game_list.drivers, (GFunc) g_free, NULL);
		g_list_free (game_list.drivers);
	}

	if (game_list.not_checked_list);
		g_list_free (game_list.not_checked_list);

	gamelist_init ();
}

#define FIELDS_PER_RECORD 27 + (NB_CPU * 4)

/**
* Adds a rom entry to the gamelist.
*
*/
void
gamelist_add (RomEntry *rom)
{
	gchar **manufacturer_fields;
	int i;

	if (!rom->romname) {
		GMAMEUI_DEBUG ("Broken parser/gamelist loader. Romname was NULL.");
	}

	/*generate glist for manufacturers*/
	manufacturer_fields = rom_entry_get_manufacturers (rom);
	if (manufacturer_fields) {

		for (i = 0; i < 2; i++) {
			if (manufacturer_fields[i]) {
				glist_insert_unique (&game_list.manufacturers, manufacturer_fields[i]);
			}
		}				
		g_strfreev (manufacturer_fields);
	}
	
	if (!rom->cloneof) {
		rom->cloneof = g_strdup ("-");
	}

	if (!rom->sampleof) {
		rom->sampleof = g_strdup ("-");
	}

	if (!rom->romof) {
		rom->romof = g_strdup ("-");
	}

	if (rom->cloneof[0] == '-') {

		/* original. Point to romname */
		rom->clonesort = rom->romname;

	} else {
		rom->clonesort = g_strdup_printf ("%s-%s",
			rom->cloneof , rom->romname);
	}

	if (!rom->year)
		rom_entry_set_year (rom, _("Unknown"));

	game_list.roms = g_list_insert_sorted (game_list.roms, (gpointer) rom, (GCompareFunc )compare_game_name);

	game_list.num_games++;

	if  (rom->nb_samples > 0)
		game_list.num_sample_games++;
}

/**
* Loads the gamelist from the file.
*
* After calling this you must also call.
*
* load_games_ini ();
* load_catver_ini ();
* quick_check ();
* create_filterslist_content ();
* create_gamelist_content ();
*/
gboolean
gamelist_load (void)
{
	FILE *gamelist;
	gint romindex, j;
	gchar line[LINE_BUF];
	gchar **tmp_array;
	gchar *tmp, *p;
	RomEntry *rom;
	gboolean exe_version_checked = FALSE;
	gint offset;
	int i;
	int supported_games = 0;	

	romindex = 0;
	g_message (_("Loading gamelist"));


	gamelist_free ();

	game_list.version = NULL;
	game_list.name = NULL;

	gamelist = gamelist_open ("r");

	if (!gamelist) {
		game_list.version = g_strdup ("unknown");
		return FALSE;
	}


	while (fgets (line, LINE_BUF, gamelist)) {
		p = line;
		tmp = line;

		/* Skip comments */
		if (*tmp != '#') {
			while (*tmp && (*tmp != '\n')) {
				tmp++;
			}
			*tmp = '\0';

			tmp_array = g_strsplit (p, SEP, FIELDS_PER_RECORD);

			/* Check if the record is corrupted */
			for (i=0; i < FIELDS_PER_RECORD; i++) {
				if (!tmp_array[i]) {
					g_strfreev (tmp_array);
					fclose (gamelist);
					game_list.version = g_strdup ("unknown");
					gmameui_message (ERROR, NULL, _("Game list is corrupted."));	
					return FALSE;
				}
			}

			rom = rom_entry_new ();

			if (!rom || !tmp_array)
			{
				g_strfreev (tmp_array);
				fclose (gamelist);
				game_list.version = g_strdup ("unknown");
				gmameui_message (ERROR, NULL, _("Out of memory while loading gamelist"));
				return FALSE;
			}
			
			g_strlcpy (rom->romname, tmp_array[0], MAX_ROMNAME);
			rom->gamename = g_strdup (tmp_array[1]);
			rom->gamenameext = g_strdup (tmp_array[2]);
			rom->the_trailer = !strcmp (tmp_array[3], "true");
			rom->is_bios = !strcmp (tmp_array[4], "true");
			rom_entry_set_year (rom, tmp_array[5]);
			rom->manu = g_strdup (tmp_array[6]);
			rom->cloneof = g_strdup (tmp_array[7]);
			rom->romof = g_strdup (tmp_array[8]);
			rom_entry_set_driver (rom, tmp_array[9]);
			
			rom->status = atoi (tmp_array[10]);
			rom->driver_status_color = atoi (tmp_array[11]);
			rom->driver_status_sound = atoi (tmp_array[12]);
			rom->driver_status_graphic = atoi (tmp_array[13]);
			rom->colors = atoi (tmp_array[14]);

			/* offset of cpu infos in the array */
			offset = 15;
			for (j = 0; j < NB_CPU; j++)
			{
				if (!strncmp (tmp_array[ (j * 2) + offset], "(sound)", 7)) {
					p = tmp_array[ (j * 2) + offset];
					p += 7;
					rom->cpu_info[j].name = g_strdup (p);
					rom->cpu_info[j].sound_flag = TRUE;
				}
				else
				{
					rom->cpu_info[j].name = g_strdup (tmp_array[ (j * 2) + offset]);
					rom->cpu_info[j].sound_flag = FALSE;
				}
				rom->cpu_info[j].clock = atoi (tmp_array[ (j * 2) + offset + 1]);
			}

			/* calculate offset of sound cpu infos in the array */
			offset = 15 + (NB_CPU * 2);

			for (j = 0; j < NB_CPU; j++)
			{
				if (strcmp (tmp_array[offset + (j * 2)], "")) {
					rom->sound_info[j].name = g_strdup (tmp_array[offset + (j * 2)]);
				}
				rom->sound_info[j].clock = atoi (tmp_array[offset + (j * 2) + 1]);
			}

			offset = 15 + (NB_CPU * 4);

			rom->num_players = atoi (tmp_array[offset + 0]);
			rom->num_buttons = atoi (tmp_array[offset + 1]);
			rom->control = atoi (tmp_array[offset + 2]);

			rom->vector = !strcmp (tmp_array[offset + 3], "true");
		
			rom->screen_x = atoi (tmp_array[offset + 4]);
			rom->screen_y = atoi (tmp_array[offset + 5]);
			rom->screen_freq = atoi (tmp_array[offset + 6]);
			rom->horizontal = (*tmp_array[offset + 7] == 'h');

			rom->channels = atoi (tmp_array[offset + 8]);

			rom->nb_roms = atoi (tmp_array[offset + 9]);
			rom->nb_samples = atoi (tmp_array[offset + 10]);
			rom->sampleof = g_strdup (tmp_array[offset + 11]);

			g_strfreev (tmp_array);

			gamelist_add (rom);
			supported_games++;

		} else if (!exe_version_checked) {
			/* Check for the GMAMEUI version in the top comment line */
			while (*tmp && (*tmp != '\n')) {
				tmp++;
			}
			*tmp = '\0';
			tmp_array = g_strsplit (p, " ", 3);

			GMAMEUI_DEBUG ("Checking version of gamelist file: %s - %s", tmp_array[1], tmp_array[2]);

			if (strcmp (tmp_array[1], "GMAMEUI") || !tmp_array[2]) {
				game_list.version = g_strdup ("unknown");
				g_strfreev (tmp_array);
				fclose (gamelist);
				return FALSE;
			}
			if (g_ascii_strtod (tmp_array[2], NULL) < 0.91)
			{
				game_list.version = g_strdup ("too old");
				g_strfreev (tmp_array);
				fclose (gamelist);
				return FALSE;
			}
			if (g_ascii_strtod (tmp_array[2], NULL) > 0.91)
			{
				game_list.version = g_strdup ("unknown");
				g_strfreev (tmp_array);
				fclose (gamelist);
				return FALSE;
			}
			exe_version_checked = TRUE;
			GMAMEUI_DEBUG ("Checking version of gamelist file - OK");
			g_strfreev (tmp_array);

		}else {
			while (*tmp && (*tmp != '\n')) {
				tmp++;
			}
			*tmp = '\0';
			p += 2; /* Skip # */

			if (!strncmp (p, "Version", 7))
			{
				p += 8;
				game_list.version = g_strdup (p);
			} if (!strncmp (p, "Name", 4))
			{
				p += 5;
				game_list.name = g_strdup (p);
			}
		}
	}
	fclose (gamelist);

	GMAMEUI_DEBUG ("List for %s %s", game_list.name, game_list.version);
	g_message (_("Loaded %d roms by %d manufacturers covering %d years."), game_list.num_games,
				g_list_length (game_list.manufacturers),g_list_length (game_list.years));
	g_message (_("with %d games supporting samples."), game_list.num_sample_games);

	return (TRUE);
}

/**
* Prints the gamelist "prefix" to the file.
* Must be called once before printing any rom entries.
*/
static void
gamelist_prefix_print (FILE *handle)
{
	fprintf (handle,
		"# GMAMEUI 0.91\n"
		"# Name %s\n"
		"# Version %s\n"
		"# list of xmame games for GMAMEUI front-end\n"
		"# The fileformat is: "
		"romname"
		"gamename"
		"gamenameext"
		"the_trailer"
		"is_bios"
		"year"
		"manufacturer"
		"cloneof"
		"romof"
		"driver"
		"driverstatus"
		"drivercolorstatus"
		"driversoundstatus"
		"drivergraphicstatus"
		"colors"
		"cpu1"
		"cpu1_clock"
		"cpu2"
		"cpu2_clock"
		"cpu3"
		"cpu3_clock"
		"cpu4"
		"cpu4_clock"
		"sound1"
		"sound1_clock"
		"sound2"
		"sound2_clock"
		"sound3"
		"sound3_clock"
		"sound4"
		"sound4_clock"
		"num_players"
		"num_buttons"
		"control"
		"vector"
		"screen_x"
		"screen_y"
		"screen_freq"
		"horizontal"
		"channels"
		"num_roms"
		"num_samples"
		"sampleof\n",
		game_list.name,
		game_list.version
	);
}

/**
* Appends a rom entry to the gamelist.
*/
static void
gamelist_print (FILE     *handle,
		RomEntry *rom)
{
	int i;
	char float_buf[FLOAT_BUF_SIZE];


	if (!rom)
		return;

	fprintf (handle,
		"%s" SEP	/* romname */
		"%s" SEP	/* gamename */
		"%s" SEP	/* gamenameext */
		"%s" SEP	/* the trailer */
		"%s" SEP	/* is_bios */
		"%s" SEP	/* year */
		"%s" SEP	/* manu */
		"%s" SEP	/* clone of */
		"%s" SEP	/* rom of */
		"%s" SEP	/* driver */
		"%d" SEP	/* status */
		"%d" SEP	/* driver color status */
		"%d" SEP	/* driver sound status */
		"%d" SEP	/* driver sound graphic */
		"%i" SEP	/* colors */
		,
		rom->romname,
		rom->gamename,
		rom->gamenameext,
		rom->the_trailer ? "true" : "false",
		rom->is_bios ? "true" : "false",
		rom->year,
		rom->manu,
		rom->cloneof,
		rom->romof,
		rom->driver,
		rom->status,
		rom->driver_status_color,
		rom->driver_status_sound,
		rom->driver_status_graphic,
		rom->colors
	);

	for (i=0; i < NB_CPU; i++) {
		fprintf (handle, "%s" SEP "%i" SEP, 
			rom->cpu_info[i].name, rom->cpu_info[i].clock);
	}
	for (i=0; i < NB_CPU; i++) {
		fprintf (handle, "%s" SEP "%i" SEP, 
			rom->sound_info[i].name, rom->sound_info[i].clock);
	}

	fprintf (handle,
		"%i" SEP	/* players */
		"%i" SEP	/* buttons */
		"%d" SEP	/* control */
		"%s" SEP	/* vector */
		"%i" SEP	/* screen X */
		"%i" SEP	/* screen Y */
		"%s" SEP	/* screen frequency */
		"%s" SEP	/* orientation */
		"%i" SEP	/* channels */
		"%i" SEP	/* roms */
		"%i" SEP	/* samples */
		"\n",
		rom->num_players,
		rom->num_buttons,
		rom->control,
		rom->vector ? "true" : "false",
		rom->screen_x,
		rom->screen_y,
		my_dtostr (float_buf, rom->screen_freq),
		rom->horizontal ? "horizontal" : "vertical",
		rom->channels,
		rom->nb_roms,
		rom->nb_samples
	);

}

/**
* Saves the gamelist.
*/
gboolean
gamelist_save (void)
{
	GList *listpointer;
	FILE *gamelist;

	g_message (_("Saving gamelist."));
	gamelist = gamelist_open ("w");

	if (!gamelist)
		return FALSE;

	gamelist_prefix_print (gamelist);

	listpointer = g_list_first (game_list.roms);

	while (listpointer) {
		gamelist_print (gamelist, (RomEntry*)listpointer->data);
		listpointer = g_list_next (listpointer);
	}

	fclose (gamelist);

	return TRUE;
}

/**
* Checks the gamelist and if its needs to be build/rebuild
* it asks the user and proceeds accordingly.
*
* Checks:
* - Gamelist was read but it was a version we don't support.
* - Gamelist is not available.
* - Gamelist was read but it was created with a very old version of gmameui.
* - Gamelist does not match the current executable (and VersionCheck = TRUE)
*/
void
gamelist_check (XmameExecutable *exec)
{

	GtkWidget *dialog = NULL;

	gint result;

	if (!exec)
		return;

	if (!game_list.version || !strcmp (game_list.version,"unknown")) {
		dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_YES_NO,
							_("Could not recognise the gamelist version.\n"
							  "Do you want to rebuild the gamelist?"));

	} else if (!strcmp (game_list.version,"none")) {

		dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_YES_NO,
							_("Gamelist not available,\n"
 							  "Do you want to build the gamelist?"));

	} else if (!strcmp (game_list.version,"too old")) {

		dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_YES_NO,
							_("Gamelist was created with an older version of GMAMEUI.\n"
							  "The gamelist is not supported.\n"
							  "Do you want to rebuild the gamelist?"));

	} else if (gui_prefs.VersionCheck) {
		
		if (strcmp (exec->name, game_list.name) ||
			strcmp (exec->version, game_list.version))
		{

			dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_YES_NO,
							_("The gamelist is from:\n"
							  "%s %s\n"
							  "and the current executable is:\n"
							  "%s %s\n"
							  "Do you want to rebuild the gamelist?"),
							  game_list.name,
							  game_list.version,
							  exec->name,
							  exec->version);

		}
	}

	if (dialog) {
		result = gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		switch (result)
		{
			case GTK_RESPONSE_YES:
					gtk_widget_set_sensitive (main_gui.scrolled_window_games, FALSE);
					UPDATE_GUI;

				save_games_ini ();

				if (gamelist_parse (exec)) {
					gamelist_save ();
					load_games_ini ();
					load_catver_ini ();
					quick_check ();
					create_gamelist_content ();
					
				}

				gtk_widget_set_sensitive (main_gui.scrolled_window_games, TRUE);
				break;
		}
		
	}		
}

RomEntry *
gamelist_get_selected_game (void)
{
	RomEntry *game_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *select;

	game_data = NULL;

	if (!main_gui.displayed_list)
		return NULL;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));

	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, ROMENTRY, &game_data, -1);
	}

	return game_data;
}

RomEntry* get_rom_from_gamelist_by_name (gchar *romname) {
	GList *listpointer;
	RomEntry *tmprom = NULL;
	
	for (listpointer = g_list_first (game_list.roms);
	     (listpointer != NULL);
	     listpointer = g_list_next (listpointer))
	{
		tmprom = (RomEntry *) listpointer->data;
		if (!strcmp (tmprom->romname, romname))
		{
			break;
		}
	}

	return tmprom;
}
