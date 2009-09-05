/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
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
#include <glib/gstdio.h>	/* For g_mkdir */
#include <gtk/gtkmain.h>
#include <gtk/gtkfilesel.h>

#include "gmameui.h"
#include "gui.h"
#include "io.h"
#include "mame_options.h"
#include "mame_options_legacy.h"
#include "options_string.h"
#include "progression_window.h"
#include "gtkjoy.h"
#include "mame-exec.h"
#include "directories.h"
#include "gmameui-statusbar.h"

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
	
	if (init_gui () == -1)
		return -1;

	/* Show a progress bar while the gamelist is being loaded */
	/* FIXME TODO These should be controlled via g_signal_emit
	   calls for loading the gamelist etc */
	gmameui_statusbar_start_pulse (main_gui.statusbar);
	gmameui_statusbar_set_progressbar_text (main_gui.statusbar,
	                                        _("Loading gamelist..."));
	
	if (!mame_gamelist_load (gui_prefs.gl)) {
		g_message (_("gamelist not found, need to rebuild one"));
		/* Loading the gamelist failed - prompt the user to recreate it */
		gamelist_check (mame_exec_list_get_current_executable (main_gui.exec_list));
	} else {
		//g_message (_("Time to load gamelist: %.02f seconds"), g_timer_elapsed (mytimer, NULL));

		gmameui_statusbar_set_progressbar_text (main_gui.statusbar,
	                                        _("Loading game data..."));
		if (!load_games_ini ())
			g_message (_("games.ini not loaded, using default values"));

		//g_message (_("Time to load games ini: %.02f seconds"), g_timer_elapsed (mytimer, NULL));

		if (!load_catver_ini ())
			g_message (_("catver not loaded, using default values"));
	}
	
	/* FIXME TODO These should be controlled via g_signal_emit
	   calls for loading the gamelist etc */
	gmameui_statusbar_stop_pulse (main_gui.statusbar);
	
	mame_gamelist_view_repopulate_contents (main_gui.displayed_list);
	mame_gamelist_view_scroll_to_selected_game (main_gui.displayed_list);
	
	/* Load the default options */
	main_gui.options = mame_options_new ();
	main_gui.legacy_options = mame_options_legacy_new ();
		
#ifdef ENABLE_SIGNAL_HANDLER
	signal (SIGHUP, gmameui_signal_handler);
	signal (SIGINT, gmameui_signal_handler);
#endif

	/* If no executables were found, prompt the user to open the directories
	   window to add at least one */
	if (mame_exec_list_size (main_gui.exec_list) == 0) {
		GtkWidget *dir_dlg, *msg_dlg;
		gint response;
		
		msg_dlg = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
						  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						  GTK_MESSAGE_QUESTION,
						  GTK_BUTTONS_YES_NO,
						  _("No MAME executables were found"));
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (msg_dlg),
							  _("Do you want to locate a MAME executable now?"));
		
		response = gtk_dialog_run (GTK_DIALOG (msg_dlg));
		gtk_widget_destroy (msg_dlg);
		
		if (response == GTK_RESPONSE_YES) {
			dir_dlg = mame_directories_dialog_new (GTK_WINDOW (MainWindow));
			gtk_widget_show (dir_dlg);
		}
		
	} 

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

	/* FIXME TODO Move to mame_options_init () */
	filename = g_build_filename (g_get_user_config_dir (), "gmameui", "options", NULL);
	if (!g_file_test (filename, G_FILE_TEST_IS_DIR)) {
		GMAMEUI_DEBUG ("no options directory creating one");
		g_mkdir (filename, S_IRWXU);
	}
	g_free (filename);

	/* init globals */
	memset (Status_Icons, 0, sizeof (GdkPixbuf *) * NUMBER_STATUS);

	GMAMEUI_DEBUG (_("Initialising list of possible MAME executable options"));
	xmame_options_init ();
	
	GMAMEUI_DEBUG (_("Initialising MAME executables"));
	main_gui.exec_list = mame_exec_list_new ();
	
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
		
		MameExec *exec = mame_exec_new_from_path (g_value_get_string (g_value_array_get_nth (va_exec_paths, i)));
		mame_exec_list_add (main_gui.exec_list, exec);
		
		// FIXME TODO Unref the exec
	}
	g_value_array_free (va_exec_paths);
	
	if (mame_executable) {
		GMAMEUI_DEBUG ("Adding executable that was specified previously as %s", mame_executable);
		mame_exec_list_set_current_executable (main_gui.exec_list,
						       mame_exec_list_get_exec_by_path (main_gui.exec_list, mame_executable));
		g_free (mame_executable);
	} else if (mame_exec_list_size (main_gui.exec_list) > 0) {
		/* Only set a default executable if there are available ones to choose from */
		GMAMEUI_DEBUG ("Adding default executable in position 0");
		mame_exec_list_set_current_executable (main_gui.exec_list,
						       mame_exec_list_nth (main_gui.exec_list, 0));
	}

#ifdef ENABLE_DEBUG
g_message (_("Time to initialise: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif

	/* Create a new audit object */
	gui_prefs.audit = gmameui_audit_new ();
	
	/* Initialise the gamelist */
	gui_prefs.gl = mame_gamelist_new ();
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
game_filtered (MameRomEntry * rom, gint rom_filter_opt)
{
	/* gchar **manufacturer; */
	
	gboolean is;
	Columns_type type;
	gchar *value;
	gint int_value;
	gboolean retval;

	/* ROM information */
	gboolean is_bios;
	gboolean is_favourite;
	gboolean is_clone;
	gboolean is_vector;
	
	g_return_val_if_fail (selected_filter != NULL, FALSE);
	g_return_val_if_fail (rom != NULL, FALSE);
	
	retval = FALSE;

	g_object_get (selected_filter,
		      "is", &is,
		      "type", &type,
		      "value", &value,
		      "int_value", &int_value,
		      NULL);

	is_bios = mame_rom_entry_is_bios (rom);

	/* Only display a BIOS rom if the BIOS filter is explicitly stated */
	if (is_bios) { 
		if (type == IS_BIOS) {
			retval = ( (is && is_bios) ||
				 (!is && !is_bios));
		} else
			retval = FALSE;
	} else {
		switch (type) {
			
			gchar *driver, *ver_added, *category;
			DriverStatus driver_status;
			DriverStatus driver_status_colour;
			DriverStatus driver_status_sound;
			DriverStatus driver_status_graphics;
			gint timesplayed, num_channels;
			RomStatus rom_status;
			ControlType control;
			
			case DRIVER:
				g_object_get (rom, "driver", &driver, NULL);
				retval = ((is && !g_strcasecmp (driver,value)) ||
					 (!is && g_strcasecmp (driver,value)));
				g_free (driver);
				break;
			case CLONE:
				is_clone = mame_rom_entry_is_clone (rom);
				retval = ((is && !is_clone) || (!is && is_clone));
				break;
			case CONTROL:
				g_object_get (rom, "control-type", &control, NULL);
				retval = ((is && (control == (ControlType) int_value))  ||
					 (!is && !(control == (ControlType) int_value)));
				break;
			case MAMEVER:				
				g_object_get (rom, "version-added", &ver_added, NULL);
				if (ver_added)
					retval = (g_ascii_strcasecmp (ver_added, value) == 0);
				g_free (ver_added);
				break;
			case CATEGORY:
				g_object_get (rom, "category", &category, NULL);
				if (category)
					retval = (g_ascii_strcasecmp (category, value) == 0);
				g_free (category);
				break;
			case FAVORITE:
				is_favourite = mame_rom_entry_is_favourite (rom);
				retval = ( (is && is_favourite) ||
					 (!is && !is_favourite));
				break;
			case VECTOR:
				is_vector = mame_rom_entry_is_vector (rom);
				retval = ( (is && is_vector) ||
					 (!is && !is_vector));
				break;
			case DRIVER_STATUS:
				/* This is a summary of imperfect colour, sound, graphic
				   and emulation */
				g_object_get (rom, "driver-status", &driver_status, NULL);
				retval = ( (is && driver_status == (DriverStatus)int_value) ||
					 (!is && !driver_status == (DriverStatus)int_value));
				break;
			case COLOR_STATUS:
				g_object_get (rom, "driver-status-colour", &driver_status_colour, NULL);
				retval = ( (is && (driver_status_colour == (DriverStatus)int_value))  ||
					 (!is && ! (driver_status_colour == (DriverStatus)int_value)));
				break;
			case SOUND_STATUS:
				g_object_get (rom, "driver-status-sound", &driver_status_sound, NULL);
				retval = ( (is && (driver_status_sound == (DriverStatus)int_value))  ||
					 (!is && ! (driver_status_sound == (DriverStatus)int_value)));
				break;
			case GRAPHIC_STATUS:
				g_object_get (rom, "driver-status-graphics", &driver_status_graphics, NULL);
				retval = ( (is && (driver_status_graphics == (DriverStatus)int_value))  ||
					 (!is && ! (driver_status_graphics == (DriverStatus)int_value)));
				break;
			case HAS_ROMS:
				rom_status = mame_rom_entry_get_rom_status (rom);
				retval = ((is && (rom_status == (RomStatus) int_value))  ||
					 (!is && !(rom_status == (RomStatus) int_value)));
				break;
			case HAS_SAMPLES:
				retval = ( (is && (mame_rom_entry_has_samples (rom) == int_value))  ||
					 (!is && ! (mame_rom_entry_has_samples (rom) == int_value)));
				break;
			case TIMESPLAYED:
				g_object_get (rom, "times-played", &timesplayed, NULL);
				retval = ( (is && (timesplayed == int_value)) ||
					 (!is && ! (timesplayed == int_value)));
				break;
			case CHANNELS:
				g_object_get (rom, "num-channels", &num_channels, NULL);
				retval = ( (is && (num_channels == int_value)) ||
					 (!is && (num_channels != int_value)));
				break;
			/* We are not currently supporting the YEAR and MANUFACTURER filters
			   since it makes the LHS filter list too long 
		
			case YEAR:
				retval = ( (is && (rom->year == value)) ||
					 (!is && (rom->year != value)));
				break;
				* comparing parsed text and text *
			case MANU:
				manufacturer = rom_entry_get_manufacturers (rom);
				* we have now one or two clean manufacturer (s) we still need to differentiates sub companies*
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
				break;*/
			default:
				GMAMEUI_DEBUG ("Trying to filter, but filter type %d is not handled", type);
				retval = FALSE;
			}
	}
	g_free (value);
	
	/* Final additional check whether the ROM should be displayed based on the ROM
	   filter settings and whether the ROM is available or unavailable */
	if (retval) {
		if (rom_filter_opt == 1) {
			/* Only show Available */
			retval = (mame_rom_entry_get_rom_status (rom) != NOT_AVAIL) ? TRUE : FALSE;
		} else if (rom_filter_opt == 2) {
			/* Only show Unavailable */
			retval = (mame_rom_entry_get_rom_status (rom) == NOT_AVAIL) ? TRUE : FALSE;
		} else {
		/* No need to process for All ROMs */

		}
	}

	return retval;
}

/* launch following the commandline prepared by play_game, playback_game and record_game 
   then test if the game is launched, detect error and update game status */
void
launch_emulation (MameRomEntry    *rom,
		  const gchar *options)
{
	FILE *xmame_pipe;
	gchar line [BUFFER_SIZE];
	gchar *p, *p2;
	gfloat done = 0;
	gint nb_loaded = 0;
	gint num_roms;
	GList *extra_output = NULL;
	
	gboolean error_rom, error_mame;	 /* Error with the load */
	gboolean warning_rom;		 /* Warning with the load */
	
	ProgressWindow *progress_window;
#ifdef ENABLE_JOYSTICK
	gboolean usejoyingui;
	
	joystick_close (joydata);
	joydata = NULL;
#endif
	
	g_object_get (rom, "num-roms", &num_roms, NULL);
	
	/* FIXME Progress of loading ROMs is not reported in newer versions of MAME
	   (e.g. SDLMame), so there is no way of updating a progress window */
	progress_window = progress_window_new (TRUE);
	progress_window_set_title (progress_window, _("Loading %s:"), mame_rom_entry_get_list_name (rom));
	progress_window_show (progress_window);

	gtk_widget_hide (MainWindow);

	/* need to use printf otherwise, with GMAMEUI_DEBUG, we dont see the complete command line */
	GMAMEUI_DEBUG ("Message: running command %s\n", options);
	xmame_pipe = popen (options, "r");
	GMAMEUI_DEBUG (_("Loading %s:"), mame_rom_entry_get_gamename (rom));

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
	
	error_rom = error_mame = warning_rom = FALSE;
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
			
			done = (gfloat) (nb_loaded / num_roms);

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
		gchar *title = NULL;
		gchar *secmessage = NULL;
		
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
		
	}
	
	/* Update game information */
	mame_rom_entry_rom_played (rom, warning_rom, error_rom);
	
	g_list_foreach (extra_output, (GFunc)g_free, NULL);
	g_list_free (extra_output);

	gtk_widget_show (MainWindow);
	
	/* Update the ROM with the times played or status if there was any error */
	mame_gamelist_view_update_game_in_list (main_gui.displayed_list, rom);

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

gchar *mame_categories[] = {
	"Playback",
	"Performance",
	"Video",
	"Display",
	"OpenGL",
	"Artwork",
	"Sound",
	"Miscellaneous",
	"Debugging",
	"Input"
};
gchar *legacy_categories[] = {
	"Video",
	"Artwork",
	"OpenGL",
	"Input",
	"X11Input",
	"Sound",
	"File",
	"MAME"
};

static gchar*
generate_command_line_options_string (MameExec *exec, MameRomEntry *rom)
{
	gchar *cl_opts;
	gchar *tmp_str;
	guint i;
	
	g_return_val_if_fail (exec != NULL, NULL);
	
	cl_opts = g_strdup ("");
	tmp_str = g_strdup ("");
	
	if (mame_exec_get_exectype (exec) == XMAME_EXEC_WIN32) {
		/* For each of the MAME categories, append to the option string */
		for (i = 0; i < G_N_ELEMENTS (mame_categories); i++) {
			tmp_str = g_strconcat (tmp_str,
					       mame_options_get_option_string (main_gui.options,
									       mame_categories[i]),
					       " ", NULL);
		}

		/* Add Vector-specific string */
		if (mame_rom_entry_is_vector (rom))
			tmp_str = g_strconcat (tmp_str,
					       mame_options_get_option_string (main_gui.options,
									       "Vector"),
					       " ", NULL);

	} else {
			
		/* For each of the MAME categories, append to the option string */
		for (i = 0; i < G_N_ELEMENTS (legacy_categories); i++) {
			tmp_str = g_strconcat (tmp_str,
					       mame_options_legacy_get_option_string (main_gui.legacy_options,
										      legacy_categories[i]),
					       " ", NULL);
		}

		/* Add Vector-specific string */
		if (mame_rom_entry_is_vector (rom))
			tmp_str = g_strconcat (tmp_str,
					       mame_options_legacy_get_option_string (main_gui.legacy_options,
										      "Vector"),
					       " ", NULL);
	}
	/* create the command */
	cl_opts = g_strdup_printf ("%s %s %s %s -%s",
				   mame_exec_get_path (exec),
				   create_rompath_options_string (exec),
				   create_io_options_string (exec),
				   tmp_str,
				   mame_exec_get_noloadconfig_option (exec));


	g_free (tmp_str);
	
	
	return cl_opts;
}

/* Prepare the commandline to use to play a game */
void
play_game (MameRomEntry *rom)
{
	MameExec *exec;
	gchar *current_rom_name;
	gchar *opt, *opts_string;
	gboolean use_xmame_options;
	

	g_object_get (main_gui.gui_prefs,
		      "current-rom", &current_rom_name,
		      "usexmameoptions", &use_xmame_options,
		      NULL);
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
//	g_return_if_fail (current_rom_name == NULL);
	g_return_if_fail (exec != NULL);
	
	if (use_xmame_options) {
		GMAMEUI_DEBUG ("Using MAME options, ignoring GMAMEUI-specified options");
		opt = g_strdup_printf ("%s %s 2>&1",
				       mame_exec_get_path (exec),
				       mame_rom_entry_get_romname (rom));
		launch_emulation (rom, opt);
		g_free (opt);
		return;
	}
	
	opts_string = generate_command_line_options_string (exec, rom);

	opt = g_strdup_printf ("%s %s 2>&1",
			       opts_string,
			       mame_rom_entry_get_romname (rom));
	launch_emulation (rom, opt);
	
	g_free (opts_string);
	g_free (opt);
}

void process_inp_function (MameRomEntry *rom, gchar *file, int action)
{
	MameExec *exec;
	char *filename;
	gchar *opt, *opts_string;

	g_return_if_fail (rom != NULL);	
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	g_return_if_fail (exec != NULL);
	
	// 0 = playback; 1 = record
	
	if (action == 0) {
		/* test if the inp file is readable */
		GMAMEUI_DEBUG ("Validating input file %s",file);
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

	opts_string = generate_command_line_options_string (exec, rom);

	/* create the command */
	if (action == 0) {
		gchar **splitname;
		
		splitname = g_strsplit (g_path_get_basename (filename), ".", 0);
		opt = g_strconcat (opts_string, " -playback ",
				   filename, " ",
				   splitname[0], " 2>&1", NULL);
		
		g_strfreev (splitname);
	} else {
		gchar *romname;
		g_object_get (main_gui.gui_prefs, "current-rom", &romname, NULL);
		opt = g_strconcat (opts_string, " -record ",
				   filename, " ",
				   romname, " 2>&1", NULL);
		g_free (romname);
	}

	/* FIXME Playing back on xmame requires hitting enter to continue
	   (run command from command line) */
	launch_emulation (rom, opt);

	/* Free options */
	g_free (filename);
	g_free (opts_string);
	g_free (opt);
}

void
exit_gmameui (void)
{
	g_message (_("Exiting GMAMEUI..."));

	save_games_ini ();

	joystick_close (joydata);
	joydata = NULL;

	/* Clear the gamelist (which clears all the romset GObjects) */
	g_object_unref (gui_prefs.gl);
	gui_prefs.gl = NULL;
	
	g_object_unref (gui_prefs.audit);
	gui_prefs.audit = NULL;

	g_object_unref (main_gui.exec_list);
	main_gui.exec_list = NULL;
	xmame_options_free ();
	
	/* Clear the gamelist view */
	g_object_unref (main_gui.displayed_list);
	main_gui.displayed_list = NULL;

	g_object_unref (main_gui.options);
	main_gui.options = NULL;
	
	g_object_unref (main_gui.filters_list);
	main_gui.filters_list = NULL;
	
	g_object_unref (main_gui.gui_prefs);
	main_gui.gui_prefs = NULL;

/*
	FIXME TODO
	g_object_unref (main_gui.statusbar);
	main_gui.statusbar = NULL;
*/
	
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
