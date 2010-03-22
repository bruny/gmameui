/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2010 Andrew Burton <adb@iinet.net.au>
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

#include "gmameui-listoutput-dlg.h"
#include "gui.h"	/* main_gui */
#include "mame-exec-list.h"
#include "gmameui-listoutput.h"

struct _GMAMEUIListOutputDialogPrivate {
	GtkBuilder *builder;

	GMAMEUIListOutput *parser;

	GtkWidget *primary_lbl;
	GtkWidget *secondary_lbl;
	GtkWidget *action_lbl;
	GtkWidget *progress;

	/* Signal emitted when the listoutput or listxml process finishes */
	gint command_sigid;
	gint romset_sigid;

};

#define GMAMEUI_LISTOUTPUT_DIALOG_GET_PRIVATE(o)  (GMAMEUI_LISTOUTPUT_DIALOG (o)->priv)

G_DEFINE_TYPE (GMAMEUIListOutputDialog, gmameui_listoutput_dialog, GTK_TYPE_DIALOG)

/* Function prototypes */
static void
gmameui_listoutput_dialog_response             (GtkDialog *dialog, gint response);
static void
gmameui_listoutput_dialog_destroy              (GtkObject *object);

/* Boilerplate functions */
static GObject *
gmameui_listoutput_dialog_constructor (GType                  type,
				guint                  n_construct_properties,
				GObjectConstructParam *construct_properties)
{
	GObject          *obj;
	GMAMEUIListOutputDialog *dialog;

	obj = G_OBJECT_CLASS (gmameui_listoutput_dialog_parent_class)->constructor (type,
									     n_construct_properties,
									     construct_properties);

	dialog = GMAMEUI_LISTOUTPUT_DIALOG (obj);

	return obj;
}

static void
gmameui_listoutput_dialog_class_init (GMAMEUIListOutputDialogClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
	GtkDialogClass *gtkdialog_class = GTK_DIALOG_CLASS (class);

	gobject_class->constructor = gmameui_listoutput_dialog_constructor;
/*	gobject_class->get_property = gmameui_listoutput_dialog_get_property;
	gobject_class->set_property = gmameui_listoutput_dialog_set_property;*/

	gtkobject_class->destroy = gmameui_listoutput_dialog_destroy;
	gtkdialog_class->response = gmameui_listoutput_dialog_response;


	g_type_class_add_private (class,
				  sizeof (GMAMEUIListOutputDialogPrivate));

	/* Signals and properties go here */

}

static gboolean
start_listoutput_parse (gpointer user_data)
{
	GMAMEUIListOutput *parser;

	parser = (GMAMEUIListOutput *) user_data;

	g_return_val_if_fail (parser != NULL, FALSE);

	gmameui_listoutput_parse (parser);

	/* Only trigger the start of the listoutput parse once */
	return FALSE;
}

static void
on_romset_parsed (GMAMEUIListOutput *parser, gchar *romset_name, gint count, gint target, gpointer user_data)
{
	GMAMEUIListOutputDialog *dlg;
	gchar *markup, *rom_action;
	gfloat pos;
	
	dlg = (gpointer) user_data;

	/* Put text describing which ROM is being processed */
	rom_action = g_strdup_printf (_("Processing romset %s"), romset_name);
	markup = g_markup_printf_escaped ("<span style=\"italic\">%s</span>", rom_action);
	gtk_label_set_markup (GTK_LABEL (dlg->priv->action_lbl), markup);
	g_free (markup);
	g_free (rom_action);

	/* Update the percentage of the progress bar */
	pos = count ? ((float) count) / target : 0.0;

	if (pos > 0 && pos <= 1) {
		gchar *progress_msg;

		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (dlg->priv->progress), pos);

		progress_msg = g_strdup_printf (_("%d of %d romsets processed"), count, target);
		gtk_progress_bar_set_text (GTK_PROGRESS_BAR (dlg->priv->progress), progress_msg);
		g_free (progress_msg);
		
		UPDATE_GUI;
	}
}

static void
on_listoutput_complete (GMAMEUIListOutput *parser, gpointer user_data)
{
	GMAMEUIListOutputDialog *dlg = (gpointer) user_data;

	/* Destroy the window */
	gtk_widget_destroy (GTK_WIDGET (dlg));
}

static void
gmameui_listoutput_dialog_init (GMAMEUIListOutputDialog *dialog)
{
	GMAMEUIListOutputDialogPrivate *priv;

	GtkWidget *listoutput_vbox;

	GError* error = NULL;

	const gchar *object_names[] = {
		"vbox1"
	};


	priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
					    GMAMEUI_TYPE_LISTOUTPUT_DIALOG,
					    GMAMEUIListOutputDialogPrivate);

	dialog->priv = priv;
	
	/* Initialise private variables */

	/* Build the UI and connect signals here */
	priv->builder = gtk_builder_new ();
	gtk_builder_set_translation_domain (priv->builder, GETTEXT_PACKAGE);
	
	if (!gtk_builder_add_objects_from_file (priv->builder,
	                                        GLADEDIR "gmameui_listoutput.builder",
	                                        (gchar **) object_names,
	                                        &error)) {
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
		return;
	}

	/* Get the dialog contents */
	listoutput_vbox = GTK_WIDGET (gtk_builder_get_object (priv->builder, "vbox1"));

	/* Add our dialog contents to the vbox of the dialog class */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
						listoutput_vbox, TRUE, TRUE, 0);
	

	/* Set the primary and secondary text */
	priv->primary_lbl = GTK_WIDGET (gtk_builder_get_object (priv->builder, "primary_lbl"));
	priv->secondary_lbl = GTK_WIDGET (gtk_builder_get_object (priv->builder, "secondary_lbl"));
	priv->action_lbl = GTK_WIDGET (gtk_builder_get_object (priv->builder, "action_lbl"));

	priv->progress = GTK_WIDGET (gtk_builder_get_object (priv->builder, "pgrsbar_listoutput"));

	priv->parser = gmameui_listoutput_new ();
	gmameui_listoutput_set_exec (priv->parser, mame_exec_list_get_current_executable (main_gui.exec_list));

	/* Signal emitted whenever the listoutput or listxml process parses a romset */
	priv->romset_sigid = g_signal_connect (priv->parser, "listoutput-romset-parsed",
										   G_CALLBACK (on_romset_parsed), dialog);
	
	/* Signal emitted when the listoutput or listxml process finishes */
	priv->command_sigid = g_signal_connect (priv->parser, "listoutput-parse-finished",
											G_CALLBACK (on_listoutput_complete), dialog);
	
	/* Start the parsing process */
	g_idle_add (start_listoutput_parse, priv->parser);
}

GtkWidget *
gmameui_listoutput_dialog_new (GtkWindow *parent)
{
	GtkWidget *dialog;

	dialog = g_object_new (GMAMEUI_TYPE_LISTOUTPUT_DIALOG,
			       "title", _("Rebuilding gamelist"),
			       NULL);

	gtk_dialog_add_button (GTK_DIALOG (dialog),
	                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

	if (parent)
		gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

	return dialog;

}

static void
gmameui_listoutput_dialog_response (GtkDialog *dialog, gint response)
{
	GMAMEUIListOutputDialogPrivate *priv;
	
	priv = G_TYPE_INSTANCE_GET_PRIVATE (GMAMEUI_LISTOUTPUT_DIALOG (dialog),
					    GMAMEUI_TYPE_LISTOUTPUT_DIALOG,
					    GMAMEUIListOutputDialogPrivate);
	
	switch (response)
	{
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_DELETE_EVENT:
			/* Cancel button clicked - need to stop the parsing */
			gmameui_listoutput_parse_stop (priv->parser);
			
			gtk_widget_destroy (GTK_WIDGET (dialog));

			break;
		default:
			g_assert_not_reached ();
	}
}

static void
gmameui_listoutput_dialog_destroy (GtkObject *object)
{
	GMAMEUIListOutputDialog *dlg;
	
GMAMEUI_DEBUG ("Destroying gmameui listoutput dialog...");	
	dlg = GMAMEUI_LISTOUTPUT_DIALOG (object);
	
	if (dlg->priv->builder)
		g_object_unref (dlg->priv->builder);

	if (dlg->priv->parser)
		g_object_unref (dlg->priv->parser);
	
	g_object_unref (dlg->priv);
	
/*	GTK_OBJECT_CLASS (gmameui_listoutput_dialog_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying gmamui listoutput dialog... done");
}
