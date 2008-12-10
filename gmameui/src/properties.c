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
 *add_controller_options_tab_new
 */

#include "common.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkhseparator.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktextview.h>
#include <gtk/gtktable.h>
#include <gtk/gtkvbox.h>

#include <glade/glade.h>

#include "audit.h"
#include "properties.h"
#include "gmameui.h"
#include "gui.h"
#include "io.h"
#include "options.h"
#include "options_string.h"

#define BUFFER_SIZE 1000

static void
exit_properties_window (GtkWidget *window, gpointer user_data)
{
	GameOptions *opts;

	opts = (GameOptions*) g_object_get_data (G_OBJECT (window), "opts");
	
	game_options_free (opts);

	gtk_widget_destroy (window);
	/* reenable joystick, was disabled in callback.c (on_properties_activate)*/
	joy_focus_on ();
}


static void
properties_reset (GtkWidget *properties_window)
{
	gchar *filename;
	RomEntry *rom;
	GtkWidget *properties_apply_button;
	GtkWidget *properties_reset_button;
	GameOptions *opts;

	rom = (RomEntry*) g_object_get_data (G_OBJECT (properties_window), "rom");
	properties_apply_button = GTK_WIDGET (g_object_get_data (G_OBJECT (properties_window), "properties_apply_button"));
	properties_reset_button = GTK_WIDGET (g_object_get_data (G_OBJECT (properties_window), "properties_reset_button"));
	opts = (GameOptions*) g_object_get_data (G_OBJECT (properties_window), "opts");

	game_options_free (opts);

	filename = g_build_filename (g_get_home_dir (), ".gmameui", "options", rom->romname, NULL);
	unlink (filename);

	load_properties_options (rom, properties_window);

	gtk_widget_set_sensitive (properties_reset_button, FALSE);

	gtk_widget_set_sensitive (properties_apply_button, FALSE);
}

static void
properties_save (GtkWidget *properties_window)
{
	RomEntry *rom;
	GameOptions *opts;
	GtkWidget *button;

	button = GTK_WIDGET (g_object_get_data (G_OBJECT (properties_window), "properties_apply_button"));
	gtk_widget_set_sensitive (button, FALSE);

	rom = (RomEntry*) g_object_get_data (G_OBJECT (properties_window), "rom");
	opts = (GameOptions*) g_object_get_data (G_OBJECT (properties_window), "opts");

	save_properties_options (rom, opts);
	save_options (rom, opts);
}

static void
properties_response (GtkWidget *dialog,
		     gint       response_id,
		     gpointer   user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_OK:
		properties_save (dialog);
		/*game_options_free (opts);*/
		exit_properties_window (dialog, NULL);
		break;
	case GTK_RESPONSE_APPLY:
		properties_save (dialog);
		break;
	case GTK_RESPONSE_REJECT:
		properties_reset (dialog);
		break;
	case GTK_RESPONSE_CANCEL:
	default:
		exit_properties_window (dialog, NULL);
		break;
	}
}



/* Entry method for old view */
GtkWidget *
create_properties_windows (RomEntry *rom)
{
	GtkWidget *properties_windows;
	GtkWidget *title_label;
	GtkWidget *properties_apply_button;
	GtkWidget *properties_reset_button = NULL;
	GtkAccelGroup *accel_group;
	GtkWidget *notebook1;
	MameExec *current_exec;

	gchar *title;
	GameOptions *opts = NULL;

	accel_group = gtk_accel_group_new ();

	/* try to load the game options */
	
	if (rom) {
		opts = load_options (rom);
		title = g_strdup_printf (_("Properties for %s"), rom_entry_get_list_name (rom));
	} else {
		title = g_strdup (_("Default Properties"));
	}

	current_exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	properties_windows = gtk_dialog_new ();
	gtk_dialog_set_has_separator (GTK_DIALOG (properties_windows), FALSE);
	gtk_window_set_title (GTK_WINDOW (properties_windows), title);
	g_free (title);
	gtk_window_set_transient_for (GTK_WINDOW (properties_windows), GTK_WINDOW (MainWindow));
	gtk_window_set_modal (GTK_WINDOW (properties_windows), TRUE);

	if (!opts)
		opts = &default_options;

	if (rom) {
		properties_reset_button = gtk_dialog_add_button (GTK_DIALOG (properties_windows),
								 _("Reset to Defaults"),
								 GTK_RESPONSE_REJECT);
		g_object_set_data (G_OBJECT (properties_windows), "properties_reset_button",
				   properties_reset_button);
		gtk_widget_set_sensitive (properties_reset_button, opts != &default_options);
	}

	properties_apply_button = gtk_dialog_add_button (GTK_DIALOG (properties_windows),
							 GTK_STOCK_APPLY,
							 GTK_RESPONSE_APPLY);	
	gtk_widget_set_sensitive (properties_apply_button, FALSE);
	g_object_set_data (G_OBJECT (properties_windows), "properties_apply_button",
			   properties_apply_button);
	gtk_dialog_add_button (GTK_DIALOG (properties_windows),
			       GTK_STOCK_CANCEL,
			       GTK_RESPONSE_CANCEL);	
	gtk_dialog_add_button (GTK_DIALOG (properties_windows),
			       GTK_STOCK_OK,
			       GTK_RESPONSE_OK);	

	g_object_set_data (G_OBJECT (properties_windows), "opts", opts);

	if (rom)
		title = g_strdup_printf ("<big><b>%s</b></big>\n\"%s\"", rom->gamename, rom->romname);
	else
		title =  g_strdup_printf ("<big><b>%s</b></big>\n%s", _("Global game options"), _("Default options used by all games"));

	title_label = gtk_label_new (title);
	gtk_label_set_use_markup (GTK_LABEL (title_label), TRUE);
	g_free (title);
	
	gtk_widget_show (title_label);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (properties_windows)->vbox), title_label, TRUE, TRUE, 0);
	gtk_label_set_line_wrap (GTK_LABEL (title_label), TRUE);
	gtk_misc_set_padding (GTK_MISC (title_label), 10, 0);
	gtk_misc_set_alignment (GTK_MISC (title_label), 0, 0.5);			

	notebook1 = gtk_notebook_new ();
	gtk_widget_ref (notebook1);
	g_object_set_data_full (G_OBJECT (properties_windows), "notebook1", notebook1,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (notebook1);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (properties_windows)->vbox), notebook1, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (notebook1), 8);

	GMAMEUI_DEBUG ("Adding display options");
	add_display_options_tab (properties_windows,
				 GTK_NOTEBOOK (notebook1),
				 rom,
				 opts,
				 properties_apply_button,
				 properties_reset_button);

	GMAMEUI_DEBUG ("Adding rendering options");
	add_rendering_options_tab (properties_windows,
				   GTK_NOTEBOOK (notebook1),
				   rom,
				   opts,
				   properties_apply_button,
				   properties_reset_button);

	GMAMEUI_DEBUG ("Adding sound options");
	add_sound_options_tab (properties_windows,
			       GTK_NOTEBOOK (notebook1),
			       rom,
			       opts,
			       properties_apply_button,
			       properties_reset_button);

	GMAMEUI_DEBUG ("Adding controller options");
	add_controller_options_tab (properties_windows,
				    GTK_NOTEBOOK (notebook1),
				    rom,
				    opts,
				    properties_apply_button,
				    properties_reset_button);

	GMAMEUI_DEBUG ("Adding misc options");
	add_misc_options_tab (properties_windows,
			      GTK_NOTEBOOK (notebook1),
			      rom,
			      opts,
			      properties_apply_button,
			      properties_reset_button);

	if (!rom || rom->vector) {
		add_vector_options_tab (properties_windows,
					GTK_NOTEBOOK (notebook1),
					rom,
					opts,
					properties_apply_button,
					properties_reset_button);
	}

	if (rom) {
		g_object_set_data (G_OBJECT (properties_windows), "rom", (gpointer) rom);
	}

	g_signal_connect (properties_windows, "response",
			  G_CALLBACK (properties_response),
			  NULL);
	
	gtk_window_add_accel_group (GTK_WINDOW (properties_windows), accel_group);

	return properties_windows;
}
