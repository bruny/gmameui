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

static GtkWidget     *details_audit_result;
static GtkTextBuffer *details_audit_result_buffer;
static GtkWidget     *sample_check_result;
static GtkWidget     *rom_check_result;

static RomEntry *rom;

static gint romset_sigid;

static void
on_romset_audited (GmameuiAudit *audit, gchar *audit_line, gint type, gint auditresult, gpointer user_data);

static void
audit_game (RomEntry *localrom) 
{	
	while (gtk_events_pending ()) gtk_main_iteration ();

	if (!current_exec) {
		gtk_label_set_text (GTK_LABEL (rom_check_result), _("Can't audit game"));
		if (localrom->nb_samples > 0) {
			gtk_label_set_text (GTK_LABEL (sample_check_result), _("Can't audit game"));
		} else {
			gtk_label_set_text (GTK_LABEL (sample_check_result), _("None required"));
		}
		return;
	}

	mame_audit_start_single (localrom->romname);
	
	rom = localrom;
	
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
	char    *value = NULL;

	/* find the clone name if there is a clone */
	if (strcmp (rom->cloneof, "-")) {
		RomEntry *tmprom;

		tmprom = get_rom_from_gamelist_by_name (gui_prefs.gl, rom->cloneof);
		if (tmprom) {
			value = g_strdup_printf ("%s - \"%s\"", rom_entry_get_list_name (tmprom), rom->cloneof);
		} else {
			value = g_strdup_printf (" - \"%s\"", rom->cloneof);
		}
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

/* Entry method for new UI */
void
show_rom_properties ()
{
	GtkWidget *dialog;
	GtkWidget *label;
	GtkWidget *scrolledwindow;
	gchar *value;
	char *title;
	RomEntry *rom;
	
	rom = gui_prefs.current_game;
	
	GladeXML *xml = glade_xml_new (GLADEDIR "rom_info.glade", "dialog1", GETTEXT_PACKAGE);
	
	dialog = glade_xml_get_widget (xml, "dialog1");
	
	gtk_window_set_title (GTK_WINDOW (dialog), rom_entry_get_list_name (rom));

	value = g_strdup_printf("<b>%s</b>", rom_entry_get_list_name (rom));
	label = glade_xml_get_widget (xml, "rom_name_lbl");
	gtk_label_set_markup (GTK_LABEL (label), value);
	
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
	gtk_label_set_text (GTK_LABEL (sample_check_result), _("None required"));
	
	scrolledwindow = glade_xml_get_widget (xml, "scrolledwindow");
	
	details_audit_result_buffer = gtk_text_buffer_new (NULL);
	details_audit_result = glade_xml_get_widget (xml, "details_audit_result");
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (details_audit_result), details_audit_result_buffer);
	
	romset_sigid = g_signal_connect (gui_prefs.audit, "romset-audited",
					 G_CALLBACK (on_romset_audited), NULL);
	
	g_idle_add (audit_idle, rom);
	
	gtk_dialog_run (GTK_DIALOG (dialog));
	
	gtk_widget_destroy (dialog);
	g_signal_handler_disconnect (gui_prefs.audit, romset_sigid);
	
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

static void
on_romset_audited (GmameuiAudit *audit, gchar *audit_line, gint type, gint auditresult, gpointer user_data)
{
	const       gchar *title;
	GtkTextIter text_iter;
	
	gchar *rom_status_desc[NUMBER_STATUS] = {
		_("Incorrect"),
		_("Passed"),
		_("Unknown"),
		"",
		_("Best available"),
		_("Not available"),
		""
	};
	
	/*GMAMEUI_DEBUG ("Properties window - romset audited, result is %d - %s", auditresult, audit_line);*/
	
	if (auditresult != NOTROMSET) {
		title = rom_status_desc[auditresult];
		if (type == AUDIT_TYPE_ROM) {
			rom->has_roms = auditresult;
			gtk_label_set_text (GTK_LABEL (rom_check_result), title);
		} else {
			rom->has_samples = auditresult;
			gtk_label_set_text (GTK_LABEL (sample_check_result), title);
		}

		update_game_in_list (rom);
	} else {
		/* Line relates to a ROM within the ROM set */
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (details_audit_result_buffer), &text_iter);
		gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_audit_result_buffer), &text_iter, audit_line, -1);
		
		/* Append carriage return */
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (details_audit_result_buffer), &text_iter);
		gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_audit_result_buffer), &text_iter, "\n", -1);
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

