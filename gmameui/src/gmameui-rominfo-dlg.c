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

#include <vte/vte.h>
#include <gio/gio.h>

#include "gmameui-rominfo-dlg.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audit.h"
#include "gui.h"
#include "options_string.h"
#include "gmameui-listoutput.h"

struct _MameRomInfoDialogPrivate {
	GtkBuilder *builder;

	gint romset_sigid;

	GtkWidget     *details_audit_result;
	GtkWidget     *sample_check_result;
	GtkWidget     *rom_check_result;
	
	MameRomEntry *rom;
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



/* Try and open the ROM using the default handler.
   This method looks in all the ROM paths for the specified <romname>.zip,
   and, if found, uses the default handler to open the file.

   TODO: If a clone and not found, try and open the parent.
   TODO: What about a directory?
 */
static void
on_show_rom_clicked (GtkWidget *widget, gpointer user_data)
{
	MameRomEntry *rom;
	GValueArray *va_rom_paths;
	gchar *romfilename;

	rom = (gpointer) user_data;
	romfilename = g_strdup_printf ("%s.zip", mame_rom_entry_get_romname (rom));

	/* Get the ROM paths */
	g_object_get (main_gui.gui_prefs, "rom-paths", &va_rom_paths, NULL);

	guint i;
	for (i = 0; i < va_rom_paths->n_values; i++) {
		GFile *file;
		GError *error = NULL;
		gchar *path;

		path = g_build_filename (g_value_get_string (g_value_array_get_nth (va_rom_paths, i)),
		                         romfilename,
		                         NULL);
		file = g_file_new_for_path (path);

		if (g_file_test (path, G_FILE_TEST_EXISTS)) {
			GMAMEUI_DEBUG ("Attempting to open file %s using default handler", path);
			gtk_show_uri (NULL, g_file_get_uri (file), GDK_CURRENT_TIME, &error);
			if (error) {
				GMAMEUI_DEBUG ("%s", error->message);
				g_error_free (error);
				error = NULL;
			}
		} else
			GMAMEUI_DEBUG ("Could not open file %s - file does not exist", path);
		
		g_free (path);
	}

	g_value_array_free (va_rom_paths);
	va_rom_paths = NULL;

	g_free (romfilename);
}

static void
audit_game (MameRomInfoDialog *dialog) 
{	
	while (gtk_events_pending ()) gtk_main_iteration ();

	if (!mame_exec_list_has_current_executable (main_gui.exec_list)) {
		gtk_label_set_text (GTK_LABEL (dialog->priv->rom_check_result), _("Can't audit game"));
		gtk_label_set_text (GTK_LABEL (dialog->priv->sample_check_result), _("Can't audit game"));
		
		return;
	}

	g_return_if_fail (dialog != NULL);
	g_return_if_fail (dialog->priv->rom != NULL);

	mame_audit_start_single (mame_rom_entry_get_romname (dialog->priv->rom));

	if (!mame_rom_entry_has_samples (dialog->priv->rom))
		gtk_label_set_text (GTK_LABEL (dialog->priv->sample_check_result), _("None required"));
	
}

static gboolean
audit_idle (gpointer data)
{
	audit_game ((MameRomInfoDialog *) data);

	return FALSE;
}

static void
on_romset_audited (GmameuiAudit *audit, gchar *audit_line, gint type, gint auditresult, gpointer user_data)
{
	MameRomInfoDialog *dialog;
	const       gchar *title;
	
	dialog = (MameRomInfoDialog *) user_data;
	
	/*GMAMEUI_DEBUG ("Properties window - romset audited, result is %d - %s", auditresult, audit_line);*/
	
	if (auditresult != NOTROMSET) {
		title = rom_status_string_value [auditresult];
		if (type == AUDIT_TYPE_ROM) {
			g_object_set (dialog->priv->rom, "has-roms", auditresult, NULL);

			gtk_label_set_text (GTK_LABEL (dialog->priv->rom_check_result), title);
		} else {
			g_object_set (dialog->priv->rom, "has-samples", auditresult, NULL);
			/* Only set the label if the ROM has samples */
			if (mame_rom_entry_has_samples (dialog->priv->rom))
				gtk_label_set_text (GTK_LABEL (dialog->priv->sample_check_result), title);
		}

		/* Update the game in the MameGamelistView */
		mame_gamelist_view_update_game_in_list (main_gui.displayed_list, dialog->priv->rom);
		
	}
	 else if (g_str_has_prefix (audit_line, mame_rom_entry_get_romname (dialog->priv->rom))) {
		/* Line relates to a ROM within the ROM set */

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
	MameRomInfoDialogPrivate *priv;
	
	MameExec *exec;
	GMAMEUIListOutput *parser;
	GtkWidget *rominfo_vbox;
	GtkWidget *label;
	GtkWidget *vte_audit, *vte_brothers, *vte_clones;
	gchar *value;
	gboolean horizontal;
	gint num_colours;
	gfloat freq;
	char *title;
	
	GSList *widgets;  /* For use with processing multiple widgets at once */
	GSList *node;	  /* For use with processing multiple widgets at once */

	GError *error = NULL;
	
	const gchar *object_names[] = {
		"vbox2",
		/* FIXME TODO"img_open_rom",
		"adjustment1",
		"adjustment2"*/
	};
	
	priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
					    MAME_TYPE_ROMINFO_DIALOG,
					    MameRomInfoDialogPrivate);

	dialog->priv = priv;
	
	/* Initialise private variables */
	priv->rom = gui_prefs.current_game;

	g_return_if_fail (priv->rom != NULL);

	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	g_return_if_fail (exec != NULL);

	/* Get extra details about the ROM from -xmlinfo that aren't stored in
	   the gamelist file */
	parser = gmameui_listoutput_new ();
	priv->rom = gmameui_listoutput_parse_rom (parser, exec, priv->rom);
	g_object_unref (parser);

	/* Build the UI and connect signals here */
	priv->builder = gtk_builder_new ();
	gtk_builder_set_translation_domain (priv->builder, GETTEXT_PACKAGE);

	if (!gtk_builder_add_objects_from_file (priv->builder,
	                                        GLADEDIR "rom_info.builder",
	                                        (gchar **) object_names,
	                                        &error)) {
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
		return;
	}

	/* Get the dialog contents */
	rominfo_vbox = GTK_WIDGET (gtk_builder_get_object (priv->builder, "vbox2"));

	/* Add our dialog contents to the vbox of the dialog class */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    rominfo_vbox, TRUE, TRUE, 0);
		
	gtk_widget_show_all (GTK_WIDGET (rominfo_vbox));
	
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
	
	gtk_window_set_title (GTK_WINDOW (dialog), mame_rom_entry_get_list_name (priv->rom));

	GMAMEUI_DEBUG ("Setting main details widgets...");
	value = g_strdup_printf("<b>%s</b>", mame_rom_entry_get_list_name (priv->rom));

	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "rom_name_lbl"));
	gtk_label_set_markup (GTK_LABEL (label), value);

	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "year_result"));
	gtk_label_set_text (GTK_LABEL (label), mame_rom_entry_get_year (priv->rom));

	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "manufacturer_result"));
	gtk_label_set_text (GTK_LABEL (label), mame_rom_entry_get_manufacturer (priv->rom));	
	GMAMEUI_DEBUG ("Setting main details widgets... done");

	widgets = gtk_builder_get_objects (priv->builder);
	node = g_slist_nth (widgets, 0);

	while (node)
	{
		const gchar *name;
		gint ival;
		gchar *val;
		DriverStatus dsval;
		GtkWidget *widget;
		widget = node->data;
		
		name = gtk_widget_get_name (widget);
		if (g_ascii_strncasecmp (name, "lbl_details-", 12) == 0) {
			name += 12;      /* Skip over lbl_details- */
		
			g_object_get (priv->rom, name, &val, NULL);
			gtk_label_set_text (GTK_LABEL (widget), val);

			g_free (val);
		} else if (g_ascii_strncasecmp (name, "lbl_ctrl-", 9) == 0) {
			name += 9;      /* Skip over lbl_ctrl- */
			g_object_get (priv->rom, name, &ival, NULL);
			val = g_strdup_printf ("%i", ival);
			gtk_label_set_text (GTK_LABEL (widget), val);

			g_free (val);
		} else if (g_ascii_strncasecmp (name, "lbl_driver-status", 17) == 0) {
			name += 4;      /* Skip over lbl_ */

			g_object_get (priv->rom, name, &dsval, NULL);
			gtk_label_set_text (GTK_LABEL (widget), driver_status_string_value[dsval]);
		}

		node = g_slist_next (node);
	}
	
	title = get_rom_cpu_value (priv->rom);
	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "cpu_result"));
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	title = get_rom_sound_value (priv->rom);
	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "sound_result"));
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	g_object_get (priv->rom,
		      "is-horizontal", &horizontal,
		      "screen-freq", &freq,
		      "num-colours", &num_colours,
		      NULL);

	/* Don't display resolution if this is a vector game */
	title = g_strdup_printf ("%s%s %.2f Hz",
				 mame_rom_entry_is_vector (priv->rom) ? "" : mame_rom_entry_get_resolution (priv->rom),
				 horizontal ? _("Horizontal") : _("Vertical"),
				 freq);

	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "screen_result"));
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "screentype_result"));
	gtk_label_set_text (GTK_LABEL (label), mame_rom_entry_is_vector (priv->rom) ? _("Vector") : _("Raster"));
	
	title = g_strdup_printf (_("%i colors"), num_colours);
	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "colors_result"));
	gtk_label_set_text (GTK_LABEL (label), title);
	g_free (title);

	GtkWidget *clone_label;
	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "clone_result"));
	clone_label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "clone_label"));
	if (mame_rom_entry_is_clone (priv->rom)) {
		/* Get the title of the original ROM */
		title = mame_rom_entry_get_parent_romname (priv->rom);
		gtk_label_set_text (GTK_LABEL (label), title);
	} else {
		gtk_widget_hide (label);
		gtk_widget_hide (clone_label);
	}
	
	/* Control type details */
	GMAMEUI_DEBUG ("Setting control type widgets...");
	ControlType val;
	label = GTK_WIDGET (gtk_builder_get_object (priv->builder, "lbl_ctrl-control-type"));
	g_object_get (priv->rom, "control-type", &val, NULL);
	gtk_label_set_text (GTK_LABEL (label), control_type_string_value[val]);

	GMAMEUI_DEBUG ("Setting control type widgets... done");

	gchar *command;
	gint argc;
	gchar **argv = NULL;
	
	/* Audit details */
	GMAMEUI_DEBUG ("Setting audit widgets...");	
	command = g_strdup_printf ("%s -%s %s %s",
	                           mame_exec_get_path (exec),
	                           mame_get_option_name (exec, "verifyroms"),
	                           create_rompath_options_string (exec),
	                           mame_rom_entry_get_romname (priv->rom));
	g_shell_parse_argv (command, &argc, &argv, NULL);
	g_free (command);

	vte_audit = vte_terminal_new ();
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (gtk_builder_get_object (priv->builder, "scrolledwindow_audit")),
	                                       vte_audit);
	gtk_widget_show (GTK_WIDGET (vte_audit));
	vte_terminal_fork_command (VTE_TERMINAL (vte_audit),
	                           mame_exec_get_path (exec),
	                           argv,
	                           NULL, NULL, TRUE, TRUE,TRUE); 

	GMAMEUI_DEBUG ("Setting audit widgets... done");

	/* Brothers */
	command = g_strdup_printf ("%s -%s %s %s",
	                           mame_exec_get_path (exec),
	                           mame_get_option_name (exec, "listbrothers"),
	                           create_rompath_options_string (exec),
	                           mame_rom_entry_get_romname (priv->rom));
	g_shell_parse_argv (command, &argc, &argv, NULL);
	g_free (command);

	vte_brothers = vte_terminal_new ();
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (gtk_builder_get_object (priv->builder, "scrolledwindow_brothers")),
	                                       vte_brothers);
	gtk_widget_show (GTK_WIDGET (vte_brothers));
	vte_terminal_fork_command (VTE_TERMINAL (vte_brothers),
	                           mame_exec_get_path (exec),
	                           argv,
	                           NULL, NULL, TRUE, TRUE,TRUE); 

	/* Clones */
	command = g_strdup_printf ("%s -%s %s %s",
	                           mame_exec_get_path (exec),
	                           mame_get_option_name (exec, "listclones"),
	                           create_rompath_options_string (exec),
	                           mame_rom_entry_get_romname (priv->rom));
	g_shell_parse_argv (command, &argc, &argv, NULL);
	g_free (command);

	vte_clones = vte_terminal_new ();
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (gtk_builder_get_object (priv->builder, "scrolledwindow_clones")),
	                                       vte_clones);
	gtk_widget_show (GTK_WIDGET (vte_clones));
	vte_terminal_fork_command (VTE_TERMINAL (vte_clones),
	                           mame_exec_get_path (exec),
	                           argv,
	                           NULL, NULL, TRUE, TRUE,TRUE);
	
	/* Show ROM button */
	GtkWidget *button = GTK_OBJECT (gtk_builder_get_object (priv->builder, "btn_open_rom"));
	g_signal_connect (button, "clicked",
	                  G_CALLBACK (on_show_rom_clicked), priv->rom);
	
	/* Get the ROM audit result labels so that they can be set later */
	priv->rom_check_result = GTK_WIDGET (gtk_builder_get_object (priv->builder, "rom_check_result"));
	priv->sample_check_result = GTK_WIDGET (gtk_builder_get_object (priv->builder, "sample_check_result"));
	

	priv->romset_sigid = g_signal_connect (gui_prefs.audit, "romset-audited",
					       G_CALLBACK (on_romset_audited), dialog);
	
	/* Add an idle callback to perform an audit of the ROM */
	g_idle_add (audit_idle, dialog);
		
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);

	/* Free the GtkBuilder objects */
	g_slist_free (widgets);
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
	
	if (dlg->priv->builder)
		g_object_unref (dlg->priv->builder);
	
	g_signal_handler_disconnect (gui_prefs.audit, dlg->priv->romset_sigid);
	
	g_object_unref (dlg->priv);
	
/*	GTK_OBJECT_CLASS (mame_rominfo_dialog_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying mame_rominfo_dialog... done");
}