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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtkmain.h>
#include <gtk/gtkscrolledwindow.h>

#include "gmameui.h"
#include "io.h"
#include "gui.h"

#ifndef XMAME_ROOT
#define XMAME_ROOT "/usr/lib/games/xmame"
#endif

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
gboolean g_key_file_save_to_file(GKeyFile *kf, const gchar *file, GError **error)
{
	gsize length, nr;
	char *data = g_key_file_to_data(kf, &length, error);
	GIOChannel *gio;

	if (!data)
		return FALSE;

	gio = g_io_channel_new_file(file, "w+", error);
	if (!gio) {
		g_free(data);
		return FALSE;
	}
	
	g_io_channel_write_chars(gio, data, length, &nr, error);

	g_free(data);

	g_io_channel_unref(gio);

	return TRUE;
}

gboolean
load_games_ini (void)
{
	gchar *filename;
	RomEntry *tmprom = NULL;
		
	GMAMEUI_DEBUG ("Loading games.ini");

	filename = g_build_filename (g_get_home_dir (), ".gmameui", "games.ini", NULL);
	if (!filename)
		return FALSE;

	GKeyFile *gameini_list = g_key_file_new ();
	GError *error = NULL;
	gboolean result = g_key_file_load_from_file (gameini_list, filename, G_KEY_FILE_KEEP_COMMENTS, &error);
	g_free (filename);
	if (!result) {
		GMAMEUI_DEBUG ("Error loading games.ini file: %s", error->message);
		g_error_free (error);
		g_key_file_free (gameini_list);

		game_list.not_checked_list = g_list_copy (game_list.roms);

		return FALSE;
	}

	/* ADB FIXME Run through the list of available roms, and look for
	the key values; if the first cannot be found, assume the rest cannot be either */
	gchar **gamelist = g_key_file_get_groups (gameini_list, NULL);
	int i;
	for (i = 0; gamelist[i] != NULL; i++) {
		tmprom = get_rom_from_gamelist_by_name (gamelist[i]);
		if (tmprom) {
			tmprom->timesplayed = g_key_file_get_integer (gameini_list, gamelist[i], "PlayCount", &error);
			if (!tmprom->timesplayed) tmprom->timesplayed = 0;	/* Set default */
			tmprom->has_roms = g_key_file_get_integer (gameini_list, gamelist[i], "HasRoms", &error);
			tmprom->has_samples = g_key_file_get_integer (gameini_list, gamelist[i], "HasSamples", &error);
			tmprom->favourite = g_key_file_get_integer (gameini_list, gamelist[i], "Favorite", &error);
			if (!tmprom->favourite) tmprom->favourite = 0;	/* Set default */
			/* TODO AutofireDelay */
#ifdef QUICK_CHECK_ENABLED
			if (((tmprom->has_roms == UNKNOWN) ||
			     (tmprom->has_roms == NOT_AVAIL) ||
			     (tmprom->has_roms == INCORRECT)) && gui_prefs.GameCheck)
			{
				game_list.not_checked_list = g_list_append (game_list.not_checked_list, tmprom);
			}
#endif
		}
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
	RomEntry *rom;
	GList *list_pointer;
	
	
	GMAMEUI_DEBUG ("Saving games.ini");

	filename = g_build_filename (g_get_home_dir (), ".gmameui", "games.ini", NULL);
	game_ini_file = fopen (filename, "w");
	g_free (filename);
	if (!game_ini_file)
	{
		GMAMEUI_DEBUG ("unable to write games.ini");
		return FALSE;
	}
		
	for (list_pointer = g_list_first (game_list.roms);
	     (list_pointer != NULL);
	     list_pointer=g_list_next (list_pointer))
	{
		rom = (RomEntry *)list_pointer->data;
		fprintf (game_ini_file, "[%s]\n", rom->romname);
		fprintf (game_ini_file, "PlayCount=%i\n", rom->timesplayed);
		fprintf (game_ini_file, "HasRoms=%i\n", rom->has_roms);
		fprintf (game_ini_file, "HasSamples=%i\n", rom->has_samples);
		fprintf (game_ini_file, "Favorite=%s\n", rom->favourite ? "1" : "0");
		fputs ("\r\n", game_ini_file);
	}
	fclose (game_ini_file);
	
	return TRUE;
}


gboolean
load_catver_ini (void)
{
	gchar *filename;
	GList *listpointer;
	RomEntry *tmprom = NULL;
	gchar *category;
	gchar *version;
	GError *error = NULL;
	
	GMAMEUI_DEBUG ("Loading catver.ini");

	GTimer *timer = g_timer_new();
	g_timer_start (timer);
	
	/* Initialize categories */
	if (!game_list.categories) {
		g_list_foreach (game_list.categories, (GFunc) g_free, NULL);
		g_list_free (game_list.categories);
	}
	/* Initialize versions */
	if (!game_list.versions) {
		g_list_foreach (game_list.versions, (GFunc) g_free, NULL);
		g_list_free (game_list.versions);
	}

	category = g_strdup (_("Unknown"));
	game_list.categories = g_list_append (NULL, category);

	version = g_strdup (_("Unknown"));
	game_list.versions = g_list_append (NULL, version);

	/* Set all roms to unknown */
	for (listpointer = g_list_first (game_list.roms); (listpointer != NULL);
		listpointer = g_list_next (listpointer))
	{
		tmprom = (RomEntry *) listpointer->data;
		tmprom->category = category;
		tmprom->mame_ver_added = version;
	}

	filename = g_build_filename (gui_prefs.catverDirectory, "catver.ini", NULL);	
	if (!filename)
		return FALSE;

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
	for (listpointer = g_list_first (game_list.roms); listpointer; listpointer = g_list_next (listpointer)) {
		tmprom = (RomEntry *)listpointer->data;
		/*GMAMEUI_DEBUG ("Parsing catver - %s", tmprom->romname);*/
		category = g_key_file_get_string (catver_file, "Category", tmprom->romname, &error);
		if (error) {
			/*GMAMEUI_DEBUG ("Error parsing catver - %s", error->message);*/
			error = NULL;
		}
		version = g_key_file_get_string (catver_file, "VerAdded", tmprom->romname, &error);
		if (error) {
			/*GMAMEUI_DEBUG ("Error parsing catver - %s", error->message);*/
			error = NULL;
		}
		tmprom->category = glist_insert_unique (&game_list.categories, category);
		tmprom->mame_ver_added = glist_insert_unique (&game_list.versions, version);

	}
	g_key_file_free (catver_file);
	g_free (category);
	g_free (version);
	
	GMAMEUI_DEBUG ("Catver loaded in %.2f seconds", g_timer_elapsed (timer, NULL));
	g_timer_stop (timer);
	g_timer_destroy (timer);

	return TRUE;
}

/* preferences for the gui */
gboolean
load_gmameui_ini (void)
{
	gchar *filename;
	gchar *default_game = NULL;
	gint i;
	gsize sizes, columnsize;
	
	GMAMEUI_DEBUG ("Loading gmameui.ini");

	gui_prefs.Splitters = g_new0 (gint, 2);
	gui_prefs.Splitters[0] = 150;
	gui_prefs.Splitters[1] = 800;

	gui_prefs.Joystick_in_GUI = g_strdup (get_joy_dev ());

	filename = g_build_filename (g_get_home_dir (), ".gmameui", "gmameui.ini", NULL);

	GKeyFile *gmameui_ini_file = g_key_file_new ();
	GError *error = NULL;
	gboolean result = g_key_file_load_from_file (gmameui_ini_file, filename, G_KEY_FILE_KEEP_COMMENTS, &error);

	if (!result) {
		GMAMEUI_DEBUG ("Error loading gmameui.ini: %s\n", error->message);
		g_error_free (error);
		g_free (filename);
		return FALSE;
	}

	default_game = g_key_file_get_string (gmameui_ini_file, "Default", "DefaultGame", &error);

	gui_prefs.Joystick_in_GUI = g_key_file_get_string (gmameui_ini_file, "Default", "Joystick_in_GUI", &error);
	
	gui_prefs.Splitters = g_key_file_get_integer_list (gmameui_ini_file, "Default", "Splitters", &sizes, &error);

	g_key_file_free (gmameui_ini_file);
	g_free (filename);
	
	if (error)
		g_error_free (error);
	g_free (default_game);

	return TRUE;
}



/* this is where directory paths are set (common with mame32k) */
gboolean
load_dirs_ini (void)
{
	gchar *filename;
	gchar *mame_executable = NULL;
	gchar **xmame_executables;
	gsize paths;	/* FIXME Define max number of rom/sample dirs */
	
	filename = g_build_filename (g_get_home_dir (), ".gmameui", "dirs.ini", NULL);	
GMAMEUI_DEBUG ("Loading directories ini file");
	GKeyFile *dirsini_list = g_key_file_new ();
	GError *error = NULL;
	gboolean result = g_key_file_load_from_file (dirsini_list, filename, G_KEY_FILE_KEEP_COMMENTS, &error);
	
	if (!result) {
		GMAMEUI_DEBUG ("Error loading %s - %s - setting default values", filename, error->message);	
	
		g_error_free (error);
	} else {
		mame_executable = g_key_file_get_string (dirsini_list, "Directories", "mame_executable", &error);
		xmame_executables = g_key_file_get_string_list (dirsini_list, "Directories", "xmame_executables_array", &paths, &error);

		int i;
		for (i = 0; i < paths; i++)
			xmame_table_add (xmame_executables[i]);
		if (mame_executable) {
			current_exec = xmame_table_get (mame_executable);
GMAMEUI_DEBUG("Current exec set to %s", current_exec->path);
			g_free (mame_executable);
		}
	
		if (!current_exec)
			current_exec = xmame_table_get_by_index (0);


		gui_prefs.catverDirectory = g_key_file_get_string (dirsini_list, "Directories", "CatverDirectory", &error);		
		
		g_key_file_free (dirsini_list);
GMAMEUI_DEBUG ("Finished loading directories ini file");
	}
		
	if (!gui_prefs.catverDirectory) gui_prefs.catverDirectory = g_build_filename (g_get_home_dir (), ".gmameui", NULL);

	
	g_free (filename);
	
	return TRUE;
}

gboolean
save_dirs_ini (void)
{
	gchar *filename;

GMAMEUI_DEBUG ("Saving dirs.ini");

	GKeyFile *dirsini_list = g_key_file_new ();

	if (current_exec)
		g_key_file_set_string (dirsini_list, "Directories", "mame_executable", current_exec->path);

	if (xmame_table_size () > 0)
		g_key_file_set_string_list (dirsini_list, "Directories", "xmame_executables_array",
					    xmame_table_get_all(), xmame_table_size());
	
	g_key_file_set_string (dirsini_list, "Directories", "CatverDirectory", gui_prefs.catverDirectory);
	
	filename = g_build_filename (g_get_home_dir (), ".gmameui", "dirs.ini", NULL);	
	g_key_file_save_to_file (dirsini_list, filename, NULL);
	g_free (filename);
	
	g_key_file_free (dirsini_list);

GMAMEUI_DEBUG ("Saving dirs.ini... done");
	
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
	static gboolean quick_check_running = FALSE;
	GList *list_pointer;
	gint nb_rom_not_checked;
	gfloat done;
	RomEntry *rom;
#ifdef QUICK_CHECK_ENABLED
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
	for (list_pointer = game_list.not_checked_list;list_pointer ;list_pointer = g_list_next (list_pointer))
	{
		rom = (RomEntry *)list_pointer->data;

		/* Check for the existence of the ROM; if the ROM is a clone, the
		   parent set must exist as well */
		if (strcmp (rom->cloneof, "-"))
			if ((check_rom_exists_as_file (rom->romname)) &&
			    (check_rom_exists_as_file (rom->cloneof)))
				rom->has_roms = UNKNOWN;
			else
				rom->has_roms = NOT_AVAIL;
		else if (check_rom_exists_as_file (rom->romname))
			rom->has_roms = UNKNOWN;
		else
			rom->has_roms = NOT_AVAIL;


		/* Looking for samples */
		rom->has_samples = 0;

		if (rom->nb_samples > 0)
		{
			/* Check for the existence of the samples; if the samples is a clone, the
			   parent set must exist as well */
			if (strcmp (rom->sampleof, "-")) {
				if ((check_rom_exists_as_file (rom->romname)) &&
				    (check_rom_exists_as_file (rom->sampleof)))
					rom->has_samples = CORRECT;
			} else if (check_rom_exists_as_file (rom->romname))
				rom->has_samples = CORRECT;
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
			update_game_in_list (rom);
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
