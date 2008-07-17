/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
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

#include <glade/glade.h>

#include "gui_prefs.h"
#include "gui_prefs_dialog.h"
#include "common.h"
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
//	GladeXML *xml;

	/* Startup preferences *
	GtkWidget *gui_joy_entry;
	GtkWidget *gui_joy_checkbutton;
	GtkWidget *gui_use_xmame_options_checkbutton;
	GtkWidget *checkgames_checkbutton;
	GtkWidget *VersionCheck_checkbutton;*/
	
	/* Column layout preferences */
	
	/* Miscellaneous option preferences *
	GtkWidget *theprefix_checkbutton;
	GtkWidget *theprefix_label;
	GtkWidget *clone_color_button;
	GtkWidget *clone_label;*/
};

/* Preferences dialog */
static void mame_gui_prefs_dialog_class_init (MameGuiPrefsDialogClass *class);
static void mame_gui_prefs_dialog_init (MameGuiPrefsDialog *dlg);
static void on_prefs_checkbutton_toggled (GtkWidget *toggle, 
										  gchar *widget_name);
static void on_prefs_col_checkbutton_toggled (GtkWidget *toggle,
											  gchar *widget_name);
static void on_prefs_checkbutton_theprefix_toggled (GtkWidget *toggle, 
													gpointer user_data);
static void on_prefs_colour_button_toggled (GtkWidget *color, 
											gpointer user_data);
static void on_mame_gui_prefs_dialog_destroyed (GtkWidget *prefs_dialog,
												gpointer user_data);
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
	GtkWidget *clone_lbl;
	GtkWidget *theprefix_label;
	
	GdkColor color;
	GList *col_list;	/* GList of column checkbutton widgets */
	GList *node;
	
	gboolean gamecheck;
	gboolean versioncheck;
	gboolean usexmameoptions;
	gboolean usejoyingui;
	gboolean theprefix;
	
	gchar *clone_color;
	
	GValueArray *cols_shown;
GMAMEUI_DEBUG ("Initialising gui prefs dialog");	
	dlg->priv = g_new0 (MameGuiPrefsDialogPrivate, 1);
	
	/* Now set up the dialog */
	GladeXML *xml = glade_xml_new (GLADEDIR "gmameui_prefs.glade", "dialog1", GETTEXT_PACKAGE);
	if (!xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "gmameui_prefs.glade");
		return;
	}
	dlg = glade_xml_get_widget (xml, "dialog1");
	gtk_widget_show (GTK_WIDGET (dlg));
	
	/* Startup option widgets */
	
	g_object_get (main_gui.gui_prefs,
				  /* Startup options */
				  "gamecheck", &gamecheck,
				  "versioncheck", &versioncheck,
				  "usexmameoptions", &usexmameoptions,
				  "usejoyingui", &usejoyingui,
				  /* Column options */
				  "cols-shown", &cols_shown,
				  /* Miscellaneous options */
				  "theprefix", &theprefix,
				  "clone-color", &clone_color,
				  NULL);

	widget = glade_xml_get_widget (xml, "gamecheck");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), gamecheck);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "gamecheck");

	/* TODO Set active based upon value */
	widget = glade_xml_get_widget (xml, "versioncheck");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), versioncheck);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "versioncheck");
	
	widget = glade_xml_get_widget (xml, "usexmameoptions");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), usexmameoptions);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "usexmameoptions");
	
	widget = glade_xml_get_widget (xml, "usejoyingui");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), usejoyingui);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_toggled), "usejoyingui");
	
	/* FIXME TODO - joy dev entry */

	/* Column layout widgets - set values */
	int i;
	for (i = 0; i < 9; i++) {
		GMAMEUI_DEBUG ("Processing %s", layout[i].widget_name);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glade_xml_get_widget (xml, layout[i].widget_name)),
					      g_value_get_int (g_value_array_get_nth (cols_shown, layout[i].column_id)));
	}

	/* Column layout widgets - set callbacks */
	col_list = glade_xml_get_widget_prefix (xml, "col");
	node = col_list;
	while (node) {
		GtkWidget *col_chk_widget;
		gchar *name;
		
		col_chk_widget = node->data;
		name = g_strdup (glade_get_widget_name (node->data));
		GMAMEUI_DEBUG ("Adding widget %s", name);		
		g_signal_connect (col_chk_widget, "toggled",
						  G_CALLBACK (on_prefs_col_checkbutton_toggled), name);
		/* FIXME TODO Can't free name here since it is used in the callback - need to free it elsewhere */
		
		node = g_list_next (node);
	}

	g_list_free (col_list);
	g_list_free (node);
	
	/* Miscellaneous option widgets */
	theprefix_label = glade_xml_get_widget (xml, "theprefix_lbl");
	
	widget = glade_xml_get_widget (xml, "theprefix");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), theprefix);
	g_signal_connect (widget, "toggled", 
			  G_CALLBACK (on_prefs_checkbutton_theprefix_toggled), theprefix_label);
	/* Default value may be off, so need to manually trigger */
	on_prefs_checkbutton_theprefix_toggled (widget, theprefix_label);

	/* Set up colour button and example label */
	clone_lbl = glade_xml_get_widget (xml, "clone_label");
	
	widget = glade_xml_get_widget (xml, "clone_color");
	gdk_color_parse (clone_color, &color);
	g_signal_connect (widget, "color-set",
			  G_CALLBACK (on_prefs_colour_button_toggled), clone_lbl);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (widget), &color);
	
	gtk_dialog_set_default_response (GTK_DIALOG (dlg), GTK_RESPONSE_CLOSE);
	
	g_object_unref (xml);
	
	g_signal_connect (G_OBJECT (dlg), "destroy",
					  G_CALLBACK (on_mame_gui_prefs_dialog_destroyed),
					  NULL);
	
	g_signal_connect_object (G_OBJECT (dlg), "delete_event",
							 G_CALLBACK (on_mame_gui_prefs_dialog_deleted),
							 dlg, 0);
	
	g_signal_connect_object (G_OBJECT (dlg), "response",
							 G_CALLBACK (on_mame_gui_prefs_dialog_response),
							 dlg, 0);
GMAMEUI_DEBUG ("Initialising gui prefs dialog... done");
}

GtkWidget *
mame_gui_prefs_dialog_new (void)
{
	return g_object_new (MAME_TYPE_GUI_PREFS_DIALOG,
			     "title", "GMAMEUI Preferences",
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

/* This callback is slightly different - we pass the GtkWidget representing
   the example label, and update it */
static void
on_prefs_checkbutton_theprefix_toggled (GtkWidget *toggle, 
										gpointer user_data)
{
	GtkWidget *example_lbl;
	gboolean active;
	gchar *text;

	example_lbl = (GtkWidget *) user_data;	
	
	GMAMEUI_DEBUG ("theprefix toggled");
	active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle));

	/* Trigger the set, which causes a save in the mame_gui_prefs_set_property () function */
	g_object_set (main_gui.gui_prefs,
				  "theprefix", active,
				  NULL);
	
	/* Update the label to show the impact */
	if (active)
		text = g_strdup ("The King of Fighters 2001 (Set 1)");
	else
		text = g_strdup ("King of Fighters 2001, The (Set 1)");
		
	gtk_label_set_text (GTK_LABEL (example_lbl), text);

	g_free (text);
}

static void
on_prefs_colour_button_toggled (GtkWidget *color, 
								gpointer user_data)
{

	GtkWidget *clone_lbl;
	gchar *color_string;
	GdkColor *newcolour = NULL;
	
	newcolour = g_malloc (sizeof (GdkColor));
	
	gtk_color_button_get_color (GTK_COLOR_BUTTON (color), newcolour);
	color_string = gdk_color_to_string (newcolour);
	GMAMEUI_DEBUG ("clone-color changed colour to %s", color_string);

	/* Trigger the set, which causes a save in the mame_gui_prefs_set_property () function */
	g_object_set (main_gui.gui_prefs,
				  "clone-color", color_string,
				  NULL);

	/* Set the colour of the example label */
	clone_lbl = (GtkWidget *) user_data;
	gtk_widget_modify_fg (clone_lbl, GTK_STATE_NORMAL, newcolour);

	if (color_string)
		g_free (color_string);
/*	if (newcolour)
		gdk_color_free (newcolour);*/

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
	ListMode mode;
GMAMEUI_DEBUG("Response from gui prefs dialog");
	if (response_id == GTK_RESPONSE_CLOSE)
		gtk_widget_hide (GTK_WIDGET (dlg));
	
	/* Recreate gamelist so that clone color etc take effect */
	g_object_get (main_gui.gui_prefs, "current-mode", &mode, NULL);
	create_gamelist (mode);
	create_gamelist_content ();
GMAMEUI_DEBUG("Response from gui prefs dialog... done");
}
