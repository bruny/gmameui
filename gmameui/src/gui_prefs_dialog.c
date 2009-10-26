/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
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

#include "gui_prefs.h"
#include "gui_prefs_dialog.h"
#include "gui.h"

typedef struct _column_layout column_layout;

struct _column_layout {
	gint column_id;
	gchar *widget_name;
};

static const column_layout layout [] = {
	{ HAS_SAMPLES, "col_samples" },
	{ ROMNAME, "col_directory" },
	{ TIMESPLAYED, "col_playcount" },
	{ MANU, "col_manufacturer" },
	{ YEAR, "col_year" },
	{ DRIVER, "col_driver" },
	{ CLONE, "col_cloneof" },
	{ MAMEVER, "col_version" },
	{ CATEGORY, "col_category" },
};

struct _MameGuiPrefsDialogPrivate {
	GtkBuilder *builder;

	/* Startup preferences *
	GtkWidget *gui_joy_entry;
	GtkWidget *gui_joy_checkbutton;
	GtkWidget *gui_use_xmame_options_checkbutton;
	GtkWidget *checkgames_checkbutton;
	GtkWidget *VersionCheck_checkbutton;*/
	
	/* Column layout preferences */
	
	/* Miscellaneous option preferences *
	GtkWidget *theprefix_checkbutton;*/
};

/* Preferences dialog */
static void mame_gui_prefs_dialog_class_init (MameGuiPrefsDialogClass *class);
static void mame_gui_prefs_dialog_init (MameGuiPrefsDialog *dlg);
static void on_prefs_entry_changed (GtkWidget *entry, gchar *widget_name);
static void on_prefs_checkbutton_toggled (GtkWidget *toggle, gchar *widget_name);
static void on_prefs_col_checkbutton_toggled (GtkWidget *toggle, gchar *widget_name);

static void on_mame_gui_prefs_dialog_destroyed (GtkWidget *prefs_dialog, gpointer user_data);
static gboolean on_mame_gui_prefs_dialog_deleted (GtkWidget *window,
				                       GdkEventAny *event,
				                       MameGuiPrefsDialog *dlg);
static void on_mame_gui_prefs_dialog_response (GtkDialog *dialog,
				              int response_id,
				              MameGuiPrefsDialog *dlg);

G_DEFINE_TYPE (MameGuiPrefsDialog, mame_gui_prefs_dialog,
			   GTK_TYPE_DIALOG)

/* Preferences dialog */
static void
mame_gui_prefs_dialog_finalize (GObject *obj)
{
	GMAMEUI_DEBUG ("Finalising mame_gui_prefs_dialog object");
	
	MameGuiPrefsDialog *dlg = MAME_GUI_PREFS_DIALOG (obj);	

	/* TODO Unref objects */
	g_free (dlg->priv);
	
	GMAMEUI_DEBUG ("Finalising mame_gui_prefs_dialog object... done");
	
	((GObjectClass *) mame_gui_prefs_dialog_parent_class)->finalize (obj);
}

static void
mame_gui_prefs_dialog_class_init (MameGuiPrefsDialogClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->finalize = mame_gui_prefs_dialog_finalize;
}


/* Creates a hbox containing a treeview (the sidebar) and a notebook */
static void
mame_gui_prefs_dialog_init (MameGuiPrefsDialog *dlg)
{
	/* Widget declarations */
	GtkWidget *widget;
	GtkAccelGroup *accel_group;

	gboolean gamecheck;
	gboolean versioncheck;
	gboolean usexmameoptions;
	gboolean prefercustomicons;
	gboolean usejoyingui;
	gchar *joystick_name;
	gboolean theprefix;
		
	GValueArray *cols_shown;

	GError* error = NULL;

	const gchar *object_names[] = {
		"vbox1",	/* Dialog */
	};
	
	dlg->priv = g_new0 (MameGuiPrefsDialogPrivate, 1);

GMAMEUI_DEBUG ("Initialising gui prefs dialog");
	
	/* Now set up the dialog */
	dlg->priv->builder = gtk_builder_new ();
	gtk_builder_set_translation_domain (dlg->priv->builder, GETTEXT_PACKAGE);

	if (!gtk_builder_add_objects_from_file (dlg->priv->builder,
	                                        GLADEDIR "gmameui_prefs.builder",
	                                        (gchar **) object_names,
	                                        &error)) {
		g_warning ("  Couldn't load builder file: %s", error->message);
		g_error_free (error);
		return;
	}

	/* Get the dialog contents */
	widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, "vbox1"));

	/* Add our dialog contents to the vbox of the dialog class */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			    widget, TRUE, TRUE, 6);
		
	gtk_widget_show_all (GTK_WIDGET (widget));

	gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);

	accel_group = gtk_accel_group_new ();
	
	/* Startup option widgets */
	g_object_get (main_gui.gui_prefs,
		      /* Startup options */
		      "gamecheck", &gamecheck,
		      "versioncheck", &versioncheck,
		      "usexmameoptions", &usexmameoptions,
		      "prefercustomicons", &prefercustomicons,
		      "usejoyingui", &usejoyingui,
		      "joystick-name", &joystick_name,
		      /* Column options */
		      "cols-shown", &cols_shown,
		      /* Miscellaneous options */
		      "theprefix", &theprefix,
		      NULL);

	widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, "gamecheck"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), gamecheck);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "gamecheck");

	/* TODO Set active based upon value */
	widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, "versioncheck"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), versioncheck);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "versioncheck");
	
	widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, "usexmameoptions"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), usexmameoptions);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "usexmameoptions");
	
	widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, "prefercustomicons"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), prefercustomicons);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "prefercustomicons");

	widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, "usejoyingui"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), usejoyingui);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "usejoyingui");

	widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, "gui_joy_entry"));
	gtk_entry_set_text (GTK_ENTRY (widget), joystick_name);
	g_signal_connect (widget, "changed", 
			  G_CALLBACK (on_prefs_entry_changed), "joystick-name");

	/* Column layout widgets - set values and callbacks */
	int i;
	for (i = 0; i < 9; i++) {
		GtkWidget *col_chk_widget;
		
		GMAMEUI_DEBUG ("  Processing %s", layout[i].widget_name);
		col_chk_widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, layout[i].widget_name));
		
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (col_chk_widget),
					      g_value_get_int (g_value_array_get_nth (cols_shown, layout[i].column_id)));
		g_signal_connect (col_chk_widget, "toggled",
		                  G_CALLBACK (on_prefs_col_checkbutton_toggled), layout[i].widget_name);
	}

	/* Miscellaneous option widgets */
	widget = GTK_WIDGET (gtk_builder_get_object (dlg->priv->builder, "theprefix"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), theprefix);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "theprefix");
		
	g_signal_connect (G_OBJECT (dlg), "destroy",
	                  G_CALLBACK (on_mame_gui_prefs_dialog_destroyed), NULL);
	
	g_signal_connect_object (G_OBJECT (dlg), "delete_event",
	                         G_CALLBACK (on_mame_gui_prefs_dialog_deleted),
	                         dlg, 0);
	
	g_signal_connect_object (G_OBJECT (dlg), "response",
	                         G_CALLBACK (on_mame_gui_prefs_dialog_response),
	                         dlg, 0);

	gtk_window_add_accel_group (GTK_WINDOW (dlg), accel_group);
	
	widget = gtk_dialog_add_button (GTK_DIALOG (dlg),
					GTK_STOCK_CLOSE,
					GTK_RESPONSE_CLOSE);

	gtk_dialog_set_default_response (GTK_DIALOG (dlg), GTK_RESPONSE_CLOSE);
	
GMAMEUI_DEBUG ("Initialising gui prefs dialog... done");
}

GtkWidget *
mame_gui_prefs_dialog_new (void)
{
	GtkWidget *dialog;
	
	dialog = g_object_new (MAME_TYPE_GUI_PREFS_DIALOG,
	                       "title", _("GMAMEUI Preferences"),
	                       NULL);

/* FIXME TODO	if (parent)
		gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);*/

	return dialog;
}

static void
on_prefs_entry_changed (GtkWidget *entry, gchar *widget_name)
{
	GMAMEUI_DEBUG ("%s text changed", widget_name);

	/* Trigger the set, which causes a save in the mame_gui_prefs_set_property () function */
	g_object_set (main_gui.gui_prefs,
		      widget_name, gtk_entry_get_text (GTK_ENTRY (entry)),
		      NULL);
}

static void
on_prefs_checkbutton_toggled (GtkWidget *toggle, gchar *widget_name)
{
	GMAMEUI_DEBUG ("%s toggled", widget_name);

	/* Trigger the set, which causes a save in the mame_gui_prefs_set_property () function */
	g_object_set (main_gui.gui_prefs,
		      widget_name, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle)),
		      NULL);
}

/* This callback is invoked when one of the column checkbuttons is toggled */
static void
on_prefs_col_checkbutton_toggled (GtkWidget *toggle,
								  gchar *widget_name)
{
	GValueArray *shown_columns;
	gint toggle_val;
	int i;
	
	g_return_if_fail (widget_name != NULL);
	
	GMAMEUI_DEBUG ("%s toggled", widget_name);
	
	g_object_get (main_gui.gui_prefs, "cols-shown", &shown_columns, NULL);
	
	/* Don't disable Gamename column */
	g_value_set_int (g_value_array_get_nth (shown_columns, GAMENAME), TRUE);
	
	toggle_val = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle));
	
	/* Loop through each of the columns, and if the name matches the widget
	   which emitted the signal, then set the value */
	for (i = 0; i < 9; i++) {
		if (g_ascii_strcasecmp (widget_name, layout[i].widget_name) == 0)
			g_value_set_int (g_value_array_get_nth (shown_columns, layout[i].column_id),
					 toggle_val);
	}
	
	/* Debug purposes only */
	/*
	for (i = 0; i < NUMBER_COLUMN; i++) {
		GMAMEUI_DEBUG ("Value of array at %d is %d", i,
					   g_value_get_int (g_value_array_get_nth (shown_columns, i)));
	}*/

	/* Trigger the set, which causes a save in the mame_gui_prefs_set_property () function */
	g_object_set (main_gui.gui_prefs, "cols-shown", shown_columns, NULL);
	
	g_value_array_free (shown_columns);
	
	GMAMEUI_DEBUG("Setting integer array... done");	
}

static void 
on_mame_gui_prefs_dialog_destroyed (GtkWidget *prefs_dialog,
									gpointer user_data)
{
	GMAMEUI_DEBUG ("About to destroy");
	g_object_unref (prefs_dialog);
	
	prefs_dialog = NULL;
	GMAMEUI_DEBUG ("About to destroy - done");
}

static gboolean
on_mame_gui_prefs_dialog_deleted (GtkWidget *window,
				       GdkEventAny *event,
				       MameGuiPrefsDialog *dlg)
{
	gtk_widget_hide (GTK_WIDGET (dlg));

	return TRUE;
}

static void
on_mame_gui_prefs_dialog_response (GtkDialog *dialog,
				  int response_id,
				  MameGuiPrefsDialog *dlg)
{

GMAMEUI_DEBUG("Response from gui prefs dialog");
	if (response_id == GTK_RESPONSE_CLOSE)
		gtk_widget_hide (GTK_WIDGET (dlg));
	
GMAMEUI_DEBUG("Response from gui prefs dialog... done");
}
