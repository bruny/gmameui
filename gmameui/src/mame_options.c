/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code for AnjutaPreferences object
 * Copyright (C) 2000 - 2003  Naba Kumar  <naba@gnome.org>
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

#include <gtk/gtk.h>
#include <glade/glade.h>

#include "mame_options.h"
#include "io.h"
#include "common.h"
#include "xmame_executable.h"

struct _MameOptionValue
{
	gchar *key;
	gchar *value;
	GmameuiPropertyDataType    type;
};

struct _MameProperty
{
	GtkWidget    *object;
	gchar        *key;
	gchar        *default_value;
	guint        flags;
	gint         notify_id;
	
	/* Set true if custom set/get to be used */
	gboolean     custom;
	
	/* For inbuilt generic objects */
	GmameuiPropertyObjectType  object_type;
	GmameuiPropertyDataType    data_type;
	
	/* For custom objects */
	void    (*set_property) (MameProperty *prop, const gchar *value);
	gchar * (*get_property) (MameProperty *prop);
	
};

struct _MameOptionsPriv
{
	GKeyFile *options_file;
	gchar *filename;
	GHashTable          *properties;
	GList				*options_list;  /* A list of the set options, of type MameOptionValue */
	GtkWidget           *prefs_dialog;
	gboolean             is_showing;
};

#define PREFERENCE_PROPERTY_PREFIX "preferences_"

/* Returns an options string for a particular category (e.g. Performance, Video, etc)
   String needs to be free'd when finished
 */
gchar *
mame_options_get_option_string (MameOptions *pr, gchar *category)
{
	gchar *options_string;
	gchar **keylist;
	gsize category_len;
	GError *error = NULL;
	MameProperty *prop;
	int i;

	g_return_if_fail (pr->priv->options_file != NULL);
	g_return_if_fail (g_hash_table_size (pr->priv->properties) != 0);
	
	options_string = g_strdup ("");

	keylist = g_key_file_get_keys (pr->priv->options_file, category, &category_len, &error);
	
	if (error) {
		GMAMEUI_DEBUG ("Error retrieving options keys for category %s: %s",
			       category, error->message);
		g_error_free (error);
		return options_string;
	}
	
	GMAMEUI_DEBUG ("Retrieving option keys for category %s - %d keys found", category, category_len);
	
	for (i = 0; i < category_len; i++) {
		gchar *value;
		gchar *key;
		
		key = g_strdup_printf("%s.%s", category, keylist[i]);
		prop = g_hash_table_lookup (pr->priv->properties, key);
		
		if (!prop) {
			GMAMEUI_DEBUG ("No property found for key %s", key);
		} else {
		
		error = NULL;
		
		value = g_key_file_get_value (pr->priv->options_file, category, keylist[i], &error);
		if (error) {
			GMAMEUI_DEBUG ("Error retrieving options value for key %s in category %s: %s",
				       keylist[i], category, error->message);
			g_error_free (error);
			break;
		}
		
		GMAMEUI_DEBUG ("Option %s has type %d and value %s", keylist[i], prop->data_type, value);
		switch (prop->data_type) {
			case GMAMEUI_PROPERTY_DATA_TYPE_BOOL:
				if (g_ascii_strcasecmp (value, "1") == 0) {
					GMAMEUI_DEBUG("Adding option %s", keylist[i]);
					options_string = g_strconcat (options_string, " -", keylist[i], NULL);
				} else
					GMAMEUI_DEBUG("Ignoring option %s", keylist[i]);
				break;
			case GMAMEUI_PROPERTY_DATA_TYPE_INT:
				options_string = g_strconcat (options_string, " -", keylist[i], " ", value, NULL);
				break;
			case GMAMEUI_PROPERTY_DATA_TYPE_DOUBLE:
				options_string = g_strconcat (options_string, " -", keylist[i], " ", value, NULL);
				break;
			case GMAMEUI_PROPERTY_DATA_TYPE_TEXT:
				/* Don't add empty strings */
				if (g_ascii_strcasecmp (value, "") != 0)
					options_string = g_strconcat (options_string, " -", keylist[i], " ", value, NULL);
				break;
		}
		g_free (value);}
		g_free (key);
	}

	if (keylist)
		g_strfreev (keylist);

	GMAMEUI_DEBUG ("Options string is %s", options_string);
	return options_string;
}

gchar *
mame_options_get (MameOptions *pr, const gchar *key)
{
	gchar *ret_val;
	gchar **stv;
	GError *error = NULL;
	
	g_return_val_if_fail (MAME_IS_OPTIONS (pr), NULL);
	g_return_val_if_fail (key != NULL, NULL);
	
	ret_val = 0;
	stv = g_strsplit (key, ".", 0);
	ret_val = g_key_file_get_string (pr->priv->options_file, stv[0], stv[1], &error);
	GMAMEUI_DEBUG ("Getting settings for %s - %s. Value is %s", stv[0], stv[1], ret_val);
	g_strfreev (stv);
	
	return ret_val;
}

void
mame_options_set (MameOptions *pr, const gchar *key,
						const gchar *value)
{
	gchar **stv;
	
	g_return_if_fail (MAME_IS_OPTIONS (pr));
	g_return_if_fail (key != NULL);

	stv = g_strsplit (key, ".", 0);
	
	if (value && (strlen (value) > 0))
	{
		g_key_file_set_string (pr->priv->options_file, stv[0], stv[1], value);
		g_key_file_save_to_file (pr->priv->options_file, pr->priv->filename, NULL);
		
		/* Update option in list */
		add_option_to_option_list (pr, key, value);
	}
	g_strfreev (stv);
}



/* All the widgets that cannot be handled via the GladeXML naming scheme are
   managed here, e.g. effect name, which is determined by .png files in artwork dir */
mame_options_generate_custom_settings (MameOptions *pr)
{
	/*FIXME TODO
	* Get the GladeXML file *
	get the widget
	add the entries*/
	GtkComboBox *box = gtk_combo_box_new_text ();   // FIXME TODO Should be a widget attribute in priv
	gtk_combo_box_append_text (box, "aperture1x2rb");
	gtk_combo_box_append_text (box, "aperture1x3rb");
	gtk_combo_box_append_text (box, "aperture2x4bg");
	gtk_combo_box_append_text (box, "aperture2x4rb");
	gtk_combo_box_append_text (box, "aperture4x6");
	gtk_combo_box_append_text (box, "scanlines");
	gtk_widget_show (GTK_WIDGET (box));

	/* Set the signal - in the handler, get the value using
	   gtk_combo_box_get_active_text (); */
	
}

static void 
on_preferences_dialog_destroyed (GtkWidget *preferencess_dialog,
							     MameOptions *pr)
{
	g_object_unref (pr->priv->prefs_dialog);
	
	pr->priv->prefs_dialog = NULL;
}

GtkWidget *
mame_options_get_dialog (MameOptions *pr)
{
	GList *plugins;

	if (pr->priv->prefs_dialog)
		return pr->priv->prefs_dialog;
	else
	{
		pr->priv->prefs_dialog = mame_options_dialog_new ();
		
		g_signal_connect (G_OBJECT (pr->priv->prefs_dialog), "destroy",
						  G_CALLBACK (on_preferences_dialog_destroyed),
						  pr);
		
		return g_object_ref_sink (pr->priv->prefs_dialog);
	}
}

static void mame_options_class_init    (MameOptionsClass *class);
static void mame_options_instance_init (MameOptions *pr);

GType
mame_options_get_type ()
{
	static GType obj_type = 0;
	
	if (!obj_type)
	{
		static const GTypeInfo obj_info = 
		{
			sizeof (MameOptionsClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mame_options_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,           /* class_data */
			sizeof (MameOptionsClass),
			0,              /* n_preallocs */
			(GInstanceInitFunc) mame_options_instance_init,
			NULL            /* value_table */
		};
		obj_type = g_type_register_static (G_TYPE_OBJECT,
		                                   "MameOptions", &obj_info, 0);
	}
	return obj_type;
}

/**
 * mame_options_new:
 * 
 * Creates a new #MameOptions object
 * 
 * Return value: A #MameOptions object.
 */
MameOptions *
mame_options_new ()
{
	/* TODO Pass in object RomEntry - if null, then its default options */
	MameOptions *pr;
	
	pr = g_object_new (MAME_TYPE_OPTIONS, NULL);
	
	return pr;
	
}

gint
mame_options_get_int (MameOptions *pr, const gchar *key)
{
	gint ret_val;
	gchar** stv;
	GError *error = NULL;

	g_return_val_if_fail (MAME_IS_OPTIONS (pr), 0);
	g_return_val_if_fail (key != NULL, 0);
	g_return_val_if_fail (pr->priv->options_file != NULL, 0);

	ret_val = 0;
	stv = g_strsplit (key, ".", 0);
	ret_val = g_key_file_get_integer (pr->priv->options_file, stv[0], stv[1], &error);

	g_strfreev (stv);

	if (error) {
		GMAMEUI_DEBUG ("Error retrieving int option %s: %s", key, error->message);
		g_error_free (error);
		error = NULL;
	}
	
	return ret_val;
}

void
mame_options_set_int (MameOptions *pr, const gchar *key,
							gint value)
{
	gchar** stv;

	g_return_if_fail (MAME_IS_OPTIONS (pr));
	g_return_if_fail (key != NULL);
	
	stv = g_strsplit (key, ".", 0);
	g_key_file_set_integer (pr->priv->options_file, stv[0], stv[1], value);
	
	g_strfreev (stv);
	
	g_key_file_save_to_file (pr->priv->options_file, pr->priv->filename, NULL);
	
	gchar *val = g_strdup_printf("%i", value);
	add_option_to_option_list (pr, key, val);
	g_free (val);
}

void
mame_options_set_double (MameOptions *pr, const gchar *key,
							gdouble value)
{
	gchar** stv;

	g_return_if_fail (MAME_IS_OPTIONS (pr));
	g_return_if_fail (key != NULL);
	
	stv = g_strsplit (key, ".", 0);
	g_key_file_set_double (pr->priv->options_file, stv[0], stv[1], value);
	
	g_strfreev (stv);
	
	g_key_file_save_to_file (pr->priv->options_file, pr->priv->filename, NULL);
	
	gchar *val = g_strdup_printf("%f", value);
	add_option_to_option_list (pr, key, val);
	g_free (val);
}

static GmameuiPropertyObjectType
get_object_type_from_string (const gchar* object_type)
{
	if (strcmp (object_type, "entry") == 0)
		return GMAMEUI_PROPERTY_OBJECT_TYPE_ENTRY;
	else if (strcmp (object_type, "combo") == 0)
		return GMAMEUI_PROPERTY_OBJECT_TYPE_COMBO;
	else if (strcmp (object_type, "spin") == 0)
		return GMAMEUI_PROPERTY_OBJECT_TYPE_SPIN;
	else if (strcmp (object_type, "toggle") == 0)
		return GMAMEUI_PROPERTY_OBJECT_TYPE_TOGGLE;
	else if (strcmp (object_type, "text") == 0)
		return GMAMEUI_PROPERTY_OBJECT_TYPE_TEXT;
	else if (strcmp (object_type, "hscale") == 0)
		return GMAMEUI_PROPERTY_OBJECT_TYPE_HSCALE;
/*
	else if (strcmp (object_type, "file") == 0)
		return GMAMEUI_PROPERTY_OBJECT_TYPE_FILE;*/
	else if (strcmp (object_type, "folder") == 0)
		return GMAMEUI_PROPERTY_OBJECT_TYPE_FOLDER;
	else
		return (GmameuiPropertyObjectType)(-1);
}

static GmameuiPropertyDataType
get_data_type_from_string (const gchar* data_type)
{
	if (strcmp (data_type, "bool") == 0)
		return GMAMEUI_PROPERTY_DATA_TYPE_BOOL;
	else if (strcmp (data_type, "int") == 0)
		return GMAMEUI_PROPERTY_DATA_TYPE_INT;
	else if (strcmp (data_type, "text") == 0)
		return GMAMEUI_PROPERTY_DATA_TYPE_TEXT;
	else if (strcmp (data_type, "double") == 0)
		return GMAMEUI_PROPERTY_DATA_TYPE_DOUBLE;
	else
		return (GmameuiPropertyDataType)(-1);
}

static void
unregister_preferences_key (GtkWidget *widget,
							gpointer user_data)
{
	MameProperty *p;
	MameOptions *pr;
	gchar *key;
	
	p = (MameProperty *) user_data;
	pr = g_object_get_data (G_OBJECT (widget),
							"MameOptions");
	key = g_strdup (p->key);
	
// NEED TO ACCESS THE TYPE LATER	g_hash_table_remove (pr->priv->properties, key);
	g_free (key);
}

static gchar*
get_property_value_as_string (MameProperty *prop)
{
	gint  int_value;
	gdouble double_value;
	gchar** values;
	gchar *text_value = NULL;

	if (prop->custom)
	{
		if (prop->get_property != NULL)
			return prop->get_property (prop);
		else
		{
			g_warning ("%s: Undefined get_property() for custom object",
					   prop->key);
			return NULL;
		}
	}
	switch (prop->object_type)
	{
	case GMAMEUI_PROPERTY_OBJECT_TYPE_TOGGLE:
		int_value =
			gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (prop->object));
		text_value = g_strdup_printf ("%d", int_value);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_HSCALE:
		double_value = gtk_range_get_value (GTK_RANGE (prop->object));
		text_value = g_strdup_printf ("%f", double_value);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_SPIN:
		int_value =
			gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (prop->object));
		text_value = g_strdup_printf ("%d", int_value);
		break;

	case GMAMEUI_PROPERTY_OBJECT_TYPE_ENTRY:
		text_value =
			gtk_editable_get_chars (GTK_EDITABLE (prop->object), 0, -1);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_COMBO:
		{
			gint idx;
			values = g_object_get_data(G_OBJECT(prop->object), "untranslated");
			idx = gtk_combo_box_get_active(GTK_COMBO_BOX(prop->object));
			if (values[idx] != NULL)
				text_value = g_strdup(values[idx]);
			break;
		}
/*
	case GMAMEUI_PROPERTY_OBJECT_TYPE_FOLDER:
		text_value = gtk_file_chooser_get_current_folder (
				GTK_FILE_CHOOSER (prop->object));
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_FILE:
		text_value = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (prop->object));
			 break;*/
	}
	if (text_value && (strlen (text_value) == 0))
	{
		g_free (text_value);
		text_value = NULL;
	}
	return text_value;
}

static gint
get_property_value_as_int (MameProperty *prop)
{
	gint  int_value;
	gchar *text_value;
	text_value = get_property_value_as_string (prop);
	int_value = atoi (text_value);
	g_free (text_value);	
	return int_value;
}		

static gdouble
get_property_value_as_double (MameProperty *prop)
{
GMAMEUI_DEBUG ("Getting property double");
	gdouble  double_value;
	gchar *text_value;
	text_value = get_property_value_as_string (prop);
	double_value = atof (text_value);
	g_free (text_value);	
	return double_value;
}	

static void
update_property_on_change_int (GtkWidget *widget, gpointer user_data)
{
	MameOptions *pr;
	MameProperty *p;
	gint val;
	
	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget),
												"MameOptions"));
	p = (MameProperty *) user_data;
	val = get_property_value_as_int (p);
	mame_options_set_int (pr, p->key, val);
}

static void
update_property_on_change_double (GtkWidget *widget, gpointer user_data)
{
	MameOptions *pr;
	MameProperty *p;
	gdouble val;
	
	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget),
												"MameOptions"));
	p = (MameProperty *) user_data;
	val = get_property_value_as_double (p);

	mame_options_set_double (pr, p->key, val);
}

static gboolean
update_property_on_event_str (GtkWidget *widget, GdkEvent *event,
							  gpointer user_data)
{
	MameOptions *pr;
	MameProperty *p;
	gchar *val;
	
	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget),
										  "MameOptions"));
	p = (MameProperty *) user_data;
	val = get_property_value_as_string (p);
	mame_options_set (pr, p->key, val);
	g_free (val);
	return FALSE;
}

static void
update_property_on_change_str (GtkWidget *widget, gpointer user_data)
{
	MameOptions *pr;
	MameProperty *p;
	gchar *val;

	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget),
										  "MameOptions"));
	p = (MameProperty *) user_data;
	val = get_property_value_as_string (p);
	mame_options_set (pr, p->key, val);
	g_free (val);
}

static gboolean
block_update_property_on_change_str (GtkWidget *widget, GdkEvent *event,
							  gpointer user_data)
{
	MameProperty *p = (MameProperty *) user_data;

	gtk_signal_handler_block_by_func (GTK_OBJECT(p->object), G_CALLBACK (update_property_on_change_str), p);
	return FALSE;
}

static gboolean
unblock_update_property_on_change_str (GtkWidget *widget, GdkEvent *event,
							  gpointer user_data)
{
	MameProperty *p = (MameProperty *) user_data;

	gtk_signal_handler_unblock_by_func (GTK_OBJECT(p->object), G_CALLBACK (update_property_on_change_str), p);
	return FALSE;
}



static void
set_property_value_as_string (MameProperty *prop, const gchar *value)
{
	gint  int_value;
	gdouble double_value;
	char** values;
	gint i; 
	
	if (prop->custom)
	{
		if (prop->set_property != NULL)
		{
			prop->set_property (prop, value);
			return;
		}
		else
		{
			g_warning ("%s: Undefined set_property() for custom object",
					   prop->key);
			return;
		}
	}
	switch (prop->object_type)
	{
	case GMAMEUI_PROPERTY_OBJECT_TYPE_TOGGLE:
		if (value) 
			int_value = atoi (value);
		else
			int_value = 0;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (prop->object),
		                              int_value);
		break;
		
	case GMAMEUI_PROPERTY_OBJECT_TYPE_SPIN:
		if (value) 
			int_value = atoi (value);
		else
			int_value = 0;
		
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (prop->object), int_value);
		break;
	
	case GMAMEUI_PROPERTY_OBJECT_TYPE_ENTRY:
		if (value)
			gtk_entry_set_text (GTK_ENTRY (prop->object), value);
		else
			gtk_entry_set_text (GTK_ENTRY (prop->object), "");
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_COMBO:
/* FIXME TODO This is where we add the strings to the combo */
		values = g_object_get_data(G_OBJECT(prop->object), "untranslated");
		if (value != NULL)
		{
			for (i=0; values[i] != NULL; i++)
			{
				if (strcmp(value, values[i]) == 0)
				{
					gtk_combo_box_set_active(GTK_COMBO_BOX(prop->object), i);
					break;
				}
			}
		}			
		break;		
	case GMAMEUI_PROPERTY_OBJECT_TYPE_TEXT:
		{
			GtkTextBuffer *buffer;
			buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (prop->object));
			if (value)
				gtk_text_buffer_set_text (buffer, value, -1);
			else
				gtk_text_buffer_set_text (buffer, "", -1);
		}
		break;

	case GMAMEUI_PROPERTY_OBJECT_TYPE_HSCALE:
		if (value) 
			double_value = atof (value);
		else
			double_value = 0.0;
		
		gtk_range_set_value (GTK_RANGE (prop->object), double_value);
		break;
/*
	case GMAMEUI_PROPERTY_OBJECT_TYPE_FOLDER:
		if (value)
			gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (prop->object), value);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_FILE:
		if (value)
		{
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (prop->object),
															 value);
		}
		break;*/
	}
}

static void
register_callbacks (MameOptions *pr, MameProperty *p)
{
	gchar *key_error_msg;
	
	g_object_set_data (G_OBJECT (p->object), "MameOptions", pr);
	switch (p->object_type) {
		case GMAMEUI_PROPERTY_OBJECT_TYPE_ENTRY:
			g_signal_connect (G_OBJECT(p->object), "changed",
							  G_CALLBACK (update_property_on_change_str), p);
			g_signal_connect (G_OBJECT(p->object), "focus_out_event",
							  G_CALLBACK (update_property_on_event_str), p);
			g_signal_connect (G_OBJECT(p->object), "focus_out_event",
							  G_CALLBACK (unblock_update_property_on_change_str), p);
			g_signal_connect (G_OBJECT(p->object), "focus_in_event",
							  G_CALLBACK (block_update_property_on_change_str), p);
			break;
		case GMAMEUI_PROPERTY_OBJECT_TYPE_SPIN:
			g_signal_connect (G_OBJECT(p->object), "value-changed",
							  G_CALLBACK (update_property_on_change_int), p);
			break;
		case GMAMEUI_PROPERTY_OBJECT_TYPE_TEXT:
			g_signal_connect (G_OBJECT(p->object), "focus_out_event",
							  G_CALLBACK (update_property_on_event_str), p);
			break;
		case GMAMEUI_PROPERTY_OBJECT_TYPE_COMBO:
			g_signal_connect (G_OBJECT(p->object), "changed",
							  G_CALLBACK (update_property_on_change_str), p);
			break;
		case GMAMEUI_PROPERTY_OBJECT_TYPE_TOGGLE:
			g_signal_connect (G_OBJECT(p->object), "toggled",
							  G_CALLBACK (update_property_on_change_int), p);
			break;
		case GMAMEUI_PROPERTY_OBJECT_TYPE_HSCALE:
			g_signal_connect (G_OBJECT(p->object), "value-changed",
							  G_CALLBACK (update_property_on_change_double), p);
			break;
/*	
		case GMAMEUI_PROPERTY_OBJECT_TYPE_FILE:
			g_signal_connect (G_OBJECT(p->object), "file-set",
							  G_CALLBACK (update_property_on_change_str), p);
			break;
		case GMAMEUI_PROPERTY_OBJECT_TYPE_FOLDER:
			g_signal_connect (G_OBJECT(p->object), "current-folder-changed",
							  G_CALLBACK (update_property_on_change_str), p);
			break;*/
		default:
			break;
	}

	/* Connect to widget destroy signal so we can automatically unregister
	 * keys so there aren't any potential conflicts or references to 
	 * nonexistent widgets on subsequent uses of the prefs dialog. */
	g_signal_connect (G_OBJECT (p->object), "destroy",
					  G_CALLBACK (unregister_preferences_key),
					  p);

}

void add_option_to_option_list (MameOptions *pr, gchar *key, gchar *value)
{
	MameOptionValue *obj;
	GList *listpointer;
	gboolean found = FALSE;
	
	/* Look for option in list of available options for this exec */

	for (listpointer = g_list_first (pr->priv->options_list);
		 listpointer != NULL;
		 listpointer = g_list_next (listpointer)) {
		
		if (g_ascii_strcasecmp (((MameOptionValue *) (listpointer->data))->key, key) == 0) {
			GMAMEUI_DEBUG ("Updating option %s", ((MameOptionValue *) (listpointer->data))->key);
			((MameOptionValue *) listpointer->data)->value = g_strdup (value);
			found = TRUE;
			break;
		}
	}
	
	/* If the option was not found, then create a new one */
	if (!found) {
		obj = g_new0 (MameOptionValue, 1);
	
		obj->key = g_strdup (key);
		obj->value = g_strdup (value);

		GMAMEUI_DEBUG ("Adding new option %s with value %s", key, value);
		/* Prepend to avoid inefficiencies with adding to end of list */
		pr->priv->options_list = g_list_prepend (pr->priv->options_list, obj);
	}
}

static void
connect_prop_to_object (MameOptions *pr, MameProperty *p)
{
	int gconf_value;
	gchar *value;
	
	if (p->data_type == GMAMEUI_PROPERTY_DATA_TYPE_BOOL ||
		p->data_type == GMAMEUI_PROPERTY_DATA_TYPE_INT)
	{	
		gconf_value = mame_options_get_int (pr, p->key);
		value = g_strdup_printf ("%d", gconf_value);
		set_property_value_as_string (p, value);
/* Add the value to the list of options */
add_option_to_option_list (pr, p->key, value);
	}
	else 
	{
		value = mame_options_get (pr, p->key);
		set_property_value_as_string (p, value);
/* Add the value to the list of options */
add_option_to_option_list (pr, p->key, value);
		g_free (value);
	}
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
gboolean
mame_options_register_property_raw (MameOptions *pr,
									GtkWidget *object,
									const gchar *key,
									const gchar *default_value,
									guint flags,
									GmameuiPropertyObjectType object_type,
									GmameuiPropertyDataType  data_type)
{
	MameProperty *p;
	
	g_return_val_if_fail (MAME_IS_OPTIONS (pr), FALSE);
	g_return_val_if_fail (GTK_IS_WIDGET (object), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (strlen(key) > 0, FALSE);
	
	p = g_new0 (MameProperty, 1);
	g_object_ref (object);
	p->object = object;
	p->object_type = object_type;
	p->data_type = data_type;
	p->key = g_strdup (key);
	
	if (default_value)
	{
		p->default_value = g_strdup (default_value);
		if (strlen (default_value) > 0)
		{
			/* For combo, initialize the untranslated strings */
			if (object_type == GMAMEUI_PROPERTY_OBJECT_TYPE_COMBO) 
			{
				gchar *old_value;
				gchar **vstr;
				
				vstr = g_strsplit (default_value, ",", 100);
				g_object_set_data(G_OBJECT(p->object), "untranslated",
									vstr);
				old_value = mame_options_get (pr, p->key);
				if (old_value == NULL && vstr[0])
				{
					GMAMEUI_DEBUG ("Setting default pref value: %s = %s",
								 p->key, default_value);
					mame_options_set (pr, p->key, vstr[0]);
				}
				if (old_value)
					g_free (old_value);
			}
			else if (p->data_type != GMAMEUI_PROPERTY_DATA_TYPE_BOOL &&
					 p->data_type != GMAMEUI_PROPERTY_DATA_TYPE_INT)
			{
				gchar *old_value;
				old_value = mame_options_get (pr, p->key);
				if (old_value == NULL)
				{
					GMAMEUI_DEBUG ("Setting default pref value: %s = %s",
								 p->key, default_value);
					mame_options_set (pr, p->key, default_value);
				}
				if (old_value)
					g_free (old_value);
			}
			else
			{/* TODO
				value = gconf_client_get (pr->priv->gclient,
										  build_key (p->key), NULL);
				if (value == NULL)
				{
					GMAMEUI_DEBUG ("Setting default pref value: %s = %s",
								 p->key, default_value);
					if (p->data_type == GMAMEUI_PROPERTY_DATA_TYPE_INT)
						gconf_client_set_int (pr->priv->gclient,
											  build_key (p->key),
											  atoi (default_value), NULL);
					else
						gconf_client_set_bool (pr->priv->gclient,
											   build_key (p->key),
											   atoi (default_value), NULL);
				}*/
/* TODO				if (value)
					gconf_value_free (value);*/
			}
		}
	}
	p->flags = flags;
	p->custom = FALSE;
	p->set_property = NULL;
	p->get_property = NULL;

	GMAMEUI_DEBUG ("Adding MameProperty in hash table under key %s", key);
	g_hash_table_insert (pr->priv->properties, g_strdup (key), p);
	connect_prop_to_object (pr, p);
	register_callbacks (pr, p);

	return TRUE;
}

/**
 * mame_options_register_property_from_string:
 * @pr: a #MameOptions object
 * @object: Widget to register
 * @property_desc: Property description (see mame_options_add_page())
 *
 * This registers only one widget. The widget could be shown elsewhere.
 * the property_description should be of the form described before.
 * 
 * Return value: TRUE if sucessful.
 */
gboolean
mame_options_register_property_from_string (MameOptions *pr,
											GtkWidget *object,
											const gchar *property_desc)
{
	gchar **fields;
	gint  n_fields;
	
	GmameuiPropertyObjectType object_type;
	GmameuiPropertyDataType data_type;
	gchar *key;
	gchar *default_value;
	gint flags;
	
	g_return_val_if_fail (MAME_IS_OPTIONS(pr), FALSE);
	g_return_val_if_fail ((GTK_IS_WIDGET (object)), FALSE);
	g_return_val_if_fail (property_desc != NULL, FALSE);
	
	fields = g_strsplit (property_desc, ":", 5);
	g_return_val_if_fail (fields, FALSE);
	for (n_fields = 0; fields[n_fields]; n_fields++);
	if (n_fields != 5)
	{
		g_strfreev (fields);
		return FALSE;
	}
	object_type = get_object_type_from_string (fields[0]);
	data_type = get_data_type_from_string (fields[1]);
	default_value = fields[2];
	flags = atoi (fields[3]);
	key = fields[4];
	if (object_type < 0)
	{
		g_warning ("Invalid property object type in property description");
		g_strfreev (fields);
		return FALSE;
	}
	if (data_type < 0)
	{
		g_warning ("Invalid property data type in property description");
		g_strfreev (fields);
		return FALSE;
	}
	mame_options_register_property_raw (pr, object, key, default_value,
										flags,  object_type,
										data_type);
	g_strfreev (fields);
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
void
mame_options_register_all_properties_from_glade_xml (MameOptions *pr,
													 GladeXML *gxml,
													 GtkWidget *parent)
{
	GList *widgets;
	GList *node;
	
	g_return_if_fail (MAME_IS_OPTIONS (pr));
	g_return_if_fail (gxml != NULL);
	
	widgets = glade_xml_get_widget_prefix (gxml, "preferences_");
	node = widgets;

	while (node)
	{
		const gchar *name;
		GtkWidget *widget, *p;
		gboolean cont_flag = FALSE;
		
		widget = node->data;
		
		p = gtk_widget_get_parent (widget);
		/* Added only if it's a desendend child of the parent */
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
		GMAMEUI_DEBUG ("Processing widget %s", name);
		if (strncmp (name, PREFERENCE_PROPERTY_PREFIX,
                     strlen (PREFERENCE_PROPERTY_PREFIX)) == 0)
		{
			const gchar *property = &name[strlen (PREFERENCE_PROPERTY_PREFIX)];
			mame_options_register_property_from_string (pr, widget, property);
		}
		node = g_list_next (node);
	}
}

void
mame_options_add_page (MameOptions *pr, GladeXML *gxml,
		       const gchar* glade_widget_name,
		       const gchar* title,
		       const gchar *icon_filename)
{
	GtkWidget *parent;
	GtkWidget *page;
	GdkPixbuf *pixbuf;
//	gchar *image_path;
	
	g_return_if_fail (MAME_IS_OPTIONS (pr));
	g_return_if_fail (glade_widget_name != NULL);
	g_return_if_fail (icon_filename != NULL);
	 
	page = glade_xml_get_widget (gxml, glade_widget_name);
	g_object_ref (page);
						   
	g_return_if_fail (GTK_IS_WIDGET (page));
	
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
//	image_path = icon_filename; /* TODO */
	pixbuf = gmameui_get_icon_from_stock (icon_filename);
	
	mame_options_dialog_add_page (MAME_OPTIONS_DIALOG (pr->priv->prefs_dialog),
								  glade_widget_name, title, pixbuf, page);
	mame_options_register_all_properties_from_glade_xml (pr, gxml, page);
	g_object_unref (page);
//	g_free (image_path);
	g_object_unref (pixbuf);						   
}

static void
property_destroy (MameProperty *property)
{
	g_return_if_fail (property);
	if (property->key) g_free (property->key);
	if (property->default_value) g_free (property->default_value);
	g_object_unref (property->object);
	g_free (property);
}

static void
mame_options_dispose (GObject *obj)
{
	MameOptions *pr = MAME_OPTIONS (obj);
	
	if (pr->priv->properties)
	{
		GMAMEUI_DEBUG ("Destroying MameOptions hash table");
		
		/* This will release the refs on property objects */
		/* FIXME - Need to remove all the elements first? */
		g_hash_table_destroy (pr->priv->properties);
		pr->priv->properties = NULL;
	}
}

static void
mame_options_instance_init (MameOptions *pr)
{
	pr->priv = g_new0 (MameOptionsPriv, 1);
	
	pr->priv->properties = g_hash_table_new_full (g_str_hash,
						      g_str_equal,
						      g_free,
						      (GDestroyNotify) property_destroy);

	/* This gets called from mame_options_new. From that function, also pass in a RomEntry
	   If RomEntry is NULL, get default options, otherwise get the options
	   for a specific ROM */
	pr->priv->filename = g_build_filename (g_get_home_dir (),
					       ".gmameui", "options", "default_new", NULL);
	pr->priv->options_file = g_key_file_new ();
	GError *error = NULL;
	g_key_file_load_from_file (pr->priv->options_file,
				   pr->priv->filename,
				   G_KEY_FILE_KEEP_COMMENTS,
				   &error);

	if (error) {
		GMAMEUI_DEBUG ("options file could not be loaded: %s", error->message);
		g_error_free (error);
		error = NULL;
	}
	
	pr->priv->options_list = NULL;  /* Initialise options list */
	
	/* Register all the options in the Glade file to initialise
	   properties hash table. This is a bit hacky, and only in
	   place to stop segfault when playing game before opening
	   the options screen */
	GladeXML *xml;
	xml = glade_xml_new (GLADEDIR "options.glade", NULL, NULL);
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "Display"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "Sound"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "performance_vbox"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "misc_vbox"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "debugging_vbox"));
	
}

static void
mame_options_finalize (GObject *obj)
{
	MameOptions *pr = MAME_OPTIONS (obj);
GMAMEUI_DEBUG ("Finalising mame_options object");
	if (pr->priv->prefs_dialog)
		gtk_widget_destroy (pr->priv->prefs_dialog);
	
	if (pr->priv->options_file)
		g_key_file_free (pr->priv->options_file);
	g_free (pr->priv->filename);
	
	if (pr->priv->options_list) {
		g_list_foreach (pr->priv->options_list, (GFunc) g_free, NULL);
		g_list_free (pr->priv->options_list);
	}

	g_free (pr->priv);
}

static void
mame_options_class_init (MameOptionsClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	
	object_class->dispose = mame_options_dispose;
	object_class->finalize = mame_options_finalize;
}
