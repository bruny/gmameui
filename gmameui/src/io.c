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
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gmameui.h"
#include "io.h"
#include "gui.h"

/*
converts a float to a string with precission 5.
this is locale independent.
buf must be of size G_ASCII_DTOSTR_BUF_SIZE
*/
const gchar *
my_dtostr (char * buf, gdouble d)
{
	char *p;
	g_ascii_dtostr (buf, FLOAT_BUF_SIZE, d);

	for (p = buf; *p && *p != '.'; p++); /* point p to the decimal point */

	if (*p)
	{
		p += 6;	/* precision + 1 */
		*p = '\0';
	}
	return buf;
}

#define LINE_BUF 512

/* Custom function to save the keyfile to disk. If the file does not exist,
   it will be created */
gboolean g_key_file_save_to_file(GKeyFile *kf, const gchar *file)
{
	GError *error = NULL;
	gsize length, nr;
	
	char *data = g_key_file_to_data(kf, &length, &error);
	GIOChannel *gio;

	if (!data)
		return FALSE;

	gio = g_io_channel_new_file(file, "w+", &error);
	if (!gio) {
		GMAMEUI_DEBUG ("Could not save key file %s: %s", file, error->message);
		g_error_free (error);
		error = NULL;
		
		g_free(data);
		
		return FALSE;
	}
	
	g_io_channel_write_chars(gio, data, length, &nr, &error);

	g_free(data);

	g_io_channel_unref(gio);

	return TRUE;
}

gboolean
load_games_ini (void)
{
	gchar *filename;
	MameRomEntry *tmprom;
	GList *romlist;
		
	GMAMEUI_DEBUG ("Loading games.ini");
	
	//filename = g_build_filename (g_get_home_dir (), ".gmameui", "games.ini", NULL);
	filename = g_build_filename (g_get_user_config_dir (), "gmameui", "games.ini", NULL);
	if (!filename)
		return FALSE;
	
	romlist = mame_gamelist_get_roms_glist (gui_prefs.gl);
	
	GKeyFile *gameini_list = g_key_file_new ();
	GError *error = NULL;
	gboolean result = g_key_file_load_from_file (gameini_list, filename, G_KEY_FILE_KEEP_COMMENTS, &error);
	g_free (filename);
	
	if (!result) {
		GMAMEUI_DEBUG ("Error loading games.ini file: %s", error->message);
		g_error_free (error);
		g_key_file_free (gameini_list);
#ifdef QUICK_CHECK_ENABLED
		mame_gamelist_set_not_checked_list (gui_prefs.gl, romlist);
#endif
		return FALSE;
	}

	/* FIXME TODO Run through the list of available roms, and look for
	the key values; if the first cannot be found, assume the rest cannot be either */
	gchar **gamelist = g_key_file_get_groups (gameini_list, NULL);
	int i;
	
	for (i = 0; gamelist[i] != NULL; i++) {
		tmprom = get_rom_from_gamelist_by_name (gui_prefs.gl, gamelist[i]);
		
		if (tmprom) {
			gint timesplayed;
			RomStatus has_roms;
			RomStatus has_samples;
			gboolean is_favourite;
					
			timesplayed = g_key_file_get_integer (gameini_list, gamelist[i], "PlayCount", &error);
			has_roms = g_key_file_get_integer (gameini_list, gamelist[i], "HasRoms", &error);
			has_samples = g_key_file_get_integer (gameini_list, gamelist[i], "HasSamples", &error);
			is_favourite = g_key_file_get_integer (gameini_list, gamelist[i], "Favorite", &error);

			g_object_set (tmprom,
				      "times-played", timesplayed,
				      "has-roms", has_roms,
				      "has-samples", has_samples,
				      "is-favourite", is_favourite,
				      NULL);

#ifdef QUICK_CHECK_ENABLED
			if ((mame_rom_entry_get_rom_status (tmprom) == UNKNOWN || NOT_AVAIL || INCORRECT) &&
			    (gui_prefs.GameCheck))
			{
				game_list.not_checked_list = g_list_append (game_list.not_checked_list, tmprom);
			}
#endif
		} else {
			GMAMEUI_DEBUG ("Could not find ROM in list for %s", gamelist[i]);
		}

		/* Short term hack until we use g_io_watch () so the IO processing
		   doesn't block the main loop */
		UPDATE_GUI;
	}

	g_strfreev (gamelist);
	g_key_file_free (gameini_list);

	return TRUE;
}

/* FIXME Only update a game if it has changed */
gboolean
save_games_ini (void)
{
	FILE *game_ini_file;
	gchar *filename;
	MameRomEntry *rom;
	GList *romlist, *list_pointer;
	
	
	GMAMEUI_DEBUG ("Saving games.ini");

	//filename = g_build_filename (g_get_home_dir (), ".gmameui", "games.ini", NULL);
	filename = g_build_filename (g_get_user_config_dir (), "gmameui", "games.ini", NULL);
	game_ini_file = fopen (filename, "w");
	g_free (filename);
	if (!game_ini_file)
	{
		GMAMEUI_DEBUG ("unable to write games.ini");
		return FALSE;
	}
	
	romlist = mame_gamelist_get_roms_glist (gui_prefs.gl);
		
	for (list_pointer = g_list_first (romlist);
	     (list_pointer != NULL);
	     list_pointer=g_list_next (list_pointer))
	{
		gchar *romname;
		gint hasroms, hassamples, timesplayed;
		gboolean favourite;
		
		rom = (MameRomEntry *) list_pointer->data;
		
		g_object_get (rom,
			      "romname", &romname,
			      "has-roms", &hasroms,
			      "has-samples", &hassamples,
			      "times-played", &timesplayed,
			      "is-favourite", &favourite,
			      NULL);

		fprintf (game_ini_file, "[%s]\n", romname);
		fprintf (game_ini_file, "PlayCount=%i\n", timesplayed);
		fprintf (game_ini_file, "HasRoms=%i\n", hasroms);
		fprintf (game_ini_file, "HasSamples=%i\n", hassamples);
		fprintf (game_ini_file, "Favorite=%s\n", favourite ? "1" : "0");
		fputs ("\r\n", game_ini_file);
		
		g_free (romname);
	}
	fclose (game_ini_file);
	
	return TRUE;
}


gboolean
load_catver_ini (void)
{ 
	gchar *filename;
	GList *romlist, *listpointer;
	GList *catlist, *verlist;
	MameRomEntry *tmprom;
	gchar *category;
	gchar *version;
	GError *error = NULL;
	
	GMAMEUI_DEBUG ("Loading catver.ini");

	GTimer *timer = g_timer_new();
	g_timer_start (timer);
	
	romlist = mame_gamelist_get_roms_glist (gui_prefs.gl);
	catlist = mame_gamelist_get_categories_glist (gui_prefs.gl);
	verlist = mame_gamelist_get_versions_glist (gui_prefs.gl);
	
	/* Initialize categories */
	if (!catlist) {
		g_list_foreach (catlist, (GFunc) g_free, NULL);
		g_list_free (catlist);
	}
	
	/* Initialize versions */
	if (!verlist) {
		g_list_foreach (verlist, (GFunc) g_free, NULL);
		g_list_free (verlist);
	}

	category = g_strdup (_("Unknown"));
	catlist = g_list_append (NULL, category);

	version = g_strdup (_("Unknown"));
	verlist = g_list_append (NULL, version);
	
	g_object_get (main_gui.gui_prefs, "file-catver", &filename, NULL);

	g_return_val_if_fail (filename, FALSE);

	GKeyFile *catver_file = g_key_file_new ();
	g_key_file_load_from_file (catver_file, filename, G_KEY_FILE_KEEP_COMMENTS, &error);
	if (error) {
		GMAMEUI_DEBUG ("catver.ini could not be loaded: %s", error->message);
		g_error_free (error);
		g_free (filename);
		return FALSE;
	}
	g_free (filename);
	
	/* For each game in the list of available roms, get the category and
	   version from the ini file, and add to the list of known categories/versions
	   for use in the custom filters */
	for (listpointer = g_list_first (romlist); listpointer; listpointer = g_list_next (listpointer)) {
		const gchar *romname;

		tmprom = (MameRomEntry *) listpointer->data;
		romname = mame_rom_entry_get_romname (tmprom);

		category = g_key_file_get_string (catver_file, "Category", romname, &error);
		if (error) {
			/*GMAMEUI_DEBUG ("Error parsing catver - %s", error->message);*/
			g_error_free (error);
			error = NULL;
		}

		version = g_key_file_get_string (catver_file, "VerAdded", romname, &error);
		if (error) {
			/*GMAMEUI_DEBUG ("Error parsing catver - %s", error->message);*/
			g_error_free (error);
			error = NULL;
		}

		mame_gamelist_add_category (gui_prefs.gl, category);
		mame_gamelist_add_version (gui_prefs.gl, version);

		mame_rom_entry_set_category_version (tmprom, category, version);
	}
	
	g_key_file_free (catver_file);
	g_free (category);
	g_free (version);
	
	GMAMEUI_DEBUG ("Catver loaded in %.2f seconds", g_timer_elapsed (timer, NULL));
	g_timer_stop (timer);
	g_timer_destroy (timer);

	return TRUE;
}

#ifdef QUICK_CHECK_ENABLED
gboolean
check_rom_exists_as_file (gchar *romname) {
	gchar *filename, *dirname;
	gchar *zipname;
	gboolean retval;
	int i;
	
	retval = FALSE;

	zipname = g_strdup_printf("%s.zip", romname);
	
	for (i = 0; gui_prefs.RomPath[i] != NULL; i++) {
		filename = g_build_filename (gui_prefs.RomPath[i], zipname, NULL);
		dirname = g_build_filename (gui_prefs.RomPath[i], romname, NULL);
		
		if (g_file_test (filename, G_FILE_TEST_EXISTS) ||
		    g_file_test (dirname, G_FILE_TEST_IS_DIR)) {
			retval = TRUE;
	    		break;
		}
	}
	if (filename)
		g_free (filename);
	if (dirname)
		g_free (dirname);
	g_free (zipname);

	return retval;
}

/* TODO FIXME This function never gets called, but should use similar code
   to check_rom_exists_as_file; or combine the two */
gboolean
check_samples_exists_as_file (gchar *samplename) {
	gchar *filename;
	int i;
	
	filename = g_malloc (200);
	for (i=0;gui_prefs.SamplePath[i]!=NULL;i++) {
		g_snprintf (filename, 200, "%s" G_DIR_SEPARATOR_S "%s.zip", gui_prefs.SamplePath[i], samplename);
		if (g_file_test (filename, g_file_test (filename, G_FILE_TEST_EXISTS)))
			return TRUE;

		g_snprintf (filename, 200, "%s" G_DIR_SEPARATOR_S "%s", gui_prefs.SamplePath[i], samplename);
		if (g_file_test (filename, G_FILE_TEST_IS_DIR))
			return TRUE;

	}

	return FALSE;
}
#endif

void
quick_check (void)
{
#ifdef QUICK_CHECK_ENABLED
	static gboolean quick_check_running = FALSE;
	GList *list_pointer;
	gint nb_rom_not_checked;
	gfloat done;
	MameRomEntry *rom;

	GMAMEUI_DEBUG ("Running quick check.");
	if (game_list.num_games == 0)
		return;

	/*prevent user to launch several quick check at the same time */
	if (quick_check_running)
	{
		GMAMEUI_DEBUG ("Quick check already running");
		return;
	}
	quick_check_running = TRUE;
		
	show_progress_bar ();

	nb_rom_not_checked = g_list_length (game_list.not_checked_list);
	g_message (_("Performing quick check, please wait:"));
	/* Disable the callback */
	g_signal_handlers_block_by_func (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))),
					 (gpointer)adjustment_scrolled, NULL);

	/* need to use the last or I can find the list anymore*/
	for (list_pointer = game_list.not_checked_list;
	     list_pointer;
	     list_pointer = g_list_next (list_pointer))
	{
		rom = (MameRomEntry *)list_pointer->data;

		/* Check for the existence of the ROM; if the ROM is a clone, the
		   parent set must exist as well */
		if (mame_rom_entry_is_clone (rom))
			if ((check_rom_exists_as_file (mame_rom_entry_get_romname (rom))) &&
			    (check_rom_exists_as_file (mame_rom_entry_get_parent_romname (rom))))
			    g_object_set (rom, "has-roms", UNKNOWN, NULL);
			else
			    g_object_set (rom, "has-roms", NOT_AVAIL, NULL);

		else if (check_rom_exists_as_file (mame_rom_entry_get_romname (rom)))
			g_object_set (rom, "has-roms", UNKNOWN, NULL);
		else
			g_object_set (rom, "has-roms", NOT_AVAIL, NULL);


		/* Looking for samples */
		rom->has_samples = 0;

		if (mame_rom_entry_has_samples (rom))
		{
			/* Check for the existence of the samples; if the samples is a clone, the
			   parent set must exist as well */
			if (strcmp (rom->sampleof, "-")) {
				if ((check_rom_exists_as_file (mame_rom_entry_get_romname (rom))) &&
				    (check_rom_exists_as_file (rom->sampleof)))
					g_object_set (rom, "has-samples", CORRECT, NULL);
			} else if (check_rom_exists_as_file (mame_rom_entry_get_romname (rom)))
				g_object_set (rom, "has-samples", CORRECT, NULL);
		}
		
		list_pointer->data = NULL;
		nb_rom_not_checked--;

		done = (gfloat) (game_list.num_games - nb_rom_not_checked) / (gfloat)game_list.num_games;

#if 0
		if (done >= 0.0 && done <= 1.0)
			update_progress_bar (done);
#endif

		/* if the game is in the list, update it */
		if (rom->is_in_list)
			mame_gamelist_view_update_game_in_list (rom);
	}
	g_list_free(game_list.not_checked_list);
	game_list.not_checked_list = NULL;

	hide_progress_bar ();
	quick_check_running= FALSE;
	
	/* Re-Enable the callback */
	g_signal_handlers_unblock_by_func (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))),
					   (gpointer)adjustment_scrolled, NULL);
#endif
}

GList *
get_ctrlr_list (void)
{
	GList *ctrlr_list = NULL;
	GDir *di;
	const gchar *dent;
	gchar *filename;
	gsize filename_len;
	gchar *ctrlr_dir;
	
	g_object_get (main_gui.gui_prefs, "dir-ctrlr", &ctrlr_dir, NULL);
	
	GMAMEUI_DEBUG ("Getting the ctrlr list from directory %s", ctrlr_dir);

	di = g_dir_open (ctrlr_dir, 0, NULL);
	if (di)
	{
		while ( (dent = g_dir_read_name (di)))
		{
			filename = g_build_filename (ctrlr_dir, dent, NULL);
			if (g_file_test (filename, G_FILE_TEST_IS_DIR))
				ctrlr_list = g_list_append (ctrlr_list, g_strdup (dent));

			if (g_str_has_suffix (filename, ".cfg")) {
				filename_len = strlen (dent) - 4;
				ctrlr_list = g_list_append (ctrlr_list, g_strndup (dent, filename_len));
			}

			g_free (filename);
		}
		g_dir_close (di);
	}
	else
		GMAMEUI_DEBUG ("ERROR - unable to open folder %s", ctrlr_dir);

	g_free (ctrlr_dir);
	
	return ctrlr_list;
}
