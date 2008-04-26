/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code
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

#ifndef _MAME_OPTIONS_H
#define _MAME_OPTIONS_H

#include <glade/glade.h>
#include "mame_options_dialog.h"


G_BEGIN_DECLS

typedef enum
{
	GMAMEUI_PROPERTY_OBJECT_TYPE_TOGGLE,
	GMAMEUI_PROPERTY_OBJECT_TYPE_SPIN,
	GMAMEUI_PROPERTY_OBJECT_TYPE_HSCALE,
	GMAMEUI_PROPERTY_OBJECT_TYPE_ENTRY,
	GMAMEUI_PROPERTY_OBJECT_TYPE_COMBO,
	GMAMEUI_PROPERTY_OBJECT_TYPE_TEXT,
	GMAMEUI_PROPERTY_OBJECT_TYPE_FOLDER
} GmameuiPropertyObjectType;

typedef enum
{
	GMAMEUI_PROPERTY_DATA_TYPE_BOOL,
	GMAMEUI_PROPERTY_DATA_TYPE_INT,
	GMAMEUI_PROPERTY_DATA_TYPE_DOUBLE,
	GMAMEUI_PROPERTY_DATA_TYPE_TEXT,
	GMAMEUI_PROPERTY_DATA_TYPE_COLOR,
	GMAMEUI_PROPERTY_DATA_TYPE_FONT
} GmameuiPropertyDataType;

typedef struct _MameProperty       MameProperty;
typedef struct _MameOptionValue    MameOptionValue;

#define MAME_TYPE_OPTIONS        (mame_options_get_type ())
#define MAME_OPTIONS(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_OPTIONS, MameOptions))
#define MAME_OPTIONS_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_OPTIONS, MameOptionsClass))
#define MAME_IS_OPTIONS(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_OPTIONS))
#define MAME_IS_OPTIONS_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_OPTIONS))

typedef struct _MameOptions        MameOptions;
typedef struct _MameOptionsClass   MameOptionsClass;
typedef struct _MameOptionsPriv    MameOptionsPriv;

struct _MameOptions
{
	GObject parent;
	
	/*< private >*/
	MameOptionsPriv *priv;
};

struct _MameOptionsClass
{
	GObjectClass parent;
};

GType mame_options_get_type (void);

void mame_options_add_page (MameOptions* pr, GladeXML *gxml,
							const gchar* glade_widget_name,
							const gchar* title,
							const gchar *icon_filename);

/*
 * Registers all properties defined for widgets below the 'parent' widget
 * in the given gxml glade UI tree
 */
void mame_options_register_all_properties_from_glade_xml (MameOptions* pr,
														  GladeXML *gxml,
														  GtkWidget *parent);
gboolean
mame_options_register_property_from_string (MameOptions *pr,
											GtkWidget *object,
											const gchar *property_desc);

gboolean
mame_options_register_property_raw (MameOptions *pr, GtkWidget *object,
									const gchar *key,
									const gchar *default_value,
									guint flags,
									GmameuiPropertyObjectType object_type,
									GmameuiPropertyDataType  data_type);

/* Sets the value (string) of a key */
void mame_options_set (MameOptions *pr,
					   const gchar *key,
					   const gchar *value);
									
/* Sets the value (int) of a key */
void mame_options_set_int (MameOptions *pr,
						   const gchar *key,
						   const gint value);

/* Gets the value (string) of a key */
/* Must free the return string */
gchar * mame_options_get (MameOptions *pr,
						  const gchar *key);

/* Gets the value (int) of a key. If not found, 0 is returned */
gint mame_options_get_int (MameOptions *pr,
						   const gchar *key);

void add_option_to_option_list (MameOptions *pr, gchar *key, gchar *value);

G_END_DECLS

#endif
