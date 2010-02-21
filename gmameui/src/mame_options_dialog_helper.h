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

#ifndef _MAME_OPTIONS_DIALOG_HELPER_H
#define _MAME_OPTIONS_DIALOG_HELPER_H

#define PREFERENCE_PROPERTY_PREFIX "preferences_"

enum {
	COL_NAME,
	COL_TITLE,
	COL_PIXBUF,
	COL_WIDGET,
	LAST_COL
};  /* Columns in the liststore */

typedef struct {
	gchar *parent;			/* Widget that, when changed, causes state of dependent widget to change */
	gchar *dependent;       /* Dependent widget */
	gchar *relationship;    /* Value that parent needs to be for child to be enabled */
} WidgetRelationship;

typedef struct {
	gchar *prop_name;       /* e.g. Video.bpp, must match preferences_Video.bpp from Glade */
	gchar *recorded_values;	/* Values to record and line up against the Items */
} _combo_link;

GtkWidget *
get_widget_with_suffix (GtkBuilder *builder, gchar *suffix);

gchar*
get_property_value_as_string (GtkWidget *widget, gchar *key);

gint
get_property_value_as_int (GtkWidget *widget, gchar *key);

gdouble
get_property_value_as_double (GtkWidget *widget, gchar *key);

void
set_property_value_as_string (GtkWidget *widget, gchar *value);

/* Functions to split a key and return either the category or the field */
gchar *
get_key_field (gchar *key);

gchar *
get_key_category (gchar *key);

/* Function to create the sidebar */
void
add_category_columns (GtkTreeView *treeview);

gint
compare_pref_page_func (GtkTreeModel *model,
			GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);

#endif /* MAME_OPTIONS_DIALOG_HELPER_H */