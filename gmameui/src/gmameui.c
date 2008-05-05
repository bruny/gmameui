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

#include "common.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <signal.h>
#include <glib/gprintf.h>
#include <glib/gutils.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkfilesel.h>

#include "gmameui.h"
#include "gui.h"
#include "io.h"
#include "game_options.h"
#include "options_string.h"
#include "progression_window.h"
#include "gtkjoy.h"


#define BUFFER_SIZE 1000

static void
gmameui_init (void);

#ifdef ENABLE_SIGNAL_HANDLER
static void
gmameui_signal_handler (int signum)
{
	g_message ("Received signal %d. Quitting", signum);
	signal (signum, SIG_DFL);
	exit_gmameui ();
}
#endif

int
main (int argc, char *argv[])
{
#ifdef ENABLE_NLS
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (PACKAGE, "UTF-8");
#endif
	gtk_init (&argc, &argv);

	gmameui_init ();
	init_gui ();

	/* Load the default options */
	main_gui.options = mame_options_new ();
	
#ifdef ENABLE_SIGNAL_HANDLER
	signal (SIGHUP, gmameui_signal_handler);
	signal (SIGINT, gmameui_signal_handler);
#endif

	if (!current_exec) {
		gmameui_message (ERROR, NULL, _("No xmame executable found"));
		
		/* Open file browser to select MAME executable *
		GtkFileChooserDialog *dialog = gtk_file_chooser_dialog_new (_("Select a MAME executable"),
			NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);
		gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), FALSE);
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			xmame_table_add (gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog)));
			add_exec_menu ();
		}
		gtk_widget_destroy (GTK_WIDGET (dialog));*/
		
	} 

	/* only need to do a quick check and redisplay games if the not_checked_list is not empty */
	if (game_list.not_checked_list) {
		quick_check ();
		create_gamelist_content ();
	}

	GMAMEUI_DEBUG ("init done, starting main loop");
	gtk_main ();
	return 0;
}

/* ONLY USED FOR DEBUG */
void
column_debug	 (void)
{
#ifdef ENABLE_DEBUG
	gint i;
	printf ("i    : ");
	for (i = 0; i < NUMBER_COLUMN; i++) {
		printf ("%2d|", i);
	}
	printf ("\n");
	printf ("Shown: ");
	for (i = 0; i < NUMBER_COLUMN; i++) {
		printf ("%2d|", gui_prefs.ColumnShown[i]);
	}
	printf ("\n");

#endif
}

void
gmameui_init (void)
{
	gchar *filename;

#ifdef ENABLE_DEBUG
	GTimer *mytimer;

	mytimer = g_timer_new ();
	g_timer_start (mytimer);
#endif

	filename = g_build_filename (g_get_home_dir (), ".gmameui", NULL);
	if (!g_file_test (filename, G_FILE_TEST_IS_DIR)) {
		GMAMEUI_DEBUG ("no initial directory creating one");
		mkdir (filename, S_IRWXU);
	}
	g_free (filename);

	filename = g_build_filename (g_get_home_dir (), ".gmameui", "options", NULL);
	if (!g_file_test (filename, G_FILE_TEST_IS_DIR)) {
		GMAMEUI_DEBUG ("no options directory creating one");
		mkdir (filename, S_IRWXU);
	}
	g_free (filename);

	/* init globals */
	memset (Status_Icons, 0, sizeof (GdkPixbuf *) * NUMBER_STATUS);

	xmame_options_init ();
	xmame_table_init ();
	
	if (!current_exec)
		GMAMEUI_DEBUG ("No executable!");

	if (!load_dirs_ini ())
		g_message (_("dirs.ini not loaded, using default values"));
	
	gamelist_init ();
	if (!gamelist_load ()) {
		g_message (_("gamelist not found, need to rebuild one"));
	} else {
		if (!load_games_ini ())
			g_message (_("games.ini not loaded, using default values"));
		if (!load_catver_ini ())
			g_message (_("catver not loaded, using default values"));
	}

	if (!load_gmameui_ini ())
		g_message (_("unable to load gmameui.ini, using default values"));

	if (!load_options (NULL))
		g_message (_("default options not loaded, using default values"));

#ifdef ENABLE_DEBUG
	g_timer_stop (mytimer);
	g_message (_("Time to initialise GMAMEUI: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
	g_timer_destroy (mytimer);
#endif

#ifdef ENABLE_JOYSTICK
	if (gui_prefs.gui_joy) {
		joydata = joystick_new (gui_prefs.Joystick_in_GUI);

		if (joydata)
			g_message (_("Joystick %s found"), joydata->device_name);
		else
			g_message (_("No Joystick found"));
	}
#endif
	/* doesn't matter if joystick is enabled or not but easier to handle after */
	joy_focus_on ();

}


gboolean
game_filtered (RomEntry * rom)
{
	gchar **manufacturer;
/* comparing taxt and text */
	
	gboolean is;
	Columns_type type;
	gchar *value;
	gint int_value;
	
	gboolean retval;

	g_return_if_fail (selected_filter != NULL);
	
	retval = FALSE;
	
	g_object_get (selected_filter,
		      "is", &is,
		      "type", &type,
		      "value", &value,
		      "int_value", &int_value,
		      NULL);

	/* Only display a BIOS rom if the BIOS filter is explicitly stated */
	if (rom->is_bios) { 
		if (type == IS_BIOS) {
			retval = ( (is && rom->is_bios) ||
				 (!is && !rom->is_bios));
		} else
			retval = FALSE;
	} else {
	
	switch (type) {
	case DRIVER:
		retval = (g_ascii_strcasecmp (rom->driver, value) == 0);
		break;
	case CLONE:
		retval = ( (is && !g_strcasecmp (rom->cloneof,value)) ||
			 (!is && g_strcasecmp (rom->cloneof,value)));
		break;
	case CONTROL:
		retval = ( (is && !g_strcasecmp (rom->control,value)) ||
			 (!is && g_strcasecmp (rom->control,value)));
		break;
	case MAMEVER:
		if (rom->mame_ver_added)
			retval = (g_ascii_strcasecmp (rom->mame_ver_added, value) == 0);
		else
			retval = 0;
		break;
	case CATEGORY:
		if (rom->category)
			retval = (g_ascii_strcasecmp (rom->category, value) == 0);
		else
			retval = 0;
		break;
	case FAVORITE:
		retval = ( (is && rom->favourite) ||
			 (!is && !rom->favourite));
		break;
	case VECTOR:
		retval = ( (is && rom->vector) ||
			 (!is && !rom->vector));
		break;
	case STATUS:
		retval = ( (is && rom->status) ||
			 (!is && !rom->status));
		break;
	case COLOR_STATUS:
		retval = ( (is && !g_strcasecmp (rom->driver_status_color,value)) ||
			 (!is && g_strcasecmp (rom->driver_status_color,value)));
		break;
	case SOUND_STATUS:
		retval = ( (is && !g_strcasecmp (rom->driver_status_sound,value)) ||
			 (!is && g_strcasecmp (rom->driver_status_sound,value)));
		break;
	case GRAPHIC_STATUS:
		retval = ( (is && !g_strcasecmp (rom->driver_status_graphic,value)) ||
			 (!is && g_strcasecmp (rom->driver_status_graphic,value)));
		break;
		/* Comparing int and int */
	case HAS_ROMS:
		retval = ( (is && (rom->has_roms == (RomStatus)int_value))  ||
			 (!is && ! (rom->has_roms == (RomStatus)int_value)));
		break;
	case HAS_SAMPLES:
		retval = ( (is && (rom->has_samples == value))  ||
			 (!is && ! (rom->has_samples == value)));
		break;
	case TIMESPLAYED:
		retval = ( (is && (rom->timesplayed == int_value)) ||
			 (!is && ! (rom->timesplayed == int_value)));
		break;
	case CHANNELS:
		retval = ( (is && (rom->channels == int_value)) ||
			 (!is && (rom->channels != int_value)));
		break;
		/* Comparing text and int */
	case YEAR:
		retval = ( (is && (rom->year == value)) ||
			 (!is && (rom->year != value)));
		break;
		/* comparing parsed text and text */
	case MANU:
		manufacturer = rom_entry_get_manufacturers (rom);
		/* we have now one or two clean manufacturer (s) we still need to differentiates sub companies*/
		if (manufacturer[1] != NULL) {
			if ( (is && !g_strncasecmp (manufacturer[0], value, 5)) ||
			     (!is && g_strncasecmp (manufacturer[0], value, 5)) ||
			     (is && !g_strncasecmp (manufacturer[1], value, 5)) ||
			     (!is && g_strncasecmp (manufacturer[1], value, 5))
			     ) {
				g_strfreev (manufacturer);
				retval = TRUE;

			}
		} else {
			if ( (is && !g_strncasecmp (manufacturer[0], value, 5)) ||
			     (!is && g_strncasecmp (manufacturer[0], value, 5))
			     ) {
				g_strfreev (manufacturer);
				retval = TRUE;
			}
		}
		g_strfreev (manufacturer);
		break;
	default:
		GMAMEUI_DEBUG ("Trying to filter, but filter type %d is not handled", type);
		retval = FALSE;
	}
	}
	g_free (value);
	
	return retval;
}


/* launch following the commandline prepared by play_game, playback_game and record_game 
   then test if the game is launched, detect error and update game status */
void
launch_emulation (RomEntry    *rom,
		  const gchar *options)
{
	FILE *xmame_pipe;
	gchar line [BUFFER_SIZE];
	gchar *p, *p2;
	gfloat done = 0;
	gint nb_loaded = 0;
	GList *extra_output = NULL, *extra_output2 = NULL;
	gboolean error_during_load,other_error;
	ProgressWindow *progress_window;

	joystick_close (joydata);
	joydata = NULL;
	
	progress_window = progress_window_new (TRUE);

	progress_window_set_title (progress_window, _("Loading %s:"), rom_entry_get_list_name (rom));
	progress_window_show (progress_window);

	gtk_window_get_position (GTK_WINDOW (MainWindow), &gui_prefs.GUIPosX, &gui_prefs.GUIPosY);
	gtk_widget_hide (MainWindow);

	/* need to use printf otherwise, with GMAMEUI_DEBUG, we dont see the complete command line */
	GMAMEUI_DEBUG ("Message: running command %s\n", options);
	xmame_pipe = popen (options, "r");
	GMAMEUI_DEBUG (_("Loading %s:"), rom->gamename);

	/* Loading */
	error_during_load = other_error = FALSE;
	while (fgets (line, BUFFER_SIZE, xmame_pipe)) {
			/* remove the last \n */
		for (p = line; (*p && (*p != '\n')); p++);
		*p = '\0';

		GMAMEUI_DEBUG ("xmame: %s", line);

		if (!strncmp (line,"loading", 7)) {
			nb_loaded++;
			/*search for the : */
			for (p = line; (*p && (*p != ':')); p++);
			p = p + 2;
			for (p2 = p; (*p2 && (*p2 != '\n')); p2++);
			p2 = '\0';
			done = (gfloat) ( (gfloat) (nb_loaded) /
					  (gfloat) (rom->nb_roms));

			progress_window_set_value (progress_window, done);
			progress_window_set_text (progress_window, p);

		} else if (!strncmp (line, "Master Mode: Waiting", 20)) {
			for (p = line; *p != ':'; p++);
			p++;
			
			progress_window_set_text (progress_window, p);
		} else if (!g_ascii_strncasecmp (line, "error", 5) ||
			   !g_ascii_strncasecmp (line, "Can't bind socket", 17)) {
			/* the game didn't even began to load*/
			other_error = TRUE;
			extra_output = g_list_append (extra_output, g_strdup (line));
		}

		if (!strncmp (line, "done", 4))
			break;

		while (gtk_events_pending ()) gtk_main_iteration ();
	}

	progress_window_destroy (progress_window);
	while (gtk_events_pending ()) gtk_main_iteration ();

	/*check if errors */
	while (fgets (line, BUFFER_SIZE, xmame_pipe)) {
		for (p = line; (*p && (*p != '\n')); p++);
		*p = '\0';

		GMAMEUI_DEBUG ("xmame: %s", line);

		if (!strncmp (line, "GLmame", 6) || !strncmp (line, "based", 5))
			continue;
		else if (!g_ascii_strncasecmp (line, "error", 5)) {		/* bad rom or no rom found */ 
			error_during_load = TRUE;
			break;
		} else if (!strncmp (line, "GLERROR", 7)) {		/* OpenGL initialization errors */
			other_error = TRUE;
		}						/* another error occurred after game loaded */
		else if (!strncmp (line, "X Error", 7) ||		/* X11 mode not found*/
			 !strncmp (line, "SDL: Unsupported", 16) ||
			 !strncmp (line, "Unable to start", 15) ||
			 !strncmp (line, "Unspected X Error", 17)) {
			other_error = TRUE;
		}
		extra_output = g_list_append (extra_output, g_strdup (line));	
	}
	
	pclose (xmame_pipe);
	if (error_during_load || other_error) {
		int size;
		char **message = NULL;
		GMAMEUI_DEBUG ("error during load");
		size = g_list_length (extra_output) + 1;
		message = g_new (gchar *, size);
		size = 0;
		
		for (extra_output2 = g_list_first (extra_output); extra_output2;) {
			message [size++] = extra_output2->data;
			extra_output2 = g_list_next (extra_output2);
		}
		message[size] = NULL;

		gmameui_message (ERROR, NULL, g_strjoinv ("\n", message));
		g_strfreev (message);
		
		g_list_free (extra_output);
		
		/* update game informations if it was an rom problem */
		if (error_during_load)
			rom->has_roms = INCORRECT;
		
	} else {
		GMAMEUI_DEBUG ("game over");
		g_list_foreach (extra_output, (GFunc)g_free, NULL);
		g_list_free (extra_output);

		/* update game informations */
		rom->timesplayed++;
		rom->has_roms = CORRECT;
	}

	gtk_window_move (GTK_WINDOW (MainWindow),
			 gui_prefs.GUIPosX,
			 gui_prefs.GUIPosY);

	gtk_widget_show (MainWindow);
	/* update the gui for the times played and romstatus if there was any error */
	update_game_in_list (rom);
	select_game (rom);

#ifdef ENABLE_JOYSTICK
	if (gui_prefs.gui_joy)
		joydata = joystick_new (gui_prefs.Joystick_in_GUI);
#endif	
}

/* Prepare the commandline to use to play a game */
void
play_game (RomEntry *rom)
{
	gchar *opt;
	gchar *general_options;
	gchar *Vector_Related_options;
	GameOptions *target;

	if (!rom)
		return;

	if (!current_exec) {
		gmameui_message (ERROR, NULL, _("No xmame executables defined"));
		return;
	}

	if (gui_prefs.use_xmame_options) {
		opt = g_strdup_printf ("%s %s 2>&1", current_exec->path, rom->romname);
		launch_emulation (rom, opt);
		g_free (opt);
		return;
	}

	if (current_exec->type == XMAME_EXEC_WIN32) {
		gchar *sdlmame_options_string_perf;
		gchar *sdlmame_options_string_video;
		gchar *sdlmame_options_string_sound;
		gchar *sdlmame_options_string_display;
		gchar *sdlmame_options_string_misc;
		gchar *sdlmame_options_string_debug;
		gchar *sdlmame_options_string_artwork;
		gchar *sdlmame_options_string_input;
		gchar *sdlmame_options_string_vector;
		
		sdlmame_options_string_perf = mame_options_get_option_string (main_gui.options, "Performance");
		sdlmame_options_string_video = mame_options_get_option_string (main_gui.options, "Video");
		sdlmame_options_string_sound = mame_options_get_option_string (main_gui.options, "Sound");
		sdlmame_options_string_display = mame_options_get_option_string (main_gui.options, "Display");
		sdlmame_options_string_misc = mame_options_get_option_string (main_gui.options, "Miscellaneous");
		sdlmame_options_string_debug = mame_options_get_option_string (main_gui.options, "Debugging");
		sdlmame_options_string_artwork = mame_options_get_option_string (main_gui.options, "Artwork");
		sdlmame_options_string_input = mame_options_get_option_string (main_gui.options, "Input");
		if (rom->vector)
			sdlmame_options_string_vector = mame_options_get_option_string (main_gui.options, "Vector");
		else
			sdlmame_options_string_vector = g_strdup ("");
		
		opt = g_strdup_printf ("%s %s %s %s %s %s %s %s %s %s %s %s -%s %s 2>&1",
				       current_exec->path,
				       create_rompath_options_string (current_exec),
				       create_io_options_string (current_exec),
				       sdlmame_options_string_perf,
				       sdlmame_options_string_video,
				       sdlmame_options_string_sound,
				       sdlmame_options_string_display,
				       sdlmame_options_string_vector,
				       sdlmame_options_string_misc,
				       sdlmame_options_string_debug,
				       sdlmame_options_string_artwork,
				       sdlmame_options_string_input,
				       current_exec->noloadconfig_option,
				       rom->romname);
		
		g_free (sdlmame_options_string_perf);
		g_free (sdlmame_options_string_video);
		g_free (sdlmame_options_string_sound);
		g_free (sdlmame_options_string_display);
		g_free (sdlmame_options_string_misc);
		g_free (sdlmame_options_string_debug);
		g_free (sdlmame_options_string_artwork);
		g_free (sdlmame_options_string_input);
		g_free (sdlmame_options_string_vector);
	} else {
	
		target = load_options (rom);
		if (!target)
			target = &default_options;
	
		/* prepares options*/
		general_options = create_options_string (current_exec, target);
	
		if (rom->vector)
			Vector_Related_options = create_vector_options_string (current_exec, target);
		else
			Vector_Related_options = g_strdup ("");				
		/* create the command */
		opt = g_strdup_printf ("%s %s %s -%s %s 2>&1",
				       current_exec->path,
				       general_options,
				       Vector_Related_options,
				       current_exec->noloadconfig_option,
				       rom->romname);

		/*free options*/
		g_free (general_options);
		g_free (Vector_Related_options);
	
		if (target != &default_options)
			game_options_free (target);
	}
	launch_emulation (rom, opt);
	g_free (opt);
}

void process_inp_function (RomEntry *rom, gchar *file, int action) {
	char *filename;
	char *filepath;
	gchar *opt;
	gchar *general_options;
	gchar *vector_options;
	gchar *function;
	GameOptions *target;
	
	// 0 = playback; 1 = record
	
	if (action == 0) {
		/* test if the inp file is readable */
		GMAMEUI_DEBUG ("play selected: {%s}",file);
		/* nedd to do a test on the unescaped string here otherwise doesn't even find the file  */
		if (g_file_test (file, G_FILE_TEST_EXISTS) == FALSE) {	
	        /* if not print a message and return to the selection screen */
			gmameui_message (ERROR, NULL, _("Could not open '%s' as valid input file"), file);
			return;
		}
	}
	
	if (!current_exec) {
		gmameui_message (ERROR, NULL, _("No xmame executables defined"));
		/* Re-enable joystick */
		joy_focus_on ();
		return;
	}
	
	filename = g_path_get_basename (file);
	filepath = g_path_get_dirname (file);
	
	if (action == 0)
		GMAMEUI_DEBUG ("Playback game %s", file);
	else
		GMAMEUI_DEBUG ("Record game %s" G_DIR_SEPARATOR_S "%s", filepath, filename);

	/* prepares options*/
	target = load_options (rom);
	
	if (!target)
		target = &default_options;
	
	general_options = create_options_string (current_exec, target);
 	  	
 	if (rom->vector) {
 		vector_options = create_vector_options_string (current_exec, target);
	} else {
 		vector_options = g_strdup ("");
	}
	
	/* create the command */
	if (action == 0) {
		function = g_strdup ("playback");
	} else {
		function = g_strdup ("record");

	}
	
	opt = g_strdup_printf ("%s %s %s -input_directory %s -%s %s -%s %s 2>&1",
			       current_exec->path,
			       general_options,
			       vector_options,
			       filepath,
			       function,
			       filename,
			       current_exec->noloadconfig_option,
			       rom->romname);

	/* Free options */
	g_free (filename);
	g_free (filepath);
	g_free (function);

	g_free (general_options);
	g_free (vector_options);
	
	if (target != &default_options)
		game_options_free (target);
	/* FIXME Playing back on xmame requires hitting enter to continue
	   (run command from command line) */
	launch_emulation (rom, opt);
	g_free (opt);
	
	/* reenable joystick, was disabled in callback.c (on_playback_input_activate/on_play_and_record_input_activate)*/
	joy_focus_on ();
}

/* Prepare the commandline to use to playback a game */
void
playback_game (RomEntry *rom,
	       gchar    *file)
{
	process_inp_function (rom, file, 0);

}


/* Prepare the commandline to use to record a game */
void
record_game (RomEntry *rom,
	     gchar    *file)
{
	process_inp_function (rom, file, 1);
}


void
exit_gmameui (void)
{
	g_message (_("Exiting GMAMEUI..."));
	/* prevent to erase games preference if the game list was not loaded
	  (gamelist file corruption or rebuild of a list with a bogus executable)*/
	if (game_list.roms)
		save_games_ini ();
	save_gmameui_ini ();

	save_options (NULL, NULL);

	joystick_close (joydata);
	joydata = NULL;

	/* Not necessary but it's easier to track down memory leaks
	* if we free as much as we can
	*/
GMAMEUI_DEBUG ("Destroying window");
	//gtk_widget_destroy (MainWindow); ADB COMMENTING THIS OUT REMOVES COLUMN REORDERING AT END
GMAMEUI_DEBUG ("Destroying window - done");
	gamelist_free ();
	xmame_table_free ();
	xmame_options_free ();

	g_object_unref (main_gui.options);
	main_gui.options == NULL;
	
	gtk_main_quit ();
}

#if 0
GList *
get_columns_shown_list (void)
{
	GList *MyColumns;
	gint i;

	MyColumns = NULL;

	for (i = 0; i < NUMBER_COLUMN; i++) {

		if (gui_prefs.ColumnShown[i] == FALSE)
			continue;

		MyColumns = g_list_append (MyColumns, GINT_TO_POINTER (i));
	}
	return MyColumns;
}
#endif

const char *
column_title (int column_num)
{
	switch (column_num) {
	case GAMENAME:
		return _("Game");
	case HAS_ROMS:
		return _("ROMs");
	case HAS_SAMPLES:
		return _("Samples");
	case ROMNAME:
		return _("Directory");
	case VECTOR:
		return _("Type");
	case CONTROL:
		return _("Trackball");
	case TIMESPLAYED:
		return _("Played");
	case MANU:
		return _("Manufacturer");
	case YEAR:
		return _("Year");
	case CLONE:
		return _("Clone of");
	case DRIVER:
		return _("Driver");
	case STATUS:       /*  Available / Not Available */
		return _("Status");
	case ROMOF:
		return _("Rom of");
	case DRIVERSTATUS: /*  Working / Not Working */
		return _("Driver Status");
	case COLOR_STATUS:
		return _("Driver Colors");
	case SOUND_STATUS:
		return _("Driver Sound");
	case GRAPHIC_STATUS:
		return _("Driver Graphics");
	case NUMPLAYERS:
		return _("Players");
	case NUMBUTTONS:
		return _("Buttons");
	case CPU1:
		return _("CPU 1");
	case CPU2:
		return _("CPU 2");
	case CPU3:
		return _("CPU 3");
	case CPU4:
		return _("CPU 4");
	case SOUND1:
		return _("Sound 1");
	case SOUND2:
		return _("Sound 2");
	case SOUND3:
		return _("Sound 3");
	case SOUND4:
		return _("Sound 4");
	case MAMEVER:
		return _("Version");
	case CATEGORY:
		return _("Category");
	case FAVORITE:
		return _("Favorite");
	case CHANNELS:
		return _("Channels");
	case IS_BIOS:
		return _("BIOS");
	default:
		return NULL;
	}
}


const gchar *
rom_entry_get_list_name (RomEntry *rom)
{
	
	if (!rom->the_trailer) {
		if (!rom->name_in_list) {
			rom->name_in_list = g_strdup_printf ("%s %s", rom->gamename, rom->gamenameext);
		}
	} else  {

		if (gui_prefs.ModifyThe) {

			if (!rom->name_in_list || !strncmp (rom->name_in_list, "The", 3)) {
				g_free (rom->name_in_list);
				rom->name_in_list = g_strdup_printf ("%s, The %s", rom->gamename, rom->gamenameext);
			}

		} else {

			if (!rom->name_in_list || strncmp (rom->name_in_list, "The", 3)) {
				g_free (rom->name_in_list);
				rom->name_in_list = g_strdup_printf ("The %s %s", rom->gamename, rom->gamenameext);
			}
		}

	} 

	return rom->name_in_list;
}
