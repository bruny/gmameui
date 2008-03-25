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

#include "properties.h"
#include "gmameui.h"
#include "gui.h"
#include "io.h"
#include "options.h"
#include "options_string.h"

#define BUFFER_SIZE 1000

static GtkWidget     *details_audit_result;
static GtkTextBuffer *details_audit_result_buffer;
static GtkWidget     *sample_check_result;
static GtkWidget     *rom_check_result;

static gboolean       game_checked = FALSE;

static void
audit_game (RomEntry *rom) 
{
	FILE       *xmame_pipe = NULL;
	gchar       line [BUFFER_SIZE];
	const       gchar *title;
	gchar      *command;
	gchar      *rompath_option;
	RomStatus   status;
	gboolean    error_during_check = FALSE;
	GtkTextIter text_iter;

	if (game_checked)
		return;

	while (gtk_events_pending ()) gtk_main_iteration ();

	if (!current_exec) {
		title = _("Can't audit game");
		gtk_label_set_text (GTK_LABEL (rom_check_result), title);
		if (rom->nb_samples > 0) {
			gtk_label_set_text (GTK_LABEL (sample_check_result), title);
		} else {
			gtk_label_set_text (GTK_LABEL (sample_check_result), _("None required"));
		}
		game_checked = TRUE;
		return;
	}
		
	GMAMEUI_DEBUG ("single audit");
	rompath_option = create_rompath_options_string (current_exec);
	command = g_strdup_printf ("%s -verifyroms %s %s 2>/dev/null",
				   current_exec->path,
				   rompath_option,
				   rom->romname);

	g_free (rompath_option);
	GMAMEUI_DEBUG ("running command %s", command);
	xmame_pipe = popen (command, "r");
	if (!xmame_pipe) {
		GMAMEUI_DEBUG ("Could not run %s", command);
		g_free (command);
		return;
	}

	gint auditresult;
	/* Loading */
	while (fgets (line, BUFFER_SIZE, xmame_pipe)) {
		/* jump the last comments */
		if (!line[0] || !line[1])	/* same as strlen <= 1 but faster */
			break;

		auditresult = process_audit_romset (line, error_during_check);

		if (auditresult == CORRECT) title = _("Passed");
		else if (auditresult == INCORRECT) title = _("Incorrect");
		else if (auditresult == BEST_AVAIL) title = _("Best available");
		else if (auditresult == UNKNOWN) title = _("Unknown");
		else if (auditresult == NOT_AVAIL) title = _("Not available");
		if (auditresult != NOTROMSET) {
			rom->has_roms = auditresult;
			gtk_label_set_text (GTK_LABEL (rom_check_result), title);
		} else {
			gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (details_audit_result_buffer), &text_iter);
			gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_audit_result_buffer), &text_iter, line, -1);
			error_during_check = TRUE;
		}
		while (gtk_events_pending ()) gtk_main_iteration ();
	}

	pclose (xmame_pipe);
	g_free (command);

	/* Samples now */
	if (rom->nb_samples > 0) {
		rompath_option = create_rompath_options_string (current_exec);

		command = g_strdup_printf ("%s %s -verifysamples %s 2>/dev/null",
					   current_exec->path,
					   rompath_option,
					   rom->romname);

		g_free (rompath_option);

		GMAMEUI_DEBUG ("running command %s", command);
		xmame_pipe = popen (command, "r");

		/* Loading */
		while (fgets (line, BUFFER_SIZE, xmame_pipe)) {
			/* jump the last comments */
			if (!line[0] || !line[1])
				break;
			auditresult = process_audit_romset (line, error_during_check);

			if (auditresult == CORRECT) {
				title = _("Passed");
				status = CORRECT;
			} else if (auditresult == INCORRECT) {
				title = _("Incorrect");
				status = INCORRECT;
			} else {
				status = UNKNOWN;
				title = _("Not found");
			}
			if (auditresult != NOTROMSET) {
				gtk_label_set_text (GTK_LABEL (sample_check_result), title);

				rom->has_samples = status;
				/*continue with the GUI */
				error_during_check = FALSE;
			} else {
				gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (details_audit_result_buffer), &text_iter);
				gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_audit_result_buffer), &text_iter, line, -1);
				error_during_check = TRUE;
			}
			while (gtk_events_pending ()) gtk_main_iteration ();
		}
		pclose (xmame_pipe);
		g_free (command);
	} else {
		gtk_label_set_text (GTK_LABEL (sample_check_result), _("None required"));
	}

	game_checked = TRUE;
	update_game_in_list (rom);
}

static gboolean
audit_idle (gpointer data)
{
	audit_game ((RomEntry *)data);

	return FALSE;
}

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

static char *
get_rom_clone_name (RomEntry *rom)
{
	GList   *l;
	char    *value = NULL;
	gboolean game_found = FALSE;

	/* find the clone name if there is a clone */
	if (strcmp (rom->cloneof, "-")) {
		RomEntry *tmprom;

		for (l = g_list_first (game_list.roms), game_found = FALSE; l; l = g_list_next (l)) {

			tmprom = (RomEntry *)l->data;
			if (!strcmp (tmprom->romname, rom->cloneof)) {
				if (tmprom->the_trailer && gui_prefs.ModifyThe)
					value = g_strdup_printf ("%s, The %s - \"%s\"",
								 tmprom->gamename, tmprom->gamenameext, rom->cloneof);
				else if (tmprom->the_trailer && !gui_prefs.ModifyThe)
					value = g_strdup_printf ("The %s %s - \"%s\"",
								 tmprom->gamename, tmprom->gamenameext, rom->cloneof);
				else
					value = g_strdup_printf ("%s %s - \"%s\"",
								 tmprom->gamename, tmprom->gamenameext, rom->cloneof);
				break;
			}
		}

		if (!value)
			value = g_strdup_printf (" - \"%s\"", rom->cloneof);
	}

	return value;
}

static char *
get_rom_cpu_value (RomEntry *rom)
{
	char *value = NULL;
	char *values [NB_CPU + 1];
	gint  i, j;

	j = 0;
	values[j] = NULL;
	for (i = 0; i < NB_CPU; i++) {
		if (strcmp (rom->cpu_info[i].name, "-")
		    && rom->cpu_info[i].name[0] != '\0') {
			values[j++] = g_strdup_printf ("%s %f MHz%s",
						       rom->cpu_info[i].name,
						       rom->cpu_info[i].clock / 1000000.0,
						       rom->cpu_info[i].sound_flag ? _(" (sound)") : " ");
		}
	}
	values[j++] = NULL;

	value = g_strjoinv ("\n", values);

	for (i = 0; i < j; i++)
		g_free (values[i]);

	return value;
}

static char *
get_rom_sound_value (RomEntry *rom)
{
	char *value = NULL;
	char *values [NB_CPU + 1];
	gint  i, j;

	j = 0;
	values[j] = NULL;
	for (i = 0; i < NB_CPU; i++) {
		if (strcmp (rom->sound_info[i].name, "-")
		    && rom->sound_info[i].name[0]) {
			if (rom->sound_info[i].clock == 0)
				values[j++] = g_strdup_printf ("%s", rom->sound_info[i].name);
			else
				values[j++] = g_strdup_printf ("%s %f MHz",
							      rom->sound_info[i].name,
							      rom->sound_info[i].clock / 1000000.0);
		}
	}
	values[j++] = NULL;

	value = g_strjoinv ("\n", values);

	for (i = 0; i < j; i++)
		g_free (values[i]);

	return value;
}

static void
add_general_tab (GtkWidget    *properties_window,
		 GtkNotebook  *target_notebook,
		 RomEntry     *rom,
		 GtkWidget    *apply_button,
		 GtkWidget    *reset_button)
{
	GtkWidget *general_table;
	GtkWidget    *general_label;
	GtkWidget    *image;
	GtkWidget    *scrolledwindow;
	GtkWidget    *label;
	gchar        *value;
	gchar        *title;

	GladeXML *xml = glade_xml_new (GLADEDIR "properties.glade", "general_table", NULL);
	general_table = glade_xml_get_widget (xml, "general_table");

	gtk_widget_show (general_table);

	label = glade_xml_get_widget (xml, "year_result");
	gtk_label_set_text (GTK_LABEL (label), rom->year);

	label = glade_xml_get_widget (xml, "manufacturer_result");
	gtk_label_set_text (GTK_LABEL (label), rom->manu);
	
	title = get_rom_cpu_value (rom);
	label = glade_xml_get_widget (xml, "cpu_result");
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	title = get_rom_sound_value (rom);
	label = glade_xml_get_widget (xml, "sound_result");
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	/* Don't display resolution if this is a vector game */
	if (!rom->vector)
		value = g_strdup_printf ("%i \303\227 %i ", rom->screen_x, rom->screen_y);
	else
		value = g_strdup_printf ("");
	title = g_strdup_printf ("%s%s %.2f Hz",
				 value,
				 rom->horizontal ? "Horizontal" : "Vertical",
				 rom->screen_freq);
	g_free (value);

	label = glade_xml_get_widget (xml, "screen_result");
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	title = g_strdup_printf (_("%i colors"), rom->colors);
	label = glade_xml_get_widget (xml, "colors_result");
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	label = glade_xml_get_widget (xml, "clone_result");
	GtkWidget *clone_label = glade_xml_get_widget (xml, "clone_label");
	if (strcmp (rom->cloneof, "-")) {
		/* Get the title of the original ROM */
		title = get_rom_clone_name (rom);
		gtk_label_set_text (GTK_LABEL (label), title);
		g_free (title);
	} else {
		gtk_widget_hide (label);
		gtk_widget_hide (clone_label);
	}
	
	/* Driver details - TODO 
	label = glade_xml_get_widget (xml, "lbl_emulation_status");
	gtk_label_set_text (GTK_LABEL (label), rom->driver_status_emulation);
	label = glade_xml_get_widget (xml, "lbl_color_status");
	gtk_label_set_text (GTK_LABEL (label), rom->driver_status_color);
	label = glade_xml_get_widget (xml, "lbl_sound_status");
	gtk_label_set_text (GTK_LABEL (label), rom->driver_status_sound);
	label = glade_xml_get_widget (xml, "lbl_graphic_status");
	gtk_label_set_text (GTK_LABEL (label), rom->driver_status_graphic);*/
	
	/* Set default labels for the check result labels - these will be changed */
	rom_check_result = glade_xml_get_widget (xml, "rom_check_result");
	gtk_label_set_text (GTK_LABEL (rom_check_result), _("Checking..."));
	
	sample_check_result = glade_xml_get_widget (xml, "sample_check_result");
	gtk_label_set_text (GTK_LABEL (sample_check_result), _("Checking..."));
	
	scrolledwindow = glade_xml_get_widget (xml, "scrolledwindow");
	
	details_audit_result_buffer = gtk_text_buffer_new (NULL);
	details_audit_result = glade_xml_get_widget (xml, "details_audit_result");
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (details_audit_result), details_audit_result_buffer);

	image = gmameui_get_image_from_stock ("gmameui-general-toolbar");
	general_label = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (general_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (general_label), gtk_label_new (_("General")), FALSE, FALSE, 0);
	gtk_widget_show_all (general_label);

	gtk_notebook_append_page (GTK_NOTEBOOK (target_notebook), general_table, general_label);
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

GtkWidget *
create_properties_windows (RomEntry *rom)
{
	GtkWidget *properties_windows;

	GtkWidget *title_label;

	GtkWidget *properties_apply_button;
	GtkWidget *properties_reset_button = NULL;
	GtkAccelGroup *accel_group;
	
	GtkWidget *notebook1;

	gchar *title;
	GameOptions *opts = NULL;

	game_checked = FALSE;

	accel_group = gtk_accel_group_new ();

	/* try to load the game options */
	
	if (rom) {
		opts = load_options (rom);
		title = g_strdup_printf (_("Properties for %s"), rom_entry_get_list_name (rom));
	} else {
		title = g_strdup (_("Default Properties"));
	}

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

	if (rom) {
		add_general_tab (properties_windows,
				 GTK_NOTEBOOK (notebook1),
				 rom,
				 properties_apply_button,
				 properties_reset_button);
		g_idle_add (audit_idle, rom);
	}

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

