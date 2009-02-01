/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code for AnjutaPreferencesDialog object
 * Copyright (C) 2002 Dave Camp
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

#include <string.h>
#include <stdlib.h>

#include "common.h"
#include <glade/glade.h>
#include "mame_options_legacy_dialog.h"
#include "mame_options_legacy.h"
#include "cell-renderer-captioned-image.h"
#include "gui.h"

struct _MameOptionsLegacyDialogPrivate {
	GladeXML *xml;
	
	GtkWidget *treeview;	/* This is for functionality of the sidebar */
	GtkListStore *store;	/* This is for functionality of the sidebar */

	GtkWidget *notebook;
};

enum {
	COL_NAME,
	COL_TITLE,
	COL_PIXBUF,
	COL_WIDGET,
	LAST_COL
};  /* Columns in the liststore */

#define MAME_AUDIT_DIALOG_GET_PRIVATE(o)  (MAME_AUDIT_DIALOG (o)->priv)

#define PREFERENCE_PROPERTY_PREFIX "preferences_"

G_DEFINE_TYPE (MameOptionsLegacyDialog, mame_options_legacy_dialog, GTK_TYPE_DIALOG)

/* Function prototypes */
static void
mame_options_legacy_dialog_response             (GtkDialog *dialog, gint response);
static void
mame_options_legacy_dialog_destroy              (GtkObject *object);

static void
mame_options_legacy_dialog_add_page (MameOptionsLegacyDialog *dlg, const gchar *widget_name,
			      const gchar *title, const gchar *icon_filename);


/* Functions to split a key and return either the category or the field */
static gchar *
get_key_category (gchar *key);

static gchar *
get_key_field (gchar *key);

typedef struct {
	gchar *parent;		/* Widget that, when changed, causes state of dependent widget to change */
	gchar *dependent;       /* Dependent widget */
	gchar *relationship;    /* Value that parent needs to be for child to be enabled */
} WidgetRelationship;

static const WidgetRelationship widget_relationships[] =
{
	
	{ "Video-autoframeskip", "Video-frameskip", "0" },
	{ "Video-autoframeskip", "Video-maxautoframeskip", "0" },
	{ "Video-autoframeskip", "Video-maxautoframeskip", "0" },
	{ "Artwork-artwork", "Artwork-use-backdrops", "1" },
	{ "Artwork-artwork", "Artwork-use-bezels", "1" },
	{ "Artwork-artwork", "Artwork-use-overlays", "1" },

};

typedef struct {
	gchar *prop_name;       /* e.g. Video.bpp, must match preferences_Video.bpp from Glade */
	gchar *recorded_values;	/* Values to record and line up against the Items */
} _combo_link;

_combo_link combo_links[] = {
	{ "Video-frameskip", "0:1:2:3:4:5:6:7:8:9:10:11:12" },
	{ "Video-bpp", "0:8:15:16:32" },
	{ "Video-effect", "none:scale2x:lq2x:hq2x:6tap2x:scan2:rgbscan:scan3:fakescan" },
	{ "Artwork-artwork_resolution", "0:1:2" },
	{ "Sound-samplefreq", "8000:11025:16000:22050:44100" },
	{ "Sound-dsp_plugin", "oss:sdl:arts" },
	{ "Sound-sound_mixer_plugin", "oss" },
	{ "Vector-vectorres", "320x200:640x480:800x600:1024x768:1280x1024:1600x1200" },
	{ "Input-joytype", "0:5" },
	{ "MAME-bios", "0:1:2:3:4:5:6:7:8:9:10:11" },
};

static gchar *
get_key_category (gchar *key)
{
	gchar **stv;
	gchar *category;
	
	/* LHS of first "-" is the category */
	stv = g_strsplit (key, "-", 2);
	category = g_strdup (stv[0]);
	g_strfreev (stv);
	
	return category;
}

static gchar *
get_key_field (gchar *key)
{
	gchar **stv;
	gchar *field;
	
	/* RHS of first "-" is the key */
	stv = g_strsplit (key, "-", 2);
	field = g_strdup (stv[1]);
	g_strfreev (stv);
	
	return field;
}

/* Boilerplate functions */
static GObject *
mame_options_legacy_dialog_constructor (GType                  type,
				guint                  n_construct_properties,
				GObjectConstructParam *construct_properties)
{
	GObject          *obj;
	MameOptionsLegacyDialog *dialog;

	obj = G_OBJECT_CLASS (mame_options_legacy_dialog_parent_class)->constructor (type,
									     n_construct_properties,
									     construct_properties);

	dialog = MAME_OPTIONS_LEGACY_DIALOG (obj);

	return obj;
}

static void
mame_options_legacy_dialog_class_init (MameOptionsLegacyDialogClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
	GtkDialogClass *gtkdialog_class = GTK_DIALOG_CLASS (class);
	
	gobject_class->constructor = mame_options_legacy_dialog_constructor;
/*	gobject_class->get_property = mame_options_legacy_dialog_get_property;
	gobject_class->set_property = mame_options_legacy_dialog_set_property;*/

	gtkobject_class->destroy = mame_options_legacy_dialog_destroy;
	gtkdialog_class->response = mame_options_legacy_dialog_response;

	g_type_class_add_private (class,
				  sizeof (MameOptionsLegacyDialogPrivate));

	/* Signals and properties go here */
}

static gchar*
get_property_value_as_string (GtkWidget *widget, gchar *key)
{
	gint  int_value;
	gdouble double_value;
	gchar* values;
	gchar *text_value = NULL;

	if (GTK_IS_TOGGLE_BUTTON (widget)) {
		int_value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
		text_value = g_strdup_printf ("%d", int_value);
	} else if (GTK_IS_SPIN_BUTTON (widget)) {
		int_value = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
		text_value = g_strdup_printf ("%d", int_value);
	} else if (GTK_IS_HSCALE (widget)) {
		double_value = gtk_range_get_value (GTK_RANGE (widget));
		text_value = g_strdup_printf ("%.2f", double_value);
	} else if (GTK_IS_COMBO_BOX (widget)) {
		gint idx;
		gchar **values_arr;
		
		values = g_object_get_data (G_OBJECT (widget), "untranslated");
		values_arr = g_strsplit (values, ":", -1);
		idx = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));
		if (values_arr[idx] != NULL)
			text_value = g_strdup (values_arr[idx]);
	} else
		GMAMEUI_DEBUG ("get_property_value_as_string: Unsupported widget: %s", key);
/*	switch (prop->object_type)
	{

	case GMAMEUI_PROPERTY_OBJECT_TYPE_ENTRY:
		text_value = gtk_editable_get_chars (GTK_EDITABLE (prop->object), 0, -1);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_TEXT:
	case GMAMEUI_PROPERTY_OBJECT_TYPE_FOLDER:
	*case GMAMEUI_PROPERTY_OBJECT_TYPE_FILE:*
		* Haven't catered for these yet
		   Folder:
		    text_value = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (prop->object));
		   File:
		    text_value = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (prop->object));
		*
		g_assert_not_reached ();
		break;
	}*/
	
	if (text_value && (strlen (text_value) == 0))
	{
		g_free (text_value);
		text_value = NULL;
	}
	return text_value;
}

static gint
get_property_value_as_int (GtkWidget *widget, gchar *key)
{
	gint  int_value;
	gchar *text_value;
	text_value = get_property_value_as_string (widget, key);
	int_value = atoi (text_value);
	g_free (text_value);	
	return int_value;
}	

static gdouble
get_property_value_as_double (GtkWidget *widget, gchar *key)
{
	gdouble  double_value;
	gchar *text_value;
	text_value = get_property_value_as_string (widget, key);
	double_value = atof (text_value);
	g_free (text_value);	
	return double_value;
}

static void
update_property_on_change_int (GtkWidget *widget, gpointer user_data)
{
	gchar *key;
	gint val;
	
	key = (gchar *) user_data;
	
	g_return_if_fail (key != NULL);
	
	GMAMEUI_DEBUG ("Callback received on integer widget for %s", key);
	val = get_property_value_as_int (widget, key);
	GMAMEUI_DEBUG ("Setting integer value of widget for %s to %d", key, val);
	mame_legacy_options_set_int (main_gui.legacy_options, key, val);
}

static void
update_property_on_change_str (GtkWidget *widget, gpointer user_data)
{
	gchar *key;
	gchar *val;
	
	key = (gchar *) user_data;
	
	g_return_if_fail (key != NULL);
	
	GMAMEUI_DEBUG ("Callback received on widget for %s", key);
	val = get_property_value_as_string (widget, key);
	GMAMEUI_DEBUG ("Setting value of widget for %s to %s", key, val);
	mame_legacy_options_set_string (main_gui.legacy_options, key, val);
}

static void
update_property_on_change_double (GtkWidget *widget, gpointer user_data)
{
	gchar *key;
	gdouble val;
	
	key = (gchar *) user_data;
	
	g_return_if_fail (key != NULL);
	
	GMAMEUI_DEBUG ("Callback received on widget for %s", key);
	val = get_property_value_as_double (widget, key);
	GMAMEUI_DEBUG ("Setting value of widget for %s to %.2f", key, val);
	mame_legacy_options_set_dbl (main_gui.legacy_options, key, val);
}

static GtkWidget *
get_widget_with_suffix (GladeXML *gxml, gchar *suffix)
{
	GList *widgets;
	GList *node;
	
	g_return_val_if_fail (gxml != NULL, NULL);
	g_return_val_if_fail (suffix != NULL, NULL);
	
	widgets = glade_xml_get_widget_prefix (gxml, "preferences_");
	node = widgets;

	while (node)
	{
		const gchar *name;
		GtkWidget *widget;
		
		widget = node->data;
		
		name = glade_get_widget_name (widget);

		if (g_str_has_suffix (name, suffix)) 
			return widget;

		node = g_list_next (node);
	}

	return NULL;
}

/* When a widget changes value, check to see whether there are any dependent
   widgets that need to have their state set correspondingly */
static void
parent_widget_clicked (GtkWidget *widget, gpointer user_data)
{
	GtkWidget *child;
	guint i;
	
	MameOptionsLegacyDialog *dlg;
	gchar *key;
	
	dlg = MAME_OPTIONS_LEGACY_DIALOG (g_object_get_data (G_OBJECT (widget), "MameOptionsLegacyDialog"));
	key = (gchar *) user_data;
		
	/* Process each of the widget relationships for the parent */
	for (i = 0; i < G_N_ELEMENTS (widget_relationships); i++) {

		if (strcmp (key, widget_relationships[i].parent) == 0) {
			child = get_widget_with_suffix (dlg->priv->xml, widget_relationships[i].dependent);
			
			/* Set the state of the children based on the value of the parent */
			gchar *parent_val = get_property_value_as_string (widget, key);

			if (strcmp (parent_val, widget_relationships[i].relationship) == 0)
				gtk_widget_set_sensitive (child, TRUE);
			else
				gtk_widget_set_sensitive (child, FALSE);
		}
	}
}

static void
register_callbacks (MameOptionsLegacyDialog *dlg, GtkWidget *widget, gchar *key)
{
	/* Set the dialog as data against the widget, so the callback can
	   use the dialog's priv struct, e.g. for the keyfile etc */
	g_object_set_data (G_OBJECT (widget), "MameOptionsLegacyDialog", dlg);
	
	GMAMEUI_DEBUG ("Registering callback for widget associated with %s", key);

	if (GTK_IS_TOGGLE_BUTTON (widget)) {
		g_signal_connect (G_OBJECT(widget), "toggled",
				  G_CALLBACK (update_property_on_change_int), key);
		g_signal_connect (G_OBJECT (widget), "toggled",
				  G_CALLBACK (parent_widget_clicked), key);
	} else if (GTK_IS_SPIN_BUTTON (widget)) {
		g_signal_connect (G_OBJECT(widget), "value-changed",
				  G_CALLBACK (update_property_on_change_int), key);
	} else if (GTK_IS_HSCALE (widget)) {
		g_signal_connect (G_OBJECT(widget), "value-changed",
				  G_CALLBACK (update_property_on_change_double), key);
	} else if (GTK_IS_COMBO_BOX (widget)) {
		g_signal_connect (G_OBJECT (widget), "changed",
				  G_CALLBACK (update_property_on_change_str), key);
		g_signal_connect (G_OBJECT (widget), "changed",
				  G_CALLBACK (parent_widget_clicked), key);
	}
}

static void
set_property_value_as_string (GtkWidget *widget, gchar *value)
{
	gint  int_value;
	gdouble double_value;
	gchar* values;
	gint i; 
	
	if (GTK_IS_TOGGLE_BUTTON (widget)) {
		if (value) 
			int_value = atoi (value);
		else
			int_value = 0;
		// AAA FIXME TODO HACK HACK HACK
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget),
			                      int_value);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget),
			                      !int_value);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget),
			                      int_value);
	} else if (GTK_IS_SPIN_BUTTON (widget)) {
		if (value) 
			int_value = atoi (value);
		else
			int_value = 0;
		
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (widget), int_value);
	} else if (GTK_IS_HSCALE (widget)) {
		if (value) 
			double_value = atof (value);
		else
			double_value = 0.0;
		
		gtk_range_set_value (GTK_RANGE (widget), double_value);
	} else if (GTK_IS_COMBO_BOX (widget)) {
		gchar **values_arr;
		
		values = g_object_get_data (G_OBJECT (widget), "untranslated");
		g_return_if_fail (values != NULL);
		
		values_arr = g_strsplit (values, ":", -1);
		if (value != NULL)
		{
			for (i=0; values_arr[i] != NULL; i++)
			{
				if (strcmp(value, values_arr[i]) == 0)
				{
					gtk_combo_box_set_active (GTK_COMBO_BOX (widget), i);
					break;
				}
			}
		}	
	} else if (GTK_IS_ENTRY (widget)) {
		if (value)
			gtk_entry_set_text (GTK_ENTRY (widget), value);
		else
			gtk_entry_set_text (GTK_ENTRY (widget), "");
	}

}

static void
connect_prop_to_object (MameOptionsLegacyDialog *dlg, GtkWidget *object, const gchar *key)
{
	guint i;
	int int_value;
	gchar *value;
	
	if (GTK_IS_TOGGLE_BUTTON (object))
	{	
		int_value = mame_legacy_options_get_bool (main_gui.legacy_options, key);
		value = g_strdup_printf ("%d", int_value);
		set_property_value_as_string (object, value);
	} else if (GTK_IS_SPIN_BUTTON (object)) {
		int_value = mame_legacy_options_get_int (main_gui.legacy_options, key);
		value = g_strdup_printf ("%d", int_value);
		set_property_value_as_string (object, value);
	} else if (GTK_IS_HSCALE (object)) {
		value = mame_legacy_options_get (main_gui.legacy_options, key);
		set_property_value_as_string (object, value);
		g_free (value);
	} else if (GTK_IS_COMBO_BOX (object)) {
		/* Find the associated internal entries (corresponding with the
		   display entries and link it to the widget */
		for (i = 0; i < G_N_ELEMENTS (combo_links); i++) {
			GMAMEUI_DEBUG ("Comparing %s against %s", combo_links[i].prop_name, key);
			if (g_ascii_strcasecmp (combo_links[i].prop_name, key) == 0) {
				g_object_set_data (G_OBJECT(object), "untranslated",
						   combo_links[i].recorded_values);
			}
		}
		
		value = mame_legacy_options_get (main_gui.legacy_options, key);
		set_property_value_as_string (object, value);
		g_free (value);
	} else
		GMAMEUI_DEBUG ("Connect_prop_to_object: Unhandled widget: %s", key);
}

/**
 * mame_options_register_property_raw:
 * @pr: a #MameOptions object
 * @object: Widget to register
 * @key: Property key
 * @default_value: Default value of the key
 * @flags: Flags
 * @object_type: Object type of widget
 * @data_type: Data type of the property
 *
 * This also registers only one widget, but instead of supplying the property
 * parameters as a single parsable string (as done in previous method), it
 * takes them separately.
 * 
 * Return value: TRUE if sucessful.
 */
static gboolean
mame_legacy_options_register_property_raw (MameOptionsLegacyDialog *dlg,
					   GtkWidget *object,
					   const gchar *key)
{
	g_return_val_if_fail (MAME_IS_OPTIONS_LEGACY_DIALOG (dlg), FALSE);
	g_return_val_if_fail (GTK_IS_WIDGET (object), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (strlen(key) > 0, FALSE);
	
	register_callbacks (dlg, object, key);

	connect_prop_to_object (dlg, object, key);
	
	return TRUE;
}

/**
 * mame_options_register_property_from_string:
 * @pr: a #MameOptions object
 * @object: Widget to register
 * @property_desc: Property description (in form #Category.#Key)
 *
 * This registers only one widget. The widget could be shown elsewhere.
 * the property_description should be of the form described before.
 * 
 * Return value: TRUE if sucessful.
 */
static gboolean
mame_legacy_options_register_property_from_string (MameOptionsLegacyDialog *dlg,
						   GtkWidget *widget,
						   const gchar *property_desc)
{
	gchar *key;
	MameExec *exec;
	
	g_return_val_if_fail (MAME_IS_OPTIONS_LEGACY_DIALOG(dlg), FALSE);
	g_return_val_if_fail ((GTK_IS_WIDGET (widget)), FALSE);
	g_return_val_if_fail (property_desc != NULL, FALSE);
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	g_return_val_if_fail (exec != NULL, FALSE);
	
	key = g_strdup (property_desc);	

	/* Disable the widget and update the tooltip if this option is not
	   supported by MAME */
	if (!g_str_has_suffix (key, "_toggle")) {       /* Exclude non-options */
		gchar *key_field;
		key_field = get_key_field (key);

		if (!mame_has_option (exec, key_field)) {
			gchar *text;
			
			GMAMEUI_DEBUG ("Option %s not supported by this version of MAME", key_field);
		
			gtk_widget_set_sensitive (widget, FALSE);
			text = g_strdup_printf (_("The option %s is not supported by MAME %s"),
						key_field, mame_exec_get_version (exec));
			gtk_widget_set_tooltip_text (widget, text);
			g_free (text);
		}
		g_free (key_field);
	}
	
	mame_legacy_options_register_property_raw (dlg, widget, key);

	//g_free (key); // FIXME TODO Free'ing this segfaults in the callbacks for the various widgets
	
	return TRUE;
}

/**
 * mame_options_register_all_properties_from_glade_xml:
 * @pr: a #MameOptions Object
 * @gxml: GladeXML object containing the properties widgets.
 * @parent: Parent widget in the gxml object
 *
 * This will register all the properties names of the format described above
 * without considering the UI. Useful if you have the widgets shown elsewhere
 * but you want them to be part of preferences system.
 */
static void
mame_legacy_options_dialog_register_all_properties_from_glade_xml (MameOptionsLegacyDialog *dlg, GtkWidget *parent)
{
	GList *widgets;
	GList *node;
	
	g_return_if_fail (MAME_IS_OPTIONS_LEGACY_DIALOG (dlg));
	g_return_if_fail (dlg->priv->xml != NULL);
	
	widgets = glade_xml_get_widget_prefix (dlg->priv->xml, "preferences_");
	node = widgets;

	while (node)
	{
		const gchar *name;
		GtkWidget *widget, *p;
		gboolean cont_flag = FALSE;
		
		widget = node->data;
		
		p = gtk_widget_get_parent (widget);
		/* Added only if it's a descendent child of the parent */
		while (p != parent)
		{
			if (p == NULL)
			{
				cont_flag = TRUE;
				break;
			}
			p = gtk_widget_get_parent (p);
		}
		if (cont_flag == TRUE)
		{
			node = g_list_next (node);
			continue;
		}
		
		name = glade_get_widget_name (widget);

		if (strncmp (name, PREFERENCE_PROPERTY_PREFIX,
                     strlen (PREFERENCE_PROPERTY_PREFIX)) == 0)
		{
			const gchar *property = &name[strlen (PREFERENCE_PROPERTY_PREFIX)];
			mame_legacy_options_register_property_from_string (dlg, widget, property);
		}
		node = g_list_next (node);
	}
}

/* Function to create the sidebar */
static void
add_category_columns (MameOptionsLegacyDialog *dlg)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	renderer = gmameui_cell_renderer_captioned_image_new ();
	g_object_ref_sink (renderer);
	column = gtk_tree_view_column_new_with_attributes (_("Category"),
							   renderer,
							   "text",
							   COL_TITLE,
							   "pixbuf",
							   COL_PIXBUF,
							   NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (dlg->priv->treeview),
				     column);
	

}

/* When an item in the sidebar is selected, we change the tab
   to the appropriate section */
static void
selection_changed_cb (GtkTreeSelection *selection,
		      MameOptionsLegacyDialog *dlg)
{
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		GtkWidget *widget;

		gtk_tree_model_get (GTK_TREE_MODEL (dlg->priv->store), &iter,
				    COL_WIDGET, &widget, -1);
		
		gtk_notebook_set_current_page 
			(GTK_NOTEBOOK (dlg->priv->notebook),
			 gtk_notebook_page_num (GTK_NOTEBOOK (dlg->priv->notebook),
						widget));
		
	}
}

/* This function fudges the order that items are added to the store. If we
   didn't have this, then the sort order would be alphabetic */
static gint
compare_pref_page_func (GtkTreeModel *model,
			GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
	gint val;
	gchar *name1, *name2;
	
	gtk_tree_model_get (model, a, COL_TITLE, &name1, -1);
	gtk_tree_model_get (model, b, COL_TITLE, &name2, -1);

	/* FIXME: Make the debugging page last */
	if (strcmp (name1, _("Debugging")) != 0)
		return -1;
		
	if (strcmp (name2, _("Debugging")) == 0)
		return 1;

	val = strcmp (name1, name2);
	g_free (name1);
	g_free (name2);
	
	return val;
}

/* Creates a hbox containing a treeview (the sidebar) and a notebook */
static void
mame_options_legacy_dialog_init (MameOptionsLegacyDialog *dlg)
{
	/* Widget declarations */
	GtkWidget *hbox;
	GtkWidget *scrolled_window;
	GtkWidget *btn_close;
	GtkTreeSelection *selection;
	GtkTreeSortable *sortable;

	MameOptionsLegacyDialogPrivate *priv;

	priv = G_TYPE_INSTANCE_GET_PRIVATE (dlg,
					    MAME_TYPE_OPTIONS_LEGACY_DIALOG,
					    MameOptionsLegacyDialogPrivate);
	
	dlg->priv = priv;

	priv->xml = glade_xml_new (GLADEDIR "options_legacy.glade", NULL, GETTEXT_PACKAGE);
	if (!priv->xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "options_legacy.glade");
		return;
	}
	
	/* Now set up the dialog */
	hbox = gtk_hbox_new (FALSE, 0);	
	
	dlg->priv->treeview = gtk_tree_view_new ();
	gtk_widget_show (dlg->priv->treeview);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (dlg->priv->treeview),
					   FALSE);
	dlg->priv->store = gtk_list_store_new (LAST_COL, 
					       G_TYPE_STRING,
					       G_TYPE_STRING,
					       GDK_TYPE_PIXBUF,
					       GTK_TYPE_WIDGET,
					       G_TYPE_INT);
	sortable = GTK_TREE_SORTABLE (dlg->priv->store);
	gtk_tree_sortable_set_sort_column_id (sortable, COL_TITLE,
					      GTK_SORT_ASCENDING);
	gtk_tree_sortable_set_sort_func (sortable, COL_TITLE,
					 compare_pref_page_func,
					 NULL, NULL);
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (dlg->priv->treeview),
				 GTK_TREE_MODEL (dlg->priv->store));

	add_category_columns (dlg);

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
					GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type 
		(GTK_SCROLLED_WINDOW (scrolled_window), GTK_SHADOW_IN);
	
	gtk_widget_show (scrolled_window);
	gtk_container_add (GTK_CONTAINER (scrolled_window), 
			   dlg->priv->treeview);
	gtk_box_pack_start (GTK_BOX (hbox), scrolled_window,
			    FALSE, FALSE, 0);

	dlg->priv->notebook = gtk_notebook_new ();
	gtk_widget_show (dlg->priv->notebook);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (dlg->priv->notebook), FALSE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (dlg->priv->notebook), 
				      FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->notebook),
					8);
	
	gtk_box_pack_start (GTK_BOX (hbox), dlg->priv->notebook,
			    TRUE, TRUE, 0);

	selection = gtk_tree_view_get_selection 
		(GTK_TREE_VIEW (dlg->priv->treeview));

	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);

	g_signal_connect (selection, "changed", 
			  G_CALLBACK (selection_changed_cb), dlg);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox), hbox,
			    TRUE, TRUE, 6);
	
	btn_close = gtk_dialog_add_button (GTK_DIALOG (dlg), GTK_STOCK_CLOSE, -7);
	gtk_widget_grab_default (btn_close);
	
	/* Add each of the notebooks */
	mame_options_legacy_dialog_add_page (dlg, "DisplayNotebook", _("Display"), "gmameui-display-toolbar");
/*	mame_options_legacy_dialog_add_page (dlg, "OpenGLVBox", _("OpenGL"), "gmameui-display-toolbar");*/
	mame_options_legacy_dialog_add_page (dlg, "SoundVBox", _("Sound"), "gmameui-sound-toolbar");
	mame_options_legacy_dialog_add_page (dlg, "InputVBox", _("Input"), "gmameui-joystick-toolbar");
//	mame_options_legacy_dialog_add_page (dlg, "PerformanceVBox", _("Performance"), "gmameui-general-toolbar");
	mame_options_legacy_dialog_add_page (dlg, "MiscVBox", _("Miscellaneous"), "gmameui-general-toolbar");
//	mame_options_legacy_dialog_add_page (dlg, "DebuggingVBox", _("Debugging"), "gmameui-rom");
	
	gtk_widget_show (hbox);
}

GtkWidget *
mame_options_legacy_dialog_new (void)
{
	return g_object_new (MAME_TYPE_OPTIONS_LEGACY_DIALOG, 
				 "title", _("MAME Legacy Options"),
				 NULL);
}

static void
mame_options_legacy_dialog_response (GtkDialog *dialog, gint response)
{
	MameOptionsLegacyDialogPrivate *priv;
	
	priv = G_TYPE_INSTANCE_GET_PRIVATE (MAME_OPTIONS_LEGACY_DIALOG (dialog),
					    MAME_TYPE_OPTIONS_LEGACY_DIALOG,
					    MameOptionsLegacyDialogPrivate);
	
	switch (response)
	{
		case GTK_RESPONSE_CLOSE:
			/* Close button clicked */
			gtk_widget_destroy (GTK_WIDGET (dialog));

			break;
		case GTK_RESPONSE_DELETE_EVENT:
			/* Dialog closed */		
			gtk_widget_destroy (GTK_WIDGET (dialog));

			break;
		default:
			g_assert_not_reached ();
	}
}

static void
mame_options_legacy_dialog_destroy (GtkObject *object)
{
	MameOptionsLegacyDialog *dlg;
	
GMAMEUI_DEBUG ("Destroying mame legacy options dialog...");	
	dlg = MAME_OPTIONS_LEGACY_DIALOG (object);
	
	if (dlg->priv->xml)
		g_object_unref (dlg->priv->xml);
	
	if (dlg->priv->store) {
		g_object_unref (dlg->priv->store);
		dlg->priv->store = NULL;
	}
	
	g_object_unref (dlg->priv);
	
/*	GTK_OBJECT_CLASS (mame_options_legacy_dialog_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying mame legacy options dialog... done");
}

static void
mame_options_legacy_dialog_add_page (MameOptionsLegacyDialog *dlg, const gchar *widget_name,
			      const gchar *title, const gchar *icon_filename)
{
	GtkTreeIter iter;
	GtkTreeSelection* selection;
	GtkTreeIter first;
	GtkWidget *parent;
	GtkWidget *page;
	GdkPixbuf *icon;
	
	g_return_if_fail (dlg != NULL);
	g_return_if_fail (dlg->priv->xml != NULL);
	g_return_if_fail (dlg->priv->notebook != NULL);
	g_return_if_fail (dlg->priv->store != NULL);
	
	page = glade_xml_get_widget (dlg->priv->xml, widget_name);
	g_object_ref (page);
	
	g_return_if_fail (GTK_IS_WIDGET (page));
	
	gtk_widget_show (page);
	
	parent = gtk_widget_get_parent (page);
	if (parent && GTK_IS_CONTAINER (parent))
	{
		if (GTK_IS_NOTEBOOK (parent)) {
			gint page_num;
			
			page_num = gtk_notebook_page_num (GTK_NOTEBOOK (parent), page);
			gtk_notebook_remove_page (GTK_NOTEBOOK (parent), page_num);
		} else {
			gtk_container_remove (GTK_CONTAINER (parent), page);
		}
	}
	GMAMEUI_DEBUG ("Appending page to notebook for %s...", widget_name);
	gtk_notebook_append_page (GTK_NOTEBOOK (dlg->priv->notebook), page, NULL);
	GMAMEUI_DEBUG ("Appending page to notebook for %s... done", widget_name);

	icon = gmameui_get_icon_from_stock (icon_filename);
	
	gtk_list_store_append (dlg->priv->store, &iter);
	gtk_list_store_set (dlg->priv->store, &iter,
			    COL_NAME, widget_name,
			    COL_TITLE, _(title),
			    COL_PIXBUF, icon,
			    COL_WIDGET, page,
			    -1);
	
	gtk_tree_model_get_iter_first (GTK_TREE_MODEL (dlg->priv->store), &first);
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dlg->priv->treeview));
	gtk_tree_selection_select_iter (selection, &first);
	
	mame_legacy_options_dialog_register_all_properties_from_glade_xml (dlg, page);
	
	g_object_unref (icon);
}
