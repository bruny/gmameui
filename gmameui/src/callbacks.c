/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2010 Andrew Burton <adb@iinet.net.au>
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
/*DELETE#include <gtk/gtkcolorseldialog.h>
#include <gtk/gtkfontsel.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmain.h>
#include <gtk/gtktreestore.h>

#ifdef ENABLE_LIBGNOME
#include <libgnome/libgnome.h>
#endif*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "callbacks.h"
#include "interface.h"
#include "directories.h"
#include "gmameui.h"
#include "audit.h"
#include "gmameui-audit-dlg.h"
#include "gui.h"
#include "progression_window.h"
#include "io.h"
#include "gui_prefs_dialog.h"
#include "mame_options_dialog.h"
#include "mame_options_legacy_dialog.h"
#include "gmameui-rominfo-dlg.h"
#include "gmameui-rommgr-dlg.h"

void update_favourites_list (gboolean add);

/* Main window menu: File */
void
on_play_activate (GtkAction *action,
		  gpointer  user_data)
{
	play_game (gui_prefs.current_game);
}

void
on_play_and_record_input_activate (GtkAction *action,
				   gpointer  user_data)
{
	select_inp (FALSE);
}


void
on_playback_input_activate (GtkAction *action,
			    gpointer  user_data)
{
	select_inp (TRUE);
}

void
on_select_random_game_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gint random_game;

	g_return_if_fail (visible_games > 0);

	random_game = (gint) g_random_int_range (0, visible_games);
	GMAMEUI_DEBUG ("random game#%i", random_game);

	mame_gamelist_view_select_random_game (main_gui.displayed_list, random_game);

}

void update_favourites_list (gboolean add) {
	g_object_set (gui_prefs.current_game, "is-favourite", add, NULL);

	gmameui_ui_set_favourites_sensitive (add);
	
	mame_gamelist_view_update_game_in_list (main_gui.displayed_list,
						gui_prefs.current_game);
}

void
on_add_to_favorites_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	update_favourites_list (TRUE);
}


void
on_remove_from_favorites_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	update_favourites_list (FALSE);
}

/* If rom_name is NULL, then the default options are used and loaded, otherwise
   the rom-specific options are used */
static void
show_properties_dialog (gchar *rom_name)
{
	MameExec *exec;
	GtkWidget *dlg;
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	g_return_if_fail (exec != NULL);
	
	/* SDLMAME uses a different set of options to XMAME. If we are running
	   XMAME, then use the legacy GXMAME method of maintaining the options */
	if (mame_exec_get_exectype (exec) == XMAME_EXEC_WIN32) {
		/* SDLMAME */	
		dlg = mame_options_dialog_new ();
	} else {		
		dlg = mame_options_legacy_dialog_new ();
	}

	gtk_widget_show (dlg);
}

void
on_romset_info_activate (GtkAction *action, gpointer user_data)
{
	GtkWidget *rominfo_dialog;
	rominfo_dialog = mame_rominfo_dialog_new ();
	gtk_widget_show (rominfo_dialog);
}

void
on_options_activate (GtkAction *action, gpointer user_data)
{
	gchar *current_rom;
	g_object_get (main_gui.gui_prefs, "current-rom", &current_rom, NULL);
	
	show_properties_dialog (current_rom);
	
	g_free (current_rom);
}

void
on_options_default_activate (GtkAction *action,
			     gpointer  user_data)
{
	show_properties_dialog (NULL);
}

void
on_audit_all_games_activate (GtkMenuItem     *menuitem,
			     gpointer         user_data)
{
	MameExec *exec;
	GtkWidget *audit_dlg;
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	if (!exec) {
		gmameui_message (ERROR, NULL, _("No xmame executables defined"));
		/* reenable joystick */
		joy_focus_on ();
		return;
	}

	gamelist_check (exec);

	audit_dlg = mame_audit_dialog_new (NULL);
	gtk_widget_show (audit_dlg);	
}

#ifdef ENABLE_ROMVALIDATION
void
on_rebuild_romsets_activate (GtkAction *action,
			     gpointer  user_data)
{
	GtkWidget *romsetsmgr_dlg;
	romsetsmgr_dlg = gmameui_rommgr_dialog_new (NULL);
	gtk_widget_show (romsetsmgr_dlg);
}
#endif

void
on_exit_activate (GtkMenuItem     *menuitem,
		  gpointer         user_data)
{
	exit_gmameui ();
}

void
on_refresh_activate (GtkAction *action,
		     gpointer  user_data)
{
	gmameui_gamelist_rebuild (main_gui.displayed_list);
}

void
on_directories_menu_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *directory_window;
	directory_window = mame_directories_dialog_new (NULL);
	gtk_widget_show (directory_window);
}

void
on_preferences_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	MameGuiPrefsDialog *prefs_dialog;
	prefs_dialog = mame_gui_prefs_dialog_new ();
GMAMEUI_DEBUG("Running dialog");
	gtk_dialog_run (GTK_DIALOG (prefs_dialog));
	gtk_widget_destroy (GTK_WIDGET (prefs_dialog));
GMAMEUI_DEBUG("Done running dialog");
}


/* Main window menu: Help */
void
on_help_activate                       (GtkAction     *action,
                                        gpointer       user_data)
{
	GError *error = NULL;
#ifdef ENABLE_GTKSHOWURI
	/* gtk_show_uri requires > GTK2.13 */
	guint timestamp;
	
	timestamp = gtk_get_current_event_time ();	
	gtk_show_uri (NULL, "ghelp:gmameui", timestamp, &error);
#elif ENABLE_LIBGNOME
	/* The following requires libgnome to be compiled in (which it is
	   by default) */
	gboolean ret;
	ret = gnome_url_show ("ghelp:gmameui", &error);
#else
	/* GTK2.13 is not available and the user has not compiled in
	   libgnome (may be a XFCE or KDE user) */
	GtkWidget *helpunavail_dlg;
	helpunavail_dlg = gmameui_dialog_create (GTK_MESSAGE_ERROR,
						 NULL,
						 _("GMAMEUI Help could not be started"));
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (helpunavail_dlg),
						  _("The Help file could not be opened. This will happen "
						   "if the version of GTK is not at least 2.13 and "
						   "libgnome support has not been compiled in."));
	gtk_dialog_run (GTK_DIALOG (helpunavail_dlg));
	gtk_widget_destroy (helpunavail_dlg);
#endif
	
	if (error) {
		GtkWidget *helperr_dlg;
		helperr_dlg = gmameui_dialog_create (GTK_MESSAGE_ERROR,
						     NULL,
						     _("There was an error opening the Help file"));
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (helperr_dlg),
							  error->message);
		gtk_dialog_run (GTK_DIALOG (helperr_dlg));
		gtk_widget_destroy (helperr_dlg);
		g_error_free (error);
		error = NULL;
	}
}

void
on_about_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GdkPixbuf *pixbuf;
	
	const char *authors [] = {
		"Andrew Burton <adb@iinet.net.au>",
		"Based on GXMame",
		"Stephane Pontier <shadow_walker@users.sourceforge.net>",
		"Benoit Dumont <vertigo17@users.sourceforge.net>",
		"Nicos Panayides <anarxia@gmx.net>",
		"Priit Laes <x-laes@users.sourceforge.net>",
		"William Jon McCann <mccann@jhu.edu>",
		NULL
	};

	const char *translators = _("translator-credits");

	pixbuf = gmameui_get_icon_from_stock ("gmameui-screen");

	gtk_show_about_dialog (GTK_WINDOW (MainWindow),
			       "name", "GMAMEUI",
			       "version", VERSION,
			       "logo", pixbuf,
			       "copyright", _("Copyright (c) 2007-2010 Andrew Burton"),
			       "website", "http://gmameui.sourceforge.net",
			       "comments", _("A program to play MAME under Linux"),
			       "authors", authors,
			       "translator-credits", translators,
			       NULL);

	g_object_unref (pixbuf);
//DELETE	about_window_show ();
}

