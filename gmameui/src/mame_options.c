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

#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <glade/glade.h>

#include "mame_options.h"
#include "mame_options_dialog.h"
#include "io.h"
#include "gui.h"	/* gmameui_get_icon_from_stock */

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
	GladeXML *xml;
	GKeyFile *options_file;
	gchar *filename;
	GHashTable          *properties;
	GList				*options_list;  /* A list of the set options, of type MameOptionValue */
	GtkWidget           *prefs_dialog;
	gboolean             is_showing;
};

typedef struct {
	gchar *parent;		/* Widget that, when changed, causes state of dependent widget to change */
	gchar *dependent;       /* Dependent widget */
	gchar *relationship;    /* Value that parent needs to be for child to be enabled */
} WidgetRelationship;

static const WidgetRelationship widget_relationships[] =
{
	{ "Video.video", "Video.scalemode", "soft" },
	{ "Sound.sound", "Sound.samples", "1" },
	{ "Sound.sound", "Sound.samplerate", "1" },
	{ "Sound.sound", "Sound.volume", "1" },
	{ "Sound.sound", "Sound.audio_latency", "1" },
	{ "Performance.autoframeskip", "Performance.frameskip", "0" },
	{ "Debugging.log", "Debugging.verbose", "1" },
	{ "Debugging.debug", "Debugging.debugscript", "1" },
	{ "Debugging.debug", "Debugging.oslog", "1" },
};

#define PREFERENCE_PROPERTY_PREFIX "preferences_"

/*
 * Registers all properties defined for widgets below the 'parent' widget
 * in the given gxml glade UI tree
 */
static void
mame_options_register_all_properties_from_glade_xml (MameOptions* pr,
						     GladeXML *gxml,
						     GtkWidget *parent);
static gboolean
mame_options_register_property_from_string (MameOptions *pr,
					    GtkWidget *object,
					    const gchar *property_desc);

static gboolean
mame_options_register_property_raw (MameOptions *pr, GtkWidget *object,
				    const gchar *key, const gchar *default_value,
				    guint flags, GmameuiPropertyObjectType object_type,
				    GmameuiPropertyDataType  data_type);

/* Sets the value (string) of a key */
static void
mame_options_set (MameOptions *pr, const gchar *key, const gchar *value);
									
/* Sets the value (int) of a key */
static void
mame_options_set_int (MameOptions *pr, const gchar *key, const gint value);

/* Sets the value (double) of a key */
static void
mame_options_set_double (MameOptions *pr, const gchar *key, gdouble value);

/* Gets the value (string) of a key */
/* Must free the return string */
static gchar*
mame_options_get (MameOptions *pr, const gchar *key);

/* Gets the value (int) of a key. If not found, 0 is returned */
static gint
mame_options_get_int (MameOptions *pr, const gchar *key);

static void
mame_options_generate_custom_settings (MameOptions *pr);

static void
add_option_to_option_list (MameOptions *pr, const gchar *key, const gchar *value);

/* Functions to split a key and return either the category or the field */
static gchar *
get_key_category (gchar *key);

static gchar *
get_key_field (gchar *key);

static GtkWidget *
get_widget_with_suffix (GladeXML *gxml, gchar *suffix);

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

/* Returns an options string for a particular category (e.g. Performance, Video, etc)
   String needs to be free'd when finished
 
   Within each category, the list of parameters is retrieved from the gmameui.ini keyfile,
   and each parameter checked that it is valid for this version of MAME
 
 */
gchar *
mame_options_get_option_string (MameOptions *pr, gchar *category)
{
	MameExec *exec;
	gchar *options_string;
	gchar **keylist;
	gsize category_len;
	GError *error = NULL;
	MameProperty *prop;
	guint i;
	gchar *togglename;

	g_return_val_if_fail (pr->priv->options_file != NULL, NULL);
	g_return_val_if_fail (g_hash_table_size (pr->priv->properties) != 0, NULL);
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	g_return_val_if_fail (exec != NULL, NULL);
	
	options_string = g_strdup ("");

	keylist = g_key_file_get_keys (pr->priv->options_file, category, &category_len, &error);
	
	if (error) {
		GMAMEUI_DEBUG ("Error retrieving options keys for category %s: %s",
			       category, error->message);
		g_error_free (error);
		return options_string;
	}
	
	for (i = 0; i < category_len; i++) {
		gchar *value;
		gchar *key;
		
		key = g_strdup_printf("%s.%s", category, keylist[i]);
		prop = g_hash_table_lookup (pr->priv->properties, key);
		
		if (!prop) {
			GMAMEUI_DEBUG ("No property found for key %s", key);
		} else {
		
			error = NULL;
			
			/* If this option is a toggle controlling a toggletext
			   (i.e. has a _toggle suffix), skip this value */
			if (g_str_has_suffix (keylist[i], "_toggle"))
				continue;
			
			/* Check that the selected version of MAME supports this option */
			GMAMEUI_DEBUG (_("Checking that MAME supports the option %s..."), keylist[i]);
			if (mame_has_option (exec, keylist[i])) {
				GMAMEUI_DEBUG (_("...it does"));
				
				value = g_key_file_get_value (pr->priv->options_file, category, keylist[i], &error);
				if (error) {
					GMAMEUI_DEBUG (_("Error retrieving options value for key %s in category %s: %s"),
						       keylist[i], category, error->message);
					g_error_free (error);
					break;
				}
		
				switch (prop->data_type) {
					case GMAMEUI_PROPERTY_DATA_TYPE_BOOL:
						/* Only append the value if it is an 'on' state */
						if (g_ascii_strcasecmp (value, "1") == 0)
							options_string = g_strconcat (options_string, " -", keylist[i], NULL);
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
					case GMAMEUI_PROPERTY_DATA_TYPE_TOGGLETEXT:
						/* Check whether the associated toggle (same as the current key value but with
						   a '_toggle' suffix) is on */
						togglename = g_strdup_printf ("%s.%s_toggle", category, keylist[i]);

						if (mame_options_get_int (pr, togglename) == 1)
							options_string = g_strconcat (options_string, " -", keylist[i], " ", value, NULL);
						
						g_free (togglename);
						break;
					case GMAMEUI_PROPERTY_DATA_TYPE_COLOR:
					case GMAMEUI_PROPERTY_DATA_TYPE_FONT:
						g_assert_not_reached ();
						break;
				}       /* Switch */
				g_free (value);

			} else {
				GMAMEUI_DEBUG (_("...it doesn't"));
			}
		}       /* If prop */
		g_free (key);
	}

	if (keylist)
		g_strfreev (keylist);
	
	return options_string;
}

/* Gets the value from the keyfile for the specified key, which should be of the
   format category.key, i.e. Sound.volume */
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

/* Updates a key in the keyfile with the specified value. Key should be in the
   format category.key, i.e. Sound.volume. The value should be a string. */
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
		/*GMAMEUI_DEBUG ("Setting MAME option %s with value %s", stv[0], stv[1]);*/
		g_key_file_set_string (pr->priv->options_file, stv[0], stv[1], value);
		g_key_file_save_to_file (pr->priv->options_file, pr->priv->filename, NULL);
		
		/* Update option in list */
		add_option_to_option_list (pr, key, value);
	}
	g_strfreev (stv);
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

static void
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

/* All the widgets that cannot be handled via the GladeXML naming scheme are
   managed here, e.g. effect name, which is determined by .png files in artwork dir */
static void
mame_options_generate_custom_settings (MameOptions *pr)
{
	/*FIXME TODO
	* Get the GladeXML file *
	get the widget
	add the entries*/
	GtkWidget *box = gtk_combo_box_new_text ();   // FIXME TODO Should be a widget attribute in priv
	gtk_combo_box_append_text (GTK_COMBO_BOX (box), "none");
	gtk_combo_box_append_text (GTK_COMBO_BOX (box), "aperture1x2rb");
	gtk_combo_box_append_text (GTK_COMBO_BOX (box), "aperture1x3rb");
	gtk_combo_box_append_text (GTK_COMBO_BOX (box), "aperture2x4bg");
	gtk_combo_box_append_text (GTK_COMBO_BOX (box), "aperture2x4rb");
	gtk_combo_box_append_text (GTK_COMBO_BOX (box), "aperture4x6");
	gtk_combo_box_append_text (GTK_COMBO_BOX (box), "scanlines");
	gtk_widget_show (box);

	/* Set the signal - in the handler, get the value using
	   gtk_combo_box_get_active_text (); */
	
}

static void 
on_preferences_dialog_destroyed (GtkWidget *preferencess_dialog, MameOptions *pr)
{
	g_object_unref (pr->priv->prefs_dialog);
	
	pr->priv->prefs_dialog = NULL;
}

GtkWidget *
mame_options_get_dialog (MameOptions *pr)
{
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
mame_options_new (void)
{
	/* TODO Pass in object RomEntry - if null, then its default options */
	MameOptions *pr;
	
	pr = g_object_new (MAME_TYPE_OPTIONS, NULL);
	
	return pr;
	
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
	else if (strcmp (data_type, "toggletext") == 0)
		return GMAMEUI_PROPERTY_DATA_TYPE_TOGGLETEXT;
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
	pr = g_object_get_data (G_OBJECT (widget), "MameOptions");
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
		int_value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (prop->object));
		text_value = g_strdup_printf ("%d", int_value);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_HSCALE:
		double_value = gtk_range_get_value (GTK_RANGE (prop->object));
		text_value = g_strdup_printf ("%f", double_value);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_SPIN:
		int_value = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (prop->object));
		text_value = g_strdup_printf ("%d", int_value);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_ENTRY:
		text_value = gtk_editable_get_chars (GTK_EDITABLE (prop->object), 0, -1);
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
	case GMAMEUI_PROPERTY_OBJECT_TYPE_TEXT:
	case GMAMEUI_PROPERTY_OBJECT_TYPE_FOLDER:
	/*case GMAMEUI_PROPERTY_OBJECT_TYPE_FILE:*/
		/* Haven't catered for these yet
		   Folder:
		    text_value = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (prop->object));
		   File:
		    text_value = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (prop->object));
		*/
		g_assert_not_reached ();
		break;
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
	
	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget), "MameOptions"));
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
	
	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget), "MameOptions"));
	p = (MameProperty *) user_data;
	val = get_property_value_as_double (p);

	mame_options_set_double (pr, p->key, val);
}

/* This event does two things - updates the MameProperty object (which triggers
   the key file to be updated) and sets the state of the associated
   text entry field. */
 static void
update_property_on_change_int_toggle (GtkWidget *widget, gpointer user_data)
{
	MameOptions *pr;
	MameProperty *p;
	
	gint val, namelen;
	gchar *toggletext;
	GtkWidget *toggletext_widget;
	
	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget), "MameOptions"));
	p = (MameProperty *) user_data;
	val = get_property_value_as_int (p);
	mame_options_set_int (pr, p->key, val);
	
	/* Get the property name of the associated toggletext controlled by this toggle */
	namelen = strlen (p->key);
	toggletext = g_strndup (p->key, namelen - 7);   /* 7 is length of _toggle */
	
	toggletext_widget = get_widget_with_suffix (pr->priv->xml, toggletext);
	gtk_widget_set_sensitive (toggletext_widget, val);
	
	g_free (toggletext);
}

static gboolean
update_property_on_event_str (GtkWidget *widget, GdkEvent *event,
							  gpointer user_data)
{
	MameOptions *pr;
	MameProperty *p;
	gchar *val;
	
	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget), "MameOptions"));
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

	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget), "MameOptions"));
	p = (MameProperty *) user_data;
	val = get_property_value_as_string (p);
	mame_options_set (pr, p->key, val);
	g_free (val);
}

/* Used with unblock_update_property_on_change_str () to prevent GtkEntry fields
   from triggering an event whenever a new letter is entered;
   i.e. we only want to trigger a 'changed' event when the GtkEntry loses focus */
static gboolean
block_update_property_on_change_str (GtkWidget *widget, GdkEvent *event,
							  gpointer user_data)
{
	MameProperty *p = (MameProperty *) user_data;

	gtk_signal_handler_block_by_func (GTK_OBJECT(p->object), G_CALLBACK (update_property_on_change_str), p);
	return FALSE;
}

/* Used with block_update_property_on_change_str () to prevent GtkEntry fields
   from triggering an event whenever a new letter is entered;
   i.e. we only want to trigger a 'changed' event when the GtkEntry loses focus */
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
// AAA FIXME TODO HACK HACK HACK
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (prop->object),
				                      int_value);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (prop->object),
				                      !int_value);
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
			g_return_if_fail (values != NULL);
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
		case GMAMEUI_PROPERTY_OBJECT_TYPE_FOLDER:
		/*case GMAMEUI_PROPERTY_OBJECT_TYPE_FILE:*/
			/* Not in use at the moment */
			g_assert_not_reached ();
			break;
			/*
			if (value)
				gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (prop->object), value);
				OR
				gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (prop->object), value);
			*/
		}
}

/* When a widget changes value, check to see whether there are any dependent
   widgets that need to have their state set correspondingly */
static void
parent_widget_clicked (GtkWidget *widget, gpointer user_data)
{
	GtkWidget *child;
	guint i;
	
	MameOptions *pr;
	MameProperty *p;
	
	pr = MAME_OPTIONS (g_object_get_data (G_OBJECT (widget), "MameOptions"));
	p = (MameProperty *) user_data;
		
	/* Process each of the widget relationships for the parent */
	for (i = 0; i < G_N_ELEMENTS (widget_relationships); i++) {

		if (strcmp (p->key, widget_relationships[i].parent) == 0) {
			child = get_widget_with_suffix (pr->priv->xml, widget_relationships[i].dependent);
			
			/* Set the state of the children based on the value of the parent */
			gchar *parent_val = get_property_value_as_string (p);

			if (strcmp (parent_val, widget_relationships[i].relationship) == 0)
				gtk_widget_set_sensitive (child, TRUE);
			else
				gtk_widget_set_sensitive (child, FALSE);
		}
	}
}


static void
register_callbacks (MameOptions *pr, MameProperty *p)
{
	g_object_set_data (G_OBJECT (p->object), "MameOptions", pr);
	
	/* Whenever an object value is changed, we not only set the default callback
	   to retrieve the value and update the key file, we also call on_parent_widget_changed ()
	   to process any other widgets based on the value */
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
			g_signal_connect (G_OBJECT(p->object), "changed",
					  G_CALLBACK (parent_widget_clicked), p);
			break;
		case GMAMEUI_PROPERTY_OBJECT_TYPE_TOGGLE:
			/* If this is a toggle controlling a toggletext (i.e. has
			   a suffix _toggle), then we call the callback which controls
			   that link */
			if (g_str_has_suffix (p->key, "_toggle"))
				g_signal_connect (G_OBJECT(p->object), "toggled",
							  G_CALLBACK (update_property_on_change_int_toggle), p);
			else
				g_signal_connect (G_OBJECT(p->object), "toggled",
							  G_CALLBACK (update_property_on_change_int), p);

			g_signal_connect (G_OBJECT(p->object), "toggled",
					  G_CALLBACK (parent_widget_clicked), p);


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

void add_option_to_option_list (MameOptions *pr, const gchar *key, const gchar *value)
{
	MameOptionValue *obj;
	GList *listpointer;
	gboolean found = FALSE;
	
	/* Look for option in list of available options for this exec */

	for (listpointer = g_list_first (pr->priv->options_list);
		 listpointer != NULL;
		 listpointer = g_list_next (listpointer)) {
		
		if (g_ascii_strcasecmp (((MameOptionValue *) (listpointer->data))->key, key) == 0) {
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
				
				/* If the setting doesn't exist in the settings file, add it */
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
				
				/* If the setting doesn't exist in the settings file, add it */
				if (old_value == NULL)
				{
					GMAMEUI_DEBUG ("Setting default pref value: %s = %s",
								 p->key, default_value);
					mame_options_set (pr, p->key, default_value);
				} else
				{
					GMAMEUI_DEBUG ("Existing value already exists: %s = %s",
						       p->key, old_value);
				}
				if (old_value)
					g_free (old_value);
			}
			else
			{
				gchar *old_value;
				old_value = mame_options_get (pr, p->key);
				
				/* If the setting doesn't exist in the settings file, add it */
				if (old_value == NULL)
				{
					GMAMEUI_DEBUG ("Setting default pref value: %s = %s",
								 p->key, default_value);
					mame_options_set (pr, p->key, default_value);
				} else
				{
					GMAMEUI_DEBUG ("Existing value already exists: %s = %s",
						       p->key, old_value);
				}
				if (old_value)
					g_free (old_value);

			}
		}
	}
	
	p->flags = flags;
	p->custom = FALSE;
	p->set_property = NULL;
	p->get_property = NULL;

	g_hash_table_insert (pr->priv->properties, g_strdup (key), p);

//	connect_prop_to_object (pr, p);
	register_callbacks (pr, p);

connect_prop_to_object (pr, p);
	return TRUE;
}

static gchar *
get_key_category (gchar *key)
{
	gchar **stv;
	gchar *category;
	
	stv = g_strsplit (key, ".", 0);
	category = g_strdup (stv[0]);
	g_strfreev (stv);
	
	return category;
}

static gchar *
get_key_field (gchar *key)
{
	gchar **stv;
	gchar *field;
	
	stv = g_strsplit (key, ".", 0);
	field = g_strdup (stv[1]);
	g_strfreev (stv);
	
	return field;
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
											GtkWidget *widget,
											const gchar *property_desc)
{
	gchar **fields;
	gint  n_fields;
	
	GmameuiPropertyObjectType object_type;
	GmameuiPropertyDataType data_type;
	gchar *key;
	gchar *default_value;
	gint flags;
	
	MameExec *exec;
	
	g_return_val_if_fail (MAME_IS_OPTIONS(pr), FALSE);
	g_return_val_if_fail ((GTK_IS_WIDGET (widget)), FALSE);
	g_return_val_if_fail (property_desc != NULL, FALSE);
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	g_return_val_if_fail (exec != NULL, FALSE);
	
	fields = g_strsplit (property_desc, ":", NUM_GMAMEUI_WIDGETNAME_PROPS);
	g_return_val_if_fail (fields, FALSE);
	for (n_fields = 0; fields[n_fields]; n_fields++);
	if (n_fields != NUM_GMAMEUI_WIDGETNAME_PROPS)
	{
		g_strfreev (fields);
		return FALSE;
	}
	object_type = get_object_type_from_string (fields[GMAMEUI_WIDGETNAME_OBJ_TYPE]);
	data_type = get_data_type_from_string (fields[GMAMEUI_WIDGETNAME_DATA_TYPE]);
	default_value = fields[GMAMEUI_WIDGETNAME_DEFAULT_VALUE];
	flags = atoi (fields[GMAMEUI_WIDGETNAME_FLAGS]);
	key = fields[GMAMEUI_WIDGETNAME_KEY];
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
	
	mame_options_register_property_raw (pr, widget, key, default_value,
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
	pixbuf = gmameui_get_icon_from_stock (icon_filename);
	
	mame_options_dialog_add_page (MAME_OPTIONS_DIALOG (pr->priv->prefs_dialog),
								  glade_widget_name, title, pixbuf, page);
	mame_options_register_all_properties_from_glade_xml (pr, gxml, page);
	
	pr->priv->xml = gxml;
	
	g_object_unref (page);
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
	xml = glade_xml_new (GLADEDIR "options.glade", NULL, GETTEXT_PACKAGE);
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "DisplayVBox"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "SoundVBox"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "PerformanceVBox"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "InputVBox"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "MiscVBox"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "DebuggingVBox"));
	mame_options_register_all_properties_from_glade_xml (pr, xml, glade_xml_get_widget (xml, "OpenGLVBox"));
	
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
	
	/* FIXME TODO Free the GladeXML object pr->priv->xml */

	g_free (pr->priv);
}

static void
mame_options_class_init (MameOptionsClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	
	object_class->dispose = mame_options_dispose;
	object_class->finalize = mame_options_finalize;
}
