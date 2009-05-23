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
#include <glade/glade.h>
#include "gmameui-rominfo-dlg.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audit.h"
#include "gui.h"

struct _MameRomInfoDialogPrivate {
	GladeXML *xml;

	gint romset_sigid;

	GtkWidget     *details_audit_result;
	GtkTextBuffer *details_audit_result_buffer;
	GtkWidget     *sample_check_result;
	GtkWidget     *rom_check_result;
	
	RomEntry *rom;
};

#define MAME_ROMINFO_DIALOG_GET_PRIVATE(o)  (MAME_ROMINFO_DIALOG (o)->priv)

G_DEFINE_TYPE (MameRomInfoDialog, mame_rominfo_dialog, GTK_TYPE_DIALOG)

/* Function prototypes */
static void
mame_rominfo_dialog_response             (GtkDialog *dialog, gint response);
static void
mame_rominfo_dialog_destroy              (GtkObject *object);

static void
on_romset_audited (GmameuiAudit *audit, gchar *audit_line, gint type, gint auditresult, gpointer user_data);

static void
audit_game (MameRomInfoDialog *dialog) 
{	
	while (gtk_events_pending ()) gtk_main_iteration ();

	if (!mame_exec_list_has_current_executable (main_gui.exec_list)) {
		gtk_label_set_text (GTK_LABEL (dialog->priv->rom_check_result), _("Can't audit game"));
		if (dialog->priv->rom->nb_samples > 0) {
			gtk_label_set_text (GTK_LABEL (dialog->priv->sample_check_result), _("Can't audit game"));
		} else {
			gtk_label_set_text (GTK_LABEL (dialog->priv->sample_check_result), _("None required"));
		}
		return;
	}

	g_return_if_fail (dialog != NULL);
	g_return_if_fail (dialog->priv->rom != NULL);
	
	mame_audit_start_single (dialog->priv->rom->romname);
	
}

static gboolean
audit_idle (gpointer data)
{
	audit_game ((MameRomInfoDialog *) data);

	return FALSE;
}

/* FIXME TODO Move to RomEntry */
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

/* FIXME TODO Move to RomEntry */
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

/* FIXME TODO Move to RomEntry */
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
on_romset_audited (GmameuiAudit *audit, gchar *audit_line, gint type, gint auditresult, gpointer user_data)
{
	MameRomInfoDialog *dialog;
	const       gchar *title;
	GtkTextIter text_iter;
	
	dialog = (MameRomInfoDialog *) user_data;
	
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
			dialog->priv->rom->has_roms = auditresult;
			gtk_label_set_text (GTK_LABEL (dialog->priv->rom_check_result), title);
		} else {
			dialog->priv->rom->has_samples = auditresult;
			gtk_label_set_text (GTK_LABEL (dialog->priv->sample_check_result), title);
		}

		/* Update the game in the MameGamelistView */
		mame_gamelist_view_update_game_in_list (main_gui.displayed_list, dialog->priv->rom);
		
	} else if (g_str_has_prefix (audit_line, dialog->priv->rom->romname)) {
		/* Line relates to a ROM within the ROM set */
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (dialog->priv->details_audit_result_buffer), &text_iter);
		gtk_text_buffer_insert (GTK_TEXT_BUFFER (dialog->priv->details_audit_result_buffer), &text_iter, audit_line, -1);
		
		/* Append carriage return */
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (dialog->priv->details_audit_result_buffer), &text_iter);
		gtk_text_buffer_insert (GTK_TEXT_BUFFER (dialog->priv->details_audit_result_buffer), &text_iter, "\n", -1);
	} else {
		/* Other output from MAME that we can ignore */
	}
}

/* Boilerplate functions */
static GObject *
mame_rominfo_dialog_constructor (GType                  type,
				guint                  n_construct_properties,
				GObjectConstructParam *construct_properties)
{
	GObject          *obj;
	MameRomInfoDialog *dialog;

	obj = G_OBJECT_CLASS (mame_rominfo_dialog_parent_class)->constructor (type,
									     n_construct_properties,
									     construct_properties);

	dialog = MAME_ROMINFO_DIALOG (obj);

	return obj;
}

static void
mame_rominfo_dialog_class_init (MameRomInfoDialogClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
	GtkDialogClass *gtkdialog_class = GTK_DIALOG_CLASS (class);

	gobject_class->constructor = mame_rominfo_dialog_constructor;
/*	gobject_class->get_property = mame_rominfo_dialog_get_property;
	gobject_class->set_property = mame_rominfo_dialog_set_property;*/

	gtkobject_class->destroy = mame_rominfo_dialog_destroy;
	gtkdialog_class->response = mame_rominfo_dialog_response;


	g_type_class_add_private (class,
				  sizeof (MameRomInfoDialogPrivate));

	/* Signals and properties go here */

}

static void
mame_rominfo_dialog_init (MameRomInfoDialog *dialog)
{
	GtkWidget *rominfo_vbox;
	GtkWidget *label;
	GtkWidget *scrolledwindow;
	gchar *value;
	char *title;
	
	MameRomInfoDialogPrivate *priv;

	priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
					    MAME_TYPE_ROMINFO_DIALOG,
					    MameRomInfoDialogPrivate);

	dialog->priv = priv;
	
	/* Initialise private variables */
	
	priv->rom = gui_prefs.current_game;

	g_return_if_fail (priv->rom != NULL);

	/* Build the UI and connect signals here */
	priv->xml = glade_xml_new (GLADEDIR "rom_info.glade", "vbox2", GETTEXT_PACKAGE);
	if (!priv->xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "rom_info.glade");
		return;
	}

	/* Get the dialog contents */
	rominfo_vbox = glade_xml_get_widget (priv->xml, "vbox2");

	/* Add our dialog contents to the vbox of the dialog class */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    rominfo_vbox, TRUE, TRUE, 0);
		
	gtk_widget_show_all (GTK_WIDGET (rominfo_vbox));
	
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
	
	gtk_window_set_title (GTK_WINDOW (dialog), rom_entry_get_list_name (priv->rom));

	value = g_strdup_printf("<b>%s</b>", rom_entry_get_list_name (priv->rom));
	label = glade_xml_get_widget (priv->xml, "rom_name_lbl");
	gtk_label_set_markup (GTK_LABEL (label), value);
	
	label = glade_xml_get_widget (priv->xml, "year_result");
	gtk_label_set_text (GTK_LABEL (label), priv->rom->year);

	label = glade_xml_get_widget (priv->xml, "manufacturer_result");
	gtk_label_set_text (GTK_LABEL (label), priv->rom->manu);
	
	title = get_rom_cpu_value (priv->rom);
	label = glade_xml_get_widget (priv->xml, "cpu_result");
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	title = get_rom_sound_value (priv->rom);
	label = glade_xml_get_widget (priv->xml, "sound_result");
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	/* Don't display resolution if this is a vector game */
	if (!priv->rom->vector)
		value = g_strdup_printf ("%i \303\227 %i ", priv->rom->screen_x, priv->rom->screen_y);
	else
		value = g_strdup_printf ("");
	title = g_strdup_printf ("%s%s %.2f Hz",
				 value,
				 priv->rom->horizontal ? "Horizontal" : "Vertical",
				 priv->rom->screen_freq);
	g_free (value);

	label = glade_xml_get_widget (priv->xml, "screen_result");
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	title = g_strdup_printf (_("%i colors"), priv->rom->colors);
	label = glade_xml_get_widget (priv->xml, "colors_result");
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	label = glade_xml_get_widget (priv->xml, "clone_result");
	GtkWidget *clone_label = glade_xml_get_widget (priv->xml, "clone_label");
	if (strcmp (priv->rom->cloneof, "-")) {
		/* Get the title of the original ROM */
		title = get_rom_clone_name (priv->rom);
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
	priv->rom_check_result = glade_xml_get_widget (priv->xml, "rom_check_result");
	gtk_label_set_text (GTK_LABEL (priv->rom_check_result), _("Checking..."));
	
	priv->sample_check_result = glade_xml_get_widget (priv->xml, "sample_check_result");
	gtk_label_set_text (GTK_LABEL (priv->sample_check_result), _("None required"));
	
	scrolledwindow = glade_xml_get_widget (priv->xml, "scrolledwindow");
	
	priv->details_audit_result_buffer = gtk_text_buffer_new (NULL);
	priv->details_audit_result = glade_xml_get_widget (priv->xml, "details_audit_result");
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (priv->details_audit_result), priv->details_audit_result_buffer);
	
	priv->romset_sigid = g_signal_connect (gui_prefs.audit, "romset-audited",
					       G_CALLBACK (on_romset_audited), dialog);
	
	/* Add an idle callback to perform an audit of the ROM */
	g_idle_add (audit_idle, dialog);
		
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
}

GtkWidget *
mame_rominfo_dialog_new ()
{
	GtkWidget *dialog;

	dialog = g_object_new (MAME_TYPE_ROMINFO_DIALOG, NULL);

	return dialog;

}

static void
mame_rominfo_dialog_response (GtkDialog *dialog, gint response)
{
	MameRomInfoDialogPrivate *priv;
	
	priv = G_TYPE_INSTANCE_GET_PRIVATE (MAME_ROMINFO_DIALOG (dialog),
					    MAME_TYPE_ROMINFO_DIALOG,
					    MameRomInfoDialogPrivate);

	/* Disconnect signal handlers so that these handlers do not further
	   update the audit window. If we created the MameAudit object only when
	   it was used, and destroyed it straight after, we wouldn't need to do this 
	g_signal_handler_disconnect (gui_prefs.audit, priv->romset_sigid);
	g_signal_handler_disconnect (gui_prefs.audit, priv->command_sigid);
	g_signal_handler_disconnect (gui_prefs.audit, priv->command_sample_sigid);*/
	
	switch (response)
	{
		case GTK_RESPONSE_CLOSE:
			/* Close button clicked */
			gtk_widget_destroy (GTK_WIDGET (dialog));
			break;
			
		case GTK_RESPONSE_DELETE_EVENT:
			/* Dialog closed */
			
			/* Stop the auditing process */
			//mame_audit_stop_full_audit (gui_prefs.audit);
			
			gtk_widget_destroy (GTK_WIDGET (dialog));
			break;
		default:
			g_assert_not_reached ();
	}
}

static void
mame_rominfo_dialog_destroy (GtkObject *object)
{
	MameRomInfoDialog *dlg;
	
GMAMEUI_DEBUG ("Destroying mame_rominfo_dialog...");	
	dlg = MAME_ROMINFO_DIALOG (object);
	
	if (dlg->priv->xml)
		g_object_unref (dlg->priv->xml);
	
	g_signal_handler_disconnect (gui_prefs.audit, dlg->priv->romset_sigid);
	
	g_object_unref (dlg->priv);
	
/*	GTK_OBJECT_CLASS (mame_rominfo_dialog_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying mame_rominfo_dialog... done");
}