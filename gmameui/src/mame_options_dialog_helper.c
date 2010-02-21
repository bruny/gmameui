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

/* Manage common code between mame_options_legacy_dialog and mame_options_dialog */

#include "common.h"
#include <stdlib.h>	/* Required for atof */
#include <string.h>	/* Required for strlen */
#include "mame_options_dialog_helper.h"
#include "cell-renderer-captioned-image.h"

GtkWidget *
get_widget_with_suffix (GtkBuilder *builder, gchar *suffix)
{
	GSList *widgets;
	GSList *node;
	
	g_return_val_if_fail (builder != NULL, NULL);
	g_return_val_if_fail (suffix != NULL, NULL);
	
	widgets = gtk_builder_get_objects (builder);
	node = g_slist_nth (widgets, 0);

	while (node)
	{
		const gchar *name;
		GtkWidget *widget;
		
		widget = node->data;
		if (GTK_IS_WIDGET (widget)) {
			name = gtk_widget_get_name (widget);

			/* Return widget if it starts with the property prefix
			   and ends with the specified suffix */
			if ((g_ascii_strncasecmp (name, PREFERENCE_PROPERTY_PREFIX,
				                  strlen (PREFERENCE_PROPERTY_PREFIX)) == 0) && 
			    (g_str_has_suffix (name, suffix)))
					return widget;
		}
		node = g_slist_next (node);
	}

	return NULL;
}

gchar*
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
	GMAMEUI_DEBUG ("dbl val is %.2f", double_value);
		text_value = g_strdup_printf ("%.2f", double_value);
		GMAMEUI_DEBUG("text val is %s", text_value);
	} else if (GTK_IS_COMBO_BOX (widget)) {
		/* Note that some options (e.g. effect) that are displayed using
		   ComboBoxes are string options even though the value sent to MAME
		   is an integer; MAME accepts the integer in quotes. */
		gint idx;
		gchar **values_arr;
		
		values = g_object_get_data (G_OBJECT (widget), "untranslated");
		values_arr = g_strsplit (values, ":", -1);
		idx = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));
		if ((idx > -1) && (values_arr[idx] != NULL))
			text_value = g_strdup (values_arr[idx]);
	} else if (GTK_IS_FILE_CHOOSER (widget)) {
GMAMEUI_DEBUG ("Getting file chooser value");
		if (gtk_file_chooser_get_action (GTK_FILE_CHOOSER (widget)) == GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER) {
			text_value = g_strdup (gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (widget)));
		} else if (gtk_file_chooser_get_action (GTK_FILE_CHOOSER (widget)) == GTK_FILE_CHOOSER_ACTION_OPEN) {
			text_value = g_strdup (gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (widget)));
			GMAMEUI_DEBUG ("Getting file chooser value - %s", text_value);
		}
	} else
		GMAMEUI_DEBUG ("get_property_value_as_string: Unsupported widget: %s", key);
/*	switch (prop->object_type)
	{

	case GMAMEUI_PROPERTY_OBJECT_TYPE_ENTRY:
		text_value = gtk_editable_get_chars (GTK_EDITABLE (prop->object), 0, -1);
		break;
	case GMAMEUI_PROPERTY_OBJECT_TYPE_TEXT:

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

gint
get_property_value_as_int (GtkWidget *widget, gchar *key)
{
	gint  int_value;
	gchar *text_value;
	text_value = get_property_value_as_string (widget, key);
	int_value = atoi (text_value);
	g_free (text_value);	
	return int_value;
}	

gdouble
get_property_value_as_double (GtkWidget *widget, gchar *key)
{
	gdouble  double_value;
	gchar *text_value;
	text_value = get_property_value_as_string (widget, key);
	double_value = atof (text_value);
	GMAMEUI_DEBUG ("dbl val is now %0.2f", double_value);
	g_free (text_value);	
	return double_value;
}

void
set_property_value_as_string (GtkWidget *widget, gchar *value)
{
	gint  int_value;
	gdouble double_value;
	gchar* values;
	gint i; 

/* FIXME TODO Should use the default value for the parameter, rather than 0 */
	
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
	} else if (GTK_IS_FILE_CHOOSER (widget)) {
		if (gtk_file_chooser_get_action (GTK_FILE_CHOOSER (widget)) == GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER) {
			gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (widget), value);
		} else if (gtk_file_chooser_get_action (GTK_FILE_CHOOSER (widget)) == GTK_FILE_CHOOSER_ACTION_OPEN) {
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (widget), value);
		}
	}

}

gchar *
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

gchar *
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

void
add_category_columns (GtkTreeView *treeview)
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

	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview),
				     column);
	

}

/* This function fudges the order that items are added to the store. If we
   didn't have this, then the sort order would be alphabetic */
gint
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