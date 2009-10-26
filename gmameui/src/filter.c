/*
 * GMAMEUI
 *
 * Copyright 2008-2009 Andrew Burton <adb@iinet.net.au>
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

#include "filter.h"
#include "gmameui.h"

/* Filter declarations */
struct _GMAMEUIFilterPrivate
{
	
	gchar *name;
	folder_filters_list folderid;
	Columns_type type;
	gboolean is;
	gchar *value;
	gint int_value; /* should be a union with value */
	gboolean update_list;
	GdkPixbuf *pixbuf;
	
};

enum
{
	PROP_0,
	PROP_NAME,
	PROP_FOLDERID,
	PROP_TYPE,
	PROP_IS,
	PROP_VALUE,
	PROP_INTVALUE,
	PROP_UPDATELIST,
	PROP_PIXBUF
};

static void gmameui_filter_finalize (GObject *obj);
static void gmameui_filter_class_init    (GMAMEUIFilterClass *class);
static void gmameui_filter_init (GMAMEUIFilter      *filter);
static void gmameui_filter_set_property (GObject *object,
										 guint prop_id,
										 const GValue *value,
										 GParamSpec *pspec);
static void gmameui_filter_get_property (GObject *object,
										 guint prop_id,
										 GValue *value,
										 GParamSpec *pspec);
void
gmameui_filter_set_pixbuf (GMAMEUIFilter  *source,
						   GdkPixbuf *pixbuf);

G_DEFINE_TYPE (GMAMEUIFilter, gmameui_filter,
			   GTK_TYPE_HBOX);

static void
gmameui_filter_finalize (GObject *obj)
{
	GMAMEUI_DEBUG ("Freeing filter");
	GMAMEUIFilter *fl = GMAMEUI_FILTER (obj);
	
	g_free (fl->priv->name);
	g_free (fl->priv->value);

	/* pixbuf doesn't need to be unref'd as long as it is unref'd when being
	   added to the GtkTreeModel */
	
//	((GObjectClass *) gmameui_filter_parent_class)->finalize (obj);
	GMAMEUI_DEBUG ("Freeing filter - done");
}

static void
gmameui_filter_class_init (GMAMEUIFilterClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	
	object_class->set_property = gmameui_filter_set_property;
	object_class->get_property = gmameui_filter_get_property;
	
	object_class->finalize = gmameui_filter_finalize;
	
	
	g_object_class_install_property (object_class,
									 PROP_NAME,
									 g_param_spec_string ("name",
														  "Filter name",
														  "Name of the filter",
														  NULL,
														  G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
									 PROP_FOLDERID,
									 g_param_spec_int ("folderid",
													   "Folder ID",
													   "ID of the folder",
													   0, 2000,  /* This would ordinarily be NUMBER_FOLDER, but need to maintain space for catver */
													   0, 
													   G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
									 PROP_TYPE,
									 g_param_spec_int ("type",
													   "Filter Type",
													   "Type of the filter",
													   0, NUMBER_COLUMN_TOTAL,
													   0,
													   G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
									 PROP_IS,
									 g_param_spec_boolean ("is",
														   "Is",
														   "Determines whether we are calculating true or false",
														   0,
														   G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
									 PROP_VALUE,
									 g_param_spec_string ("value",
														  "Filter value",
														  "String value of the filter",
														  NULL,
														  G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
									 PROP_INTVALUE,
									 g_param_spec_int ("int_value",
													   "Integer value",
													   "Integer value to compare",
													   0, 1000, /* TODO */
													   0,
													   G_PARAM_READWRITE));
	
	g_object_class_install_property (object_class,
									 PROP_UPDATELIST,
									 g_param_spec_boolean ("update_list",
														   "Update list",
														   "Determines whether the games list needs to be updated",
														   0,
														   G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
									 PROP_PIXBUF,
									 g_param_spec_object ("pixbuf",
														  "Icon",
														  "Filter Icon",
														  GDK_TYPE_PIXBUF,
														  G_PARAM_READWRITE));
	
	g_type_class_add_private (object_class, sizeof (GMAMEUIFilterPrivate));
}

/* Creates the UI for the filters list */
static void
gmameui_filter_init (GMAMEUIFilter *fl)
{
	fl->priv = g_new0 (GMAMEUIFilterPrivate, 1);
}

GtkWidget *
gmameui_filter_new (void)
{
	return g_object_new (GMAMEUI_TYPE_FILTER,
						 NULL);
}


static void
gmameui_filter_set_property (GObject *object,
							 guint prop_id,
							 const GValue *value,
							 GParamSpec *pspec)
{
	GMAMEUIFilterPrivate *priv = GMAMEUI_FILTER_GET_PRIVATE (object);
	GMAMEUIFilter *source = GMAMEUI_FILTER (object);
	
	switch (prop_id) {
		case PROP_NAME:
			g_free (priv->name);
			priv->name = g_strdup (g_value_get_string (value));
			break;
		case PROP_PIXBUF:
			gmameui_filter_set_pixbuf (source, g_value_get_object (value));
			break;
		case PROP_FOLDERID:
			priv->folderid = g_value_get_int (value);
			break;
		case PROP_TYPE:
			priv->type = g_value_get_int (value);
			break;
		case PROP_IS:
			priv->is = g_value_get_boolean (value);
			break;
		case PROP_VALUE:
			priv->value = g_strdup (g_value_get_string (value));
			break;
		case PROP_INTVALUE:
			priv->int_value = g_value_get_int (value);
			break;
		case PROP_UPDATELIST:
			priv->update_list = g_value_get_boolean (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gmameui_filter_get_property (GObject *object,
							 guint prop_id,
							 GValue *value,
							 GParamSpec *pspec)
{
	GMAMEUIFilterPrivate *priv = GMAMEUI_FILTER_GET_PRIVATE (object);
	
	switch (prop_id) {
		case PROP_NAME:
			g_value_set_string (value, priv->name);
			break;
		case PROP_PIXBUF:
			g_value_set_object (value, priv->pixbuf);
			break;
		case PROP_FOLDERID:
			g_value_set_int (value, priv->folderid);
			break;
		case PROP_TYPE:
			g_value_set_int (value, priv->type);
			break;
		case PROP_IS:
			g_value_set_boolean (value, priv->is);
			break;
		case PROP_VALUE:
			g_value_set_string (value, priv->value);
			break;
		case PROP_INTVALUE:
			g_value_set_int (value, priv->int_value);
			break;
		case PROP_UPDATELIST:
			g_value_set_boolean (value, priv->update_list);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

void
gmameui_filter_set_pixbuf (GMAMEUIFilter  *source,
						   GdkPixbuf *pixbuf)
{
	GMAMEUIFilterPrivate *priv = GMAMEUI_FILTER_GET_PRIVATE (source);
	
	g_return_if_fail (GMAMEUI_IS_FILTER (source));
	g_return_if_fail (pixbuf != NULL);
	
	if (priv->pixbuf != NULL) {
		g_object_unref (priv->pixbuf);
	}
	
	priv->pixbuf = pixbuf;
	
	if (priv->pixbuf != NULL) {
		g_object_ref (priv->pixbuf);
	}
}
