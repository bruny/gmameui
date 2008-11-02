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
#include "mame_options.h"
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
			xmame_table_add (gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog)));    FIXME TODO DELETE
			add_exec_menu ();
		}
		gtk_widget_destroy (GTK_WIDGET (dialog));*/
		
	} 

	/* only need to do a quick check and redisplay games if the not_checked_list is not empty */
#ifdef QUICK_CHECK_ENABLED
	if (game_list.not_checked_list) {
GMAMEUI_DEBUG ("Processing not checked list");
		quick_check ();
		create_gamelist_content ();
	}
#endif
	GMAMEUI_DEBUG ("init done, starting main loop");
	gtk_main ();
	return 0;
}

void
gmameui_init (void)
{
	gchar *filename;
#ifdef ENABLE_JOYSTICK
	gboolean usejoyingui;
#endif
	
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

	GMAMEUI_DEBUG (_("Initialising list of possible MAME executable options"));
	xmame_options_init ();
	
	

	GMAMEUI_DEBUG (_("Initialising MAME executables"));
	main_gui.exec_list = mame_exec_list_new ();
	
	if (!current_exec)
		GMAMEUI_DEBUG ("No executable!");

	/* Load GUI preferences */
	main_gui.gui_prefs = mame_gui_prefs_new ();

	/* Set the MAME executable list */
	GValueArray *va_exec_paths;
	gchar *mame_executable = NULL;
	guint i;
	g_object_get (main_gui.gui_prefs,
		      "executable-paths", &va_exec_paths,
		      "current-executable", &mame_executable,
		      NULL);
	for (i = 0; i < va_exec_paths->n_values; i++) {
		GMAMEUI_DEBUG (_("Adding executable from preferences file: %s"),
			       g_value_get_string (g_value_array_get_nth (va_exec_paths, i)));
		
		XmameExecutable *exec = xmame_executable_new (g_value_get_string (g_value_array_get_nth (va_exec_paths, i)));
		xmame_executable_set_version (exec);    /* To get the name and version */
		mame_exec_list_add (main_gui.exec_list, exec);
	}
	g_value_array_free (va_exec_paths);
	
	if (mame_executable) {
		current_exec = mame_exec_list_get_exec_by_path (main_gui.exec_list, mame_executable);
		GMAMEUI_DEBUG (_("Current exec set to %s"), current_exec->path);
		g_free (mame_executable);
	} else
		current_exec = mame_exec_list_nth (main_gui.exec_list, 0);	

#ifdef ENABLE_DEBUG
g_message (_("Time to initialise: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif

	/* Create a new audit object */
	gui_prefs.audit = gmameui_audit_new ();
	
	/* Initialise the current game */
	gui_prefs.current_game = NULL;
	
	gui_prefs.gl = mame_gamelist_new ();
	if (!mame_gamelist_load (gui_prefs.gl)) {
		g_message (_("gamelist not found, need to rebuild one"));
	} else {

#ifdef ENABLE_DEBUG
g_message (_("Time to load gamelist: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif
		if (!load_games_ini ())
			g_message (_("games.ini not loaded, using default values"));
#ifdef ENABLE_DEBUG
g_message (_("Time to load games ini: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif
		if (!load_catver_ini ())
			g_message (_("catver not loaded, using default values"));
	}
	
	if (!load_options (NULL))
		g_message (_("default options not loaded, using default values"));

#ifdef ENABLE_DEBUG
	g_timer_stop (mytimer);
	g_message (_("Time to initialise GMAMEUI: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
	g_timer_destroy (mytimer);
#endif

#ifdef ENABLE_JOYSTICK
	gchar *joystick_device;
	
	g_object_get (main_gui.gui_prefs,
		      "usejoyingui", &usejoyingui,
		      "joystick-name", &joystick_device,
		      NULL);
	if (usejoyingui) {
		joydata = joystick_new (joystick_device);

		if (joydata)
			g_message (_("Joystick %s found"), joydata->device_name);
		else
			g_message (_("No Joystick found"));
	}
	
	g_free (joystick_device);
#endif
	/* doesn't matter if joystick is enabled or not but easier to handle after */
	joy_focus_on ();

}

gboolean
game_filtered (RomEntry * rom)
{
	gchar **manufacturer;
	
	gboolean is;
	Columns_type type;
	gchar *value;
	gint int_value;
	
	gboolean retval;

	g_return_val_if_fail (selected_filter != NULL, FALSE);
	
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
		retval = ( (is && !g_strcasecmp (rom->driver,value)) ||
			 (!is && g_strcasecmp (rom->driver,value)));
		break;
	case CLONE:
		retval = ( (is && !g_strcasecmp (rom->cloneof,value)) ||
			 (!is && g_strcasecmp (rom->cloneof,value)));
		break;
	case CONTROL:
		retval = ( (is && (rom->control == (ControlType)int_value))  ||
			 (!is && ! (rom->control == (ControlType)int_value)));
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
		retval = ( (is && rom->status == (DriverStatus)int_value) ||
			 (!is && !rom->status == (DriverStatus)int_value));
		break;
	case COLOR_STATUS:
		retval = ( (is && (rom->driver_status_color == (DriverStatus)int_value))  ||
			 (!is && ! (rom->driver_status_color == (DriverStatus)int_value)));
		break;
	case SOUND_STATUS:
		retval = ( (is && (rom->driver_status_sound == (DriverStatus)int_value))  ||
			 (!is && ! (rom->driver_status_sound == (DriverStatus)int_value)));
		break;
	case GRAPHIC_STATUS:
		retval = ( (is && (rom->driver_status_graphic == (DriverStatus)int_value))  ||
			 (!is && ! (rom->driver_status_graphic == (DriverStatus)int_value)));
		break;
	case HAS_ROMS:
		retval = ( (is && (rom->has_roms == (RomStatus)int_value))  ||
			 (!is && ! (rom->has_roms == (RomStatus)int_value)));
		break;
	case HAS_SAMPLES:
		retval = ( (is && (rom->nb_samples == int_value))  ||
			 (!is && ! (rom->nb_samples == int_value)));
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
	GList *extra_output = NULL;
	
	gboolean error_rom, error_mame;	 /* Error with the load */
	gboolean warning_rom;		 /* Warning with the load */
	
	ProgressWindow *progress_window;
#ifdef ENABLE_JOYSTICK
	gboolean usejoyingui;
	
	joystick_close (joydata);
	joydata = NULL;
#endif
	
	/* FIXME Progress of loading ROMs is not reported in newer versions of MAME
	   (e.g. SDLMame), so there is no way of updating a progress window */
	progress_window = progress_window_new (TRUE);
	progress_window_set_title (progress_window, _("Loading %s:"), rom_entry_get_list_name (rom));
	progress_window_show (progress_window);

	gtk_widget_hide (MainWindow);

	/* need to use printf otherwise, with GMAMEUI_DEBUG, we dont see the complete command line */
	GMAMEUI_DEBUG ("Message: running command %s\n", options);
	xmame_pipe = popen (options, "r");
	GMAMEUI_DEBUG (_("Loading %s:"), rom->gamename);

	/* Loading */
	
	/* XMAME loads the ROM first and then prepares the display. We watch the
	   pipe during the ROM loading until we reach "done.", which is XMAME's way
	   of indicating the ROM has loaded. Errors with particular ROMs are not
	   displayed until after "done.", which is why we have two clauses looking
	   for NOT FOUND below */
	
	/* The following are examples of output lines from MAME:
	 sv02.bin NOT FOUND (SDLMAME)
	 WARNING: the game might not run correctly. (SDLMAME)
	 mcu.bin      NOT FOUND (NO GOOD DUMP KNOWN) (XMAME)
	 s92-23c      NOT FOUND (XMAME)
	 ERROR: required files are missing, the game cannot be run.
	 
	 SDL found mode:720x450x32 (XMAME - SDL)
	 
	 XMAME generates the following output which can be parsed:
	 loading rom 0: epr11282.a4
	 loading rom 1: epr11280.a1
	 info: sysdep_mixer: using oss plugin
	 OSD: Info: Ideal mode for this game = 384x225
	 
	 SVGAlib: Info: Found videomode 640x400x32
	 svgalib: Failed to initialize mouse
	*/
	
	error_rom = error_mame = FALSE;
	while (fgets (line, BUFFER_SIZE, xmame_pipe)) {
		/* remove the last \n */
		for (p = line; (*p && (*p != '\n')); p++);
		*p = '\0';

		GMAMEUI_DEBUG ("xmame: %s", line);

		if (!strncmp (line,"loading", 7)) {
			/* Only works with XMAME */
			/* xmame: loading rom 16: s92_18.bin */
			
			nb_loaded++;
			/*search for the : */
			for (p = line; (*p && (*p != ':')); p++);
			p = p + 2;
			for (p2 = p; (*p2 && (*p2 != '\n')); p2++);
			p2 = '\0';

			done = (gfloat) (nb_loaded / rom->nb_roms);

			progress_window_set_value (progress_window, done);
			progress_window_set_text (progress_window, p);

		} else if (!g_ascii_strncasecmp (line, "Ignoring", 8)) {
			/* Ignore any errors starting with 'Ignoring' */
			continue;
		} else if (g_str_has_suffix (line, "NOT FOUND")) {
			/* Catch errors relating to missing ROMs (SDLMAME catches them here) */
			error_rom = TRUE;
			extra_output = g_list_append (extra_output, g_strdup (line));
		} else if (g_str_has_suffix (line, "(NO GOOD DUMP KNOWN)")) {
			/* Catch errors relating to best available ROMs (SDLMAME catches them here) */
			warning_rom = TRUE;
		}

		/* XMAME writes "done." to the command line once the ROMs have been
		   loaded, but before they are checked and before the display is
		   prepared. SDLMAME doesn't do this */
		if (!strncmp (line, "done", 4))
			break;

		while (gtk_events_pending ()) gtk_main_iteration ();
	}

	progress_window_destroy (progress_window);
	while (gtk_events_pending ()) gtk_main_iteration ();

	/* Parse the output and check for errors */
	while (fgets (line, BUFFER_SIZE, xmame_pipe)) {
		for (p = line; (*p && (*p != '\n')); p++);
		*p = '\0';

		GMAMEUI_DEBUG ("xmame: %s", line);

		if (!strncmp (line, "X Error", 7) ||		/* X11 mode not found*/
		    !strncmp (line, "GLERROR", 7) ||		/* OpenGL initialization errors */
		    !strncmp (line, "SDL: Unsupported", 16) ||
		    !strncmp (line, "Unable to start", 15) ||
		    !strncmp (line, "svgalib: ", 9))		/* XMAME SVGA */
		{
			GMAMEUI_DEBUG (_("Error with MAME graphics creation: %s"), line);
			error_mame = TRUE;
			extra_output = g_list_append (extra_output, g_strdup (line));
		} else if (g_str_has_suffix (line, "NOT FOUND")) {
			/* Catch errors relating to missing ROMs (XMAME catches them here) */
			error_rom = TRUE;
			extra_output = g_list_append (extra_output, g_strdup (line));
		} else if (g_str_has_suffix (line, "(NO GOOD DUMP KNOWN)")) {
			/* Catch errors relating to best available ROMs (XMAME catches them here) */
			warning_rom = TRUE;
		}
	}
	
	extra_output = glist_remove_duplicates (extra_output);
	
	pclose (xmame_pipe);
	if (error_rom || error_mame) {
		/* There was an error during the load. Create a dialog to present the errors */
		GtkWidget *dialog;
		gchar *title;
		gchar *secmessage;
		
		if (error_rom) {
			title = g_strdup (_("GMAMEUI could not load the ROM"));
			secmessage = g_strdup (_("The following ROMs were not found:\n"));
		} else if (error_mame) {
			title = g_strdup (_("MAME did not start successfully"));
			secmessage = g_strdup (_("The following errors were reported:\n"));
		}
			
		GMAMEUI_DEBUG ("error during load");
		
		GList *node;
		for (node = g_list_first (extra_output); node; node = node->next) {
			secmessage = g_strconcat (secmessage, node->data, "\n", NULL);
		}
		dialog = gmameui_dialog_create (ERROR, NULL, title);
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
							  secmessage);

		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		g_free (title);
		g_free (secmessage);

		/* Update game information if there was a ROM problem */
		if (error_rom) rom->has_roms = INCORRECT;
		
	} else {
		/* Game was successfully loaded */
		GMAMEUI_DEBUG ("game over");

		/* Update game informations */
		/* FIXME TODO Set g_object rom info, which triggers signal to update game in list.
		   This will then replace update_game_in_list call below */
		rom->timesplayed++;

		/* Update game information if there was a ROM warning, otherwise set to correct */
		warning_rom ? rom->has_roms = BEST_AVAIL : CORRECT;
	}
	
	g_list_foreach (extra_output, (GFunc)g_free, NULL);
	g_list_free (extra_output);

	gtk_widget_show (MainWindow);
	
	/* Update the ROM with the times played or status if there was any error */
	update_game_in_list (rom);

#ifdef ENABLE_JOYSTICK
	gchar *joystick_device;
	
	g_object_get (main_gui.gui_prefs,
		      "usejoyingui", &usejoyingui,
		      "joystick-name", &joystick_device,
		      NULL);
	if (usejoyingui)
		joydata = joystick_new (joystick_device);
	
	g_free (joystick_device);
#endif	
}

/* Prepare the commandline to use to play a game */
void
play_game (RomEntry *rom)
{
	gchar *current_rom_name;
	gchar *opt;
	gchar *general_options;
	gchar *Vector_Related_options;
	GameOptions *target;
	gboolean use_xmame_options;

	g_object_get (main_gui.gui_prefs,
		      "current-rom", &current_rom_name,
		      "usexmameoptions", &use_xmame_options,
		      NULL);
	
//	g_return_if_fail (current_rom_name == NULL);
	g_return_if_fail (current_exec != NULL);
	
	if (use_xmame_options) {
		GMAMEUI_DEBUG ("Using MAME options, ignoring GMAMEUI-specified options");
		opt = g_strdup_printf ("%s %s 2>&1", current_exec->path, rom->romname);
		launch_emulation (rom, opt);
		g_free (opt);
		return;
	}

	if (current_exec->type == XMAME_EXEC_WIN32) {
		gchar *sdlmame_options_string_playback;
		gchar *sdlmame_options_string_perf;
		gchar *sdlmame_options_string_video;
		gchar *sdlmame_options_string_opengl;
		gchar *sdlmame_options_string_sound;
		gchar *sdlmame_options_string_display;
		gchar *sdlmame_options_string_misc;
		gchar *sdlmame_options_string_debug;
		gchar *sdlmame_options_string_artwork;
		gchar *sdlmame_options_string_input;
		gchar *sdlmame_options_string_vector;
		
		sdlmame_options_string_playback = mame_options_get_option_string (main_gui.options, "Playback");
		sdlmame_options_string_perf = mame_options_get_option_string (main_gui.options, "Performance");
		sdlmame_options_string_video = mame_options_get_option_string (main_gui.options, "Video");
		sdlmame_options_string_opengl = mame_options_get_option_string (main_gui.options, "OpenGL");
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
		
		opt = g_strdup_printf ("%s %s %s %s %s %s %s %s %s %s %s %s %s %s -%s %s 2>&1",
				       current_exec->path,
				       create_rompath_options_string (current_exec),
				       create_io_options_string (current_exec),
				       sdlmame_options_string_playback,
				       sdlmame_options_string_perf,
				       sdlmame_options_string_video,
				       sdlmame_options_string_opengl,
				       sdlmame_options_string_sound,
				       sdlmame_options_string_display,
				       sdlmame_options_string_vector,
				       sdlmame_options_string_misc,
				       sdlmame_options_string_debug,
				       sdlmame_options_string_artwork,
				       sdlmame_options_string_input,
				       current_exec->noloadconfig_option,
				       rom->romname);
		
		g_free (sdlmame_options_string_playback);
		g_free (sdlmame_options_string_perf);
		g_free (sdlmame_options_string_video);
		g_free (sdlmame_options_string_opengl);
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
	gchar *opt;
	gchar *general_options;
	gchar *vector_options;
	GameOptions *target;
	
	// 0 = playback; 1 = record
	
	if (action == 0) {
		/* test if the inp file is readable */
		GMAMEUI_DEBUG ("play selected: {%s}",file);
		/* nedd to do a test on the unescaped string here otherwise doesn't even find the file  */
		if (g_file_test (file, G_FILE_TEST_EXISTS) == FALSE) {	
			gmameui_message (ERROR, NULL, _("Could not open '%s' as valid input file"), file);
			return;
		}
	}
	
	filename = g_path_get_basename (file);
	
	if (action == 0)
		GMAMEUI_DEBUG ("Playback game %s", file);
	else
		GMAMEUI_DEBUG ("Record game %s", file);

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
	
	opt = g_strdup_printf ("%s %s %s ",
			       current_exec->path,
			       general_options,
			       vector_options);
	
	/* create the command */
	if (action == 0) {
		gchar **splitname;
		
		splitname = g_strsplit (g_path_get_basename (filename), ".", 0);
		opt = g_strconcat (opt, "-playback ",
				   filename, " ",
				   "-", current_exec->noloadconfig_option, " ",
				   splitname[0], " 2>&1", NULL);
		
		g_strfreev (splitname);
	} else {
		gchar *romname;
		g_object_get (main_gui.gui_prefs, "current-rom", &romname, NULL);
		opt = g_strconcat (opt, "-record ",
				   filename, " ",
				   "-", current_exec->noloadconfig_option, " ",
				   romname, " 2>&1", NULL);
		g_free (romname);
	}

	if (target != &default_options)
		game_options_free (target);
	/* FIXME Playing back on xmame requires hitting enter to continue
	   (run command from command line) */
	launch_emulation (rom, opt);

	/* Free options */
	g_free (filename);
	g_free (general_options);
	g_free (vector_options);
	g_free (opt);
}

void
exit_gmameui (void)
{
	g_message (_("Exiting GMAMEUI..."));

	save_games_ini ();

	save_options (NULL, NULL);

	joystick_close (joydata);
	joydata = NULL;

	g_object_unref (gui_prefs.gl);
	gui_prefs.gl = NULL;
	
	g_object_unref (gui_prefs.audit);
	gui_prefs.audit = NULL;

	g_object_unref (main_gui.exec_list);
	main_gui.exec_list = NULL;
	xmame_options_free ();

	g_object_unref (main_gui.options);
	main_gui.options = NULL;
	
	g_object_unref (main_gui.filters_list);
	main_gui.filters_list = NULL;
	
	g_object_unref (main_gui.gui_prefs);
	main_gui.gui_prefs = NULL;
	
	/* FIXME TODO gtk_widget_destroy (MainWindow);*/
	
	g_message (_("Finished cleaning up GMAMEUI"));
	
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
